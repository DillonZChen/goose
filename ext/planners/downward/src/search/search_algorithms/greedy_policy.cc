#include "greedy_policy.h"

#include "../algorithms/ordered_set.h"
#include "../evaluation_context.h"
#include "../evaluator.h"
#include "../open_list_factory.h"
#include "../plugins/options.h"
#include "../plugins/plugin.h"
#include "../pruning_method.h"
#include "../task_utils/successor_generator.h"
#include "../task_utils/task_properties.h"
#include "../utils/logging.h"
#include "../utils/system.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <optional>
#include <set>

using namespace std;

namespace greedy_policy {
  GreedyPolicy::GreedyPolicy(
    const shared_ptr<Evaluator> &h, const int s,
    OperatorCost cost_type, int bound, double max_time,
    const string &description, utils::Verbosity verbosity)
    : SearchAlgorithm(
          cost_type, bound, max_time, description, verbosity),
        reopen_closed_nodes(false),
        evaluator(h),
        current_eval_context(state_registry.get_initial_state(), &statistics),
        gen(s) {}

  void GreedyPolicy::initialize() {
    log << "Executing greedy policy, (real) bound = " << bound << endl;

    /*
        Collect path-dependent evaluators that are used in the heuristic_function.
        They are usually also used in the open list and will hence already be
        included, but we want to be sure.
    */
    if (evaluator) {
      evaluator->get_path_dependent_evaluators(path_dependent_evaluators);
    }

    State initial_state = state_registry.get_initial_state();
    for (Evaluator *evaluator : path_dependent_evaluators) {
      evaluator->notify_initial_state(initial_state);
    }

    if (task_properties::is_goal_state(task_proxy, initial_state)) {
      std::cout << "Initial state is the goal state!" << std::endl;
      exit(0);
    }

    bool dead_end = current_eval_context.is_evaluator_value_infinite(evaluator.get());
    statistics.inc_evaluated_states();
    print_initial_evaluator_values(current_eval_context);

    if (dead_end) {
      log << "Initial state is a dead end, no solution" << endl;
      if (evaluator->dead_ends_are_reliable())
        utils::exit_with(utils::ExitCode::SEARCH_UNSOLVABLE);
      else
        utils::exit_with(utils::ExitCode::SEARCH_UNSOLVED_INCOMPLETE);
    }

    node = std::make_shared<SearchNode>(search_space.get_node(current_eval_context.get_state()));
    node->open_initial();
  }

  void GreedyPolicy::print_statistics() const {
    statistics.print_detailed_statistics();
    search_space.print_statistics();
  }

  SearchStatus GreedyPolicy::step() {
    State s = node->get_state();
    vector<OperatorID> applicable_ops;
    successor_generator.generate_applicable_ops(s, applicable_ops);
    statistics.inc_expanded();

    vector<State> succ_states;
    vector<int> hs;

    for (OperatorID op_id : applicable_ops) {
      OperatorProxy op = task_proxy.get_operators()[op_id];
      if ((node->get_real_g() + op.get_cost()) >= bound)
        continue;

      State succ_state = state_registry.get_successor_state(s, op);
      EvaluationContext eval_context(succ_state, &statistics);
      int h = eval_context.get_evaluator_value(evaluator.get());

      statistics.inc_generated();
      statistics.inc_evaluated_states();

      if (task_properties::is_goal_state(task_proxy, succ_state)) {
        log << "Solution found!" << endl;
        SearchNode succ_node = search_space.get_node(succ_state);
        succ_node.open_new_node(*node, op, get_adjusted_cost(op));
        Plan plan;
        search_space.trace_path(succ_state, plan);
        set_plan(plan);
        return SOLVED;
      }

      succ_states.push_back(succ_state);
      hs.push_back(h);
    }

    if (succ_states.empty()) {
      log << "No successors available and not at goal!" << endl;
      return FAILED;
    }

    // Get indices of succ_states with the smallest h value
    vector<int> best_indices;
    int best_h = numeric_limits<int>::max();
    for (size_t i = 0; i < succ_states.size(); ++i) {
      if (hs[i] < best_h) {
        best_h = hs[i];
        best_indices.clear();
      }
      if (hs[i] == best_h) {
        best_indices.push_back(i);
      }
    }

    // Randomly select one of the best states
    int best_index = best_indices[gen() % best_indices.size()];
    SearchNode succ_node = search_space.get_node(succ_states[best_index]);
    OperatorProxy op = task_proxy.get_operators()[applicable_ops[best_index]];
    succ_node.open_new_node(*node, op, get_adjusted_cost(op));

    node = std::make_shared<SearchNode>(succ_node);

    // std::cout << op.get_name() << std::endl;

    return IN_PROGRESS;
  }

  class GreedyPolicyFeature : public plugins::TypedFeature<SearchAlgorithm, GreedyPolicy> {
   public:
    GreedyPolicyFeature() : TypedFeature("gp") {
      document_title("Greedy policy");
      document_synopsis("");

      add_option<shared_ptr<Evaluator>>("h", "heuristic");
      add_option<int>("s", "seed", "0");
      add_search_algorithm_options_to_feature(*this, "gp");
    }

    virtual shared_ptr<GreedyPolicy> create_component(
        const plugins::Options &opts,
        const utils::Context &) const override {
        return plugins::make_shared_from_arg_tuples<GreedyPolicy>(
            opts.get<shared_ptr<Evaluator>>("h"),
            opts.get<int>("s"),
            get_search_algorithm_arguments_from_options(opts)
            );
    }
  };

  static plugins::FeaturePlugin<GreedyPolicyFeature> _plugin;
}  // namespace greedy_policy
