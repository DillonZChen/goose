#include "greedy_batch_best_first_search.h"
#include "search.h"
#include "utils.h"

#include "../action.h"

#include "../heuristics/heuristic.h"
#include "../open_lists/greedy_open_list.h"
#include "../states/extensional_states.h"
#include "../states/sparse_states.h"
#include "../successor_generators/successor_generator.h"
#include "../utils/timer.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

template <class PackedStateT>
utils::ExitCode GreedyBatchBestFirstSearch<PackedStateT>::search(const Task &task,
                                                            SuccessorGenerator &generator,
                                                            Heuristic &heuristic)
{
    cout << "Starting greedy batch best first search" << endl;
    double timer_start = get_wall_time();
    StatePackerT packer(task);

    GreedyOpenList queue;

    SearchNode& root_node = space.insert_or_get_previous_node(packer.pack(task.initial_state), LiftedOperatorId::no_operator, StateID::no_state);
    utils::Timer t;
    heuristic_layer = heuristic.compute_heuristic(task.initial_state, task);
    t.stop();
    cout << "Time to evaluate initial state: " << t() << endl;
    root_node.open(0, heuristic_layer);
    if (heuristic_layer == numeric_limits<int>::max()) {
        cerr << "Initial state is unsolvable!" << endl;
        exit(1);
    }
    statistics.inc_evaluations();
    cout << "Initial heuristic value " << heuristic_layer << endl;
    statistics.report_f_value_progress(heuristic_layer);
    queue.do_insertion(root_node.state_id, make_pair(heuristic_layer, 0));

    if (check_goal(task, generator, timer_start, task.initial_state, root_node, space)) return utils::ExitCode::SUCCESS;

    while (not queue.empty()) {
        StateID sid = queue.remove_min();
        SearchNode &node = space.get_node(sid);
        int h = node.h;
        int g = node.g;
        if (node.status == SearchNode::Status::CLOSED) {
            continue;
        }
        node.close();
        statistics.report_f_value_progress(h); // In GBFS f = h.
        statistics.inc_expanded();

        if (h < heuristic_layer) {
            heuristic_layer = h;
            cout << "New heuristic value expanded: h=" << h
                 << " [expansions: " << statistics.get_expanded()
                 << ", evaluations: " << statistics.get_evaluations()
                 << ", generations: " << statistics.get_generated()
                 << ", time: " << get_wall_time() - timer_start << "]" << '\n';
        }
        assert(sid.id() >= 0 && (unsigned) sid.id() < space.size());

        DBState state = packer.unpack(space.get_state(sid));
        if (check_goal(task, generator, timer_start, state, node, space)) return utils::ExitCode::SUCCESS;

        // Let's expand the state, one schema at a time. If necessary, i.e. if it really helps
        // performance, we could implement some form of std iterator
        std::vector<DBState> succs;
        std::vector<int> action_costs;
        std::vector<SearchNode*> child_nodes;
        for (const auto& action:task.get_action_schemas()) {
            auto applicable = generator.get_applicable_actions(action, state);
            statistics.inc_generated(applicable.size());

            for (const LiftedOperatorId& op_id:applicable) {
                DBState succ = generator.generate_successor(op_id, action, state);
                
                auto& child_node = space.insert_or_get_previous_node(packer.pack(succ), op_id, node.state_id);
                if (child_node.status == SearchNode::Status::NEW) {
                  succs.push_back(succ);
                  action_costs.push_back(action.get_cost());
                  child_nodes.push_back(&child_node);
                }
            }
        }

        if (succs.size() > 0) {
            std::vector<int> heuristics = heuristic.compute_heuristic_batch(succs, task);

            for (size_t i=0; i < heuristics.size(); i++) {
                auto& child_node = child_nodes[i];
                int dist = g + action_costs[i];
                int new_h = heuristics[i];
                statistics.inc_evaluations();
                if (new_h == UNSOLVABLE_STATE) {
                    if (child_node->status == SearchNode::Status::NEW) {
                        // Only increase statistics for new dead-ends
                        child_node->open(dist, new_h);
                        statistics.inc_dead_ends();
                        statistics.inc_pruned_states();
                    }
                    continue;
                }

                // guaranteed new node from above
                child_node->open(dist, new_h);
                statistics.inc_evaluated_states();
                queue.do_insertion(child_node->state_id, make_pair(new_h, dist));
            }
        }
    }

    print_no_solution_found(timer_start);

    return utils::ExitCode::SEARCH_UNSOLVABLE;
}

template <class PackedStateT>
void GreedyBatchBestFirstSearch<PackedStateT>::print_statistics() const {
    statistics.print_detailed_statistics();
    space.print_statistics();
}

// explicit template instantiations
template class GreedyBatchBestFirstSearch<SparsePackedState>;
template class GreedyBatchBestFirstSearch<ExtensionalPackedState>;