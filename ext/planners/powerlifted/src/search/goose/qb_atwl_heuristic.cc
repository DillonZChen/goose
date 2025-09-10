#include "qb_atwl_heuristic.h"
#include "wl_utils.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"

using namespace std;

QbAtWlHeuristic::QbAtWlHeuristic(const Options &opts,
                                 const Task &task,
                                 std::shared_ptr<Heuristic> heuristic)
    : QbHeuristic(opts, task, heuristic)
{
    const planning::Domain domain = wl_utils::get_wlplan_domain(task);
    planning::Problem problem = wl_utils::get_wlplan_problem(domain, task);
    pwl_index_to_predicate = wl_utils::get_pwl_index_to_predicate(domain, task);
    model = std::make_shared<feature_generator::WLFeatures>(domain, "ilg", 2, "none", true);
    model->set_problem(problem);
    model->be_quiet();

    size_t n_relations = task.initial_state.get_relations().size();
    atom_to_lowest_h.resize(n_relations);

    for (size_t i = 0; i < n_relations; ++i) {
        atom_to_lowest_h[i] = NoveltySet();
    }
}

int QbAtWlHeuristic::compute_heuristic(const DBState &s, const Task &task)
{
    cached_heuristic = original_heuristic->compute_heuristic(s, task);

    int nov_h = 0;  // always non-positive; eqn. 2 katz. et al 2017
    int non_h = 0;  // always non-negative; eqn. 3 katz. et al 2017

    // WL part
    planning::State wl_state = wl_utils::to_wlplan_state(s, task, pwl_index_to_predicate);
    std::unordered_map<int, int> features = model->collect_embed(wl_state);
    for (const std::pair<const int, int> &feat : features) {
        if (feat.second == 0) {  // feature not present, their values do not matter
            continue;
        }
        bool in_map = feat_to_lowest_h.count(feat) > 0;
        if (!in_map || cached_heuristic < feat_to_lowest_h[feat]) {
            feat_to_lowest_h[feat] = cached_heuristic;
            nov_h -= 1;
        }
        else if (in_map && cached_heuristic > feat_to_lowest_h[feat]) {
            non_h += 1;
        }
    }

    // PN part
    // nullary
    const vector<bool> &nullary_atoms = s.get_nullary_atoms();
    for (size_t i = 0; i < nullary_atoms.size(); ++i) {
        if (!nullary_atoms[i])
            continue;
        bool in_map = nullary_mapping.count(i) > 0;
        if (!in_map || cached_heuristic < nullary_mapping[i]) {
            nullary_mapping[i] = cached_heuristic;
            nov_h -= 1;
        }
        else if (in_map && cached_heuristic > nullary_mapping[i]) {
            non_h += 1;
        }
    }

    // n-ary
    for (const Relation &relation : s.get_relations()) {
        int i = relation.predicate_symbol;
        for (const GroundAtom &f : relation.tuples) {
            bool in_map = atom_to_lowest_h[i].count(f) > 0;
            if (!in_map || cached_heuristic < atom_to_lowest_h[i][f]) {
                atom_to_lowest_h[i][f] = cached_heuristic;
                nov_h -= 1;
            }
            else if (in_map && cached_heuristic > atom_to_lowest_h[i][f]) {
                non_h += 1;
            }
        }
    }


    int h = nov_h < 0 ? nov_h : non_h;

    return h;
}

void QbAtWlHeuristic::print_statistics()
{
    std::cout << "Number of collected features: " << feat_to_lowest_h.size() << std::endl;
}
