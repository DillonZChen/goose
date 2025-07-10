#include "qb_pn_heuristic.h"
#include "wl_utils.h"

#include <cassert>

using namespace std;

QbPnHeuristic::QbPnHeuristic(const Options &opts,
                             const Task &task,
                             std::shared_ptr<Heuristic> heuristic)
    : QbHeuristic(opts, task, heuristic)
{
    size_t n_relations = task.initial_state.get_relations().size();
    atom_to_lowest_h.resize(n_relations);

    for (size_t i = 0; i < n_relations; ++i) {
        atom_to_lowest_h[i] = NoveltySet();
    }
}

int QbPnHeuristic::compute_heuristic(const DBState &s, const Task &task)
{
    cached_heuristic = original_heuristic->compute_heuristic(s, task);

    int nov_h = 0;  // always non-positive; eqn. 2 katz. et al 2017
    int non_h = 0;  // always non-negative; eqn. 3 katz. et al 2017

    // nullary
    const vector<bool> &nullary_atoms = s.get_nullary_atoms();
    for (size_t i = 0; i < nullary_atoms.size(); ++i) {
        if (!nullary_atoms[i])
            continue;
        bool in_map = nullary_mapping.count(i) > 0;
        if (!in_map || nullary_mapping[i] < cached_heuristic) {
            nullary_mapping[i] = cached_heuristic;
            nov_h -= 1;
        }
        else if (in_map && nullary_mapping[i] > cached_heuristic) {
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

void QbPnHeuristic::print_statistics()
{
    int size = nullary_mapping.size();
    for (const auto &entry : atom_to_lowest_h) {
        size += entry.size();
    }
    std::cout << "Number of collected features: " << size << std::endl;
}
