#include "qb_wl_heuristic.h"
#include "wl_utils.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"

using namespace std;

QbWlHeuristic::QbWlHeuristic(const Options &opts,
                             const Task &task,
                             std::shared_ptr<Heuristic> heuristic)
    : QbHeuristic(opts, task, heuristic)
{
    const planning::Domain domain = wl_utils::get_wlplan_domain(task);
    planning::Problem problem = wl_utils::get_wlplan_problem(domain, task);
    pwl_index_to_predicate = wl_utils::get_pwl_index_to_predicate(domain, task);
    model = std::make_shared<feature_generator::WLFeatures>(domain, "ilg", 4, "none", true);
    model->set_problem(problem);
    model->be_quiet();
}

int QbWlHeuristic::compute_heuristic(const DBState &s, const Task &task)
{
    cached_heuristic = original_heuristic->compute_heuristic(s, task);

    int nov_h = 0;  // always non-positive
    planning::State wl_state = wl_utils::to_wlplan_state(s, task, pwl_index_to_predicate);
    model->collect(wl_state);
    std::vector<double> embed = model->embed_state(wl_state);  // TODO optimise this
    for (int i = 0; i < (int)embed.size(); i++) {
        if (embed[i] == 0) {  // feature not present, their values do not matter
            continue;
        }
        std::pair<int, int> feat = std::make_pair(i, (int)embed[i]);
        if (feat_to_lowest_h.count(feat) == 0 || cached_heuristic < feat_to_lowest_h[feat]) {
            feat_to_lowest_h[feat] = cached_heuristic;
            nov_h -= 1;
        }
    }

    return nov_h;
}

void QbWlHeuristic::print_statistics()
{
    std::cout << "Number of collected features: " << feat_to_lowest_h.size() << std::endl;
}
