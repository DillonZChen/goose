#include "wlgoose_heuristic.h"
#include "wl_utils.h"

#include <cassert>

using namespace std;

WlGooseHeuristic::WlGooseHeuristic(const Options &opts, const Task &task)
{
    model = load_feature_generator(opts.get_goose_model_path());
    const planning::Domain domain = *(model->get_domain());
    pwl_index_to_predicate = wl_utils::get_pwl_index_to_predicate(domain, task);
    planning::Problem problem = wl_utils::get_wlplan_problem(domain, task);
    model->set_problem(problem);
}


int WlGooseHeuristic::compute_heuristic(const DBState &s, const Task &task)
{
    planning::State state = wl_utils::to_wlplan_state(s, task, pwl_index_to_predicate);
    double h = model->predict(state);
    int h_round = static_cast<int>(std::round(h));

    return h_round;
}

void WlGooseHeuristic::print_statistics()
{
    // TODO
}
