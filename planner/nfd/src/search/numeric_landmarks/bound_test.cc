#include "bound_test.h"

#include <vector>

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"
#include "../task_tools.h"

namespace bound_test {
  BoundTestHeuristic::BoundTestHeuristic(const Options &opts)
    : Heuristic(opts),
      numeric_task_proxy(numeric_helper::NumericTaskProxy(task_proxy, true, false, opts.get<ap_float>("epsilon"), opts.get<ap_float>("precision"))),
      bound(numeric_task_proxy, opts.get<ap_float>("precision")) {
    auto initial_state = task_proxy.get_initial_state();
    auto variables = task_proxy.get_variables();
    size_t n_numeric_variables = numeric_task_proxy.get_n_numeric_variables();
    std::vector<double> state(n_numeric_variables);

    for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
      auto id = numeric_task_proxy.get_numeric_variable(var_id).id_abstract_task;
      state[var_id] = initial_state.nval(id);
    }

    bound.calculate_bounds(state, opts.get<int>("bound_iterations"));
    bound.dump(task_proxy);
  }

  BoundTestHeuristic::~BoundTestHeuristic() {}

  ap_float BoundTestHeuristic::compute_heuristic(const GlobalState &global_state) {
    return 0.0;
  }

  static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Bound Test", "");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    parser.add_option<ap_float>("precision", "values less than this value are considered as zero", "0.000001");
    parser.add_option<ap_float>("epsilon", "small value added to strict inequalities", "0");
    parser.add_option<int>("bound_iterations", "the maximum number of iterations to extract bounds", "0");
    
    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
      return nullptr;
    else
      return new BoundTestHeuristic(opts);
  }

  static Plugin<Heuristic> _plugin("bound_test", _parse);
}