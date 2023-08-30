#include "eager_search.h"

#include "../evaluation_context.h"
#include "../evaluator.h"
#include "../open_list_factory.h"
#include "../pruning_method.h"

#include "../algorithms/ordered_set.h"
#include "../plugins/options.h"
#include "../task_utils/successor_generator.h"
#include "../utils/logging.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <optional.hh>
#include <set>
#include <typeinfo>

using namespace std;

namespace eager_search {
EagerSearch::EagerSearch(const plugins::Options &opts)
    : SearchEngine(opts),
      reopen_closed_nodes(opts.get<bool>("reopen_closed")),
      open_list(opts.get<shared_ptr<OpenListFactory>>("open")->
                create_state_open_list()) {
    const vector<shared_ptr<Evaluator>> &evals = opts.get_list<shared_ptr<Evaluator>>("evals");
    heuristic = evals[0];
}

void EagerSearch::initialize() {
    log << "Conducting best first search"
        << (reopen_closed_nodes ? " with" : " without")
        << " reopening closed nodes, (real) bound = " << bound
        << endl;
    assert(open_list);

    set<Evaluator *> evals;
    open_list->get_path_dependent_evaluators(evals);

    path_dependent_evaluators.assign(evals.begin(), evals.end());

    State initial_state = state_registry.get_initial_state();
    for (Evaluator *evaluator : path_dependent_evaluators) {
        evaluator->notify_initial_state(initial_state);
    }

    /*
      Note: we consider the initial state as reached by a preferred
      operator.
    */
    EvaluationContext eval_context(initial_state, 0, true, &statistics);

    statistics.inc_evaluated_states();

    best_h = heuristic->compute_result_single(initial_state);

    if (open_list->is_dead_end(eval_context)) {
        log << "Initial state is a dead end." << endl;
    } else {
        if (search_progress.check_progress(eval_context))
            statistics.print_checkpoint_line(0);
        SearchNode node = search_space.get_node(initial_state);
        node.open_initial();
        node.set_h(best_h);

        open_list->insert(eval_context, initial_state.get_id());
    }
}

void EagerSearch::print_statistics() const {
    statistics.print_detailed_statistics();
    search_space.print_statistics();
}

SearchStatus EagerSearch::step() {
    tl::optional<SearchNode> node;

    // Get first node in queue that is not closed
    while (true) {
        if (open_list->empty()) {
            log << "Completely explored state space -- no solution!" << endl;
            return FAILED;
        }
        StateID id = open_list->remove_min();
        State s = state_registry.lookup_state(id);
        node.emplace(search_space.get_node(s));

        if (node->is_closed())
            continue;

        node->close();
        assert(!node->is_dead_end());
        statistics.inc_expanded();
        break;
    }

    const State &s = node->get_state();
    if (check_goal_and_set_plan(s))
        return SOLVED;

    int h = node->get_h();
    if (h < best_h) {
        best_h = h;
        log << "New heuristic value expanded: h=" << h
            << " [expansions: " << statistics.get_expanded()
            << ", evaluations: " << statistics.get_evaluations()
            << ", generations: " << statistics.get_generated()
            // << ", time: " << get_wall_time() - timer_start
            << "]" << '\n';
    }

    vector<OperatorID> applicable_ops;
    successor_generator.generate_applicable_ops(s, applicable_ops);

    for (OperatorID op_id : applicable_ops) {
        OperatorProxy op = task_proxy.get_operators()[op_id];
        if ((node->get_g() + op.get_cost()) >= bound)
          continue;

        State succ_state = state_registry.get_successor_state(s, op);
        statistics.inc_generated();

        SearchNode succ_node = search_space.get_node(succ_state);

        // Previously encountered dead end. Don't re-evaluate.
        if (succ_node.is_dead_end())
            continue;

        if (succ_node.is_new()) {
            // We have not seen this state before.
            // Evaluate and create a new node.
            h = heuristic->compute_result_single(succ_state);
            statistics.inc_evaluated_states();
            statistics.inc_evaluations();
            succ_node.open(*node, op, h);
            open_list->insert(h, succ_state.get_id());
        }
    }

    return IN_PROGRESS;
}

void EagerSearch::reward_progress() {
    // Boost the "preferred operator" open lists somewhat whenever
    // one of the heuristics finds a state with a new best h value.
    open_list->boost_preferred();
}

void EagerSearch::dump_search_space() const {
    search_space.dump(task_proxy);
}

void add_options_to_feature(plugins::Feature &feature) {
    SearchEngine::add_pruning_option(feature);
    SearchEngine::add_options_to_feature(feature);
}
}
