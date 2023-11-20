#include "batch_lazy_search.h"

#include "search.h"
#include "utils.h"

#include "../action.h"
#include "../task.h"

#include "../heuristics/heuristic.h"
#include "../open_lists/greedy_open_list.h"
#include "../states/extensional_states.h"
#include "../states/sparse_states.h"

#include "../successor_generators/successor_generator.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

using namespace std;

template <class PackedStateT>
utils::ExitCode BatchLazySearch<PackedStateT>::search(const Task &task,
                                                 SuccessorGenerator &generator,
                                                 Heuristic &heuristic)
{
    cout << "Starting batch lazy search" << endl;
    double timer_start = get_wall_time();
    StatePackerT packer(task);

    // TODO: learn preferred operators
    // GreedyOpenList preferred_open_list;
    // GreedyOpenList regular_open_list;
    GreedyOpenList open_list;

    //cout << "@ Initial state: \n\t";
    //task.dump_state(task.initial_state);

    SearchNode& root_node = space.insert_or_get_previous_node(packer.pack(task.initial_state), LiftedOperatorId::no_operator, StateID::no_state);
    heuristic_layer = heuristic.compute_heuristic(task.initial_state, task);
    root_node.open(0, heuristic_layer);
    if (heuristic_layer == UNSOLVABLE_STATE) {
        cerr << "Initial state is unsolvable!" << endl;
        exit(1);
    }
    statistics.inc_evaluations();
    cout << "Initial heuristic value " << heuristic_layer << endl;
    statistics.report_f_value_progress(heuristic_layer);
    open_list.do_insertion(root_node.state_id, make_pair(heuristic_layer, 0));

    if (check_goal(task, generator, timer_start, task.initial_state, root_node, space)) return utils::ExitCode::SUCCESS;

    int batch_size = 1;  // adaptive batch size
    int iters_since_batch = 0;

    while (not open_list.empty()) {
        StateID sid = open_list.remove_min(); //regular_open_list.remove_min();
        SearchNode &node = space.get_node(sid);
        DBState state = packer.unpack(space.get_state(sid));
        if (node.status == SearchNode::Status::CLOSED) {
            continue;
        }
        node.close();

        if (node.h == -1) {
            if (iters_since_batch >= batch_size) {
              batch_size *= 2;
            } else {
              batch_size /= 2;
            }

            // upper limit to prevent gpu mem issues; TODO max batch size as parameter
            batch_size = max(1, min(batch_size, 64)); 
            // printf("Iter %d; Batch size is now %d\n", statistics.get_expanded(), batch_size);

            // add one to evaluate current state
            size_t to_eval = static_cast<size_t>(batch_size + 1);
            iters_since_batch = 0;
            std::vector<StateID> sids;
            sids.push_back(sid);
            for (const auto& it: open_list.get_buckets()) {
                for (const auto& q_sid: it.second) {
                    if (space.get_node(q_sid).h == -1) {
                        sids.push_back(q_sid);
                    }
                    if (sids.size() == to_eval) goto endloop;
                }
            }
          endloop:
            std::vector<DBState> states;
            for (const auto& q_sid: sids) {
                states.push_back(packer.unpack(space.get_state(q_sid)));
            }
            std::vector<int> hs = heuristic.compute_heuristic_batch(states, task);
            statistics.inc_evaluations(sids.size());
            statistics.inc_evaluated_states(sids.size());
            for (size_t i = 0; i < sids.size(); i++) {
                space.get_node(sids[i]).update_h(hs[i]);
            }
        } else {
          iters_since_batch++;
        }

        int h = node.h;
        if (h == UNSOLVABLE_STATE) {
            statistics.inc_dead_ends();
            statistics.inc_pruned_states();
            node.mark_as_unsolvable();
            continue;
        }
        int g = node.g;
        statistics.report_f_value_progress(h); // In GBFS f = h.

        statistics.inc_expanded();

        if (h < heuristic_layer) {
            heuristic_layer = h;
            boost_priority_preferred();
            cout << "New heuristic value expanded: h=" << h
                 << " [expansions: " << statistics.get_expanded()
                 << ", evaluations: " << statistics.get_evaluations()
                 << ", generations: " << statistics.get_generated()
                 << ", time: " << get_wall_time() - timer_start << "]" << '\n';
        }
        assert(sid.id() >= 0 && (unsigned) sid.id() < space.size());

        if (check_goal(task, generator, timer_start, state, node, space)) return utils::ExitCode::SUCCESS;

        // Let's expand the state, one schema at a time. If necessary, i.e. if it really helps
        // performance, we could implement some form of std iterator
        for (const auto& action:task.get_action_schemas()) {
            auto applicable = generator.get_applicable_actions(action, state);
            statistics.inc_generated(applicable.size());

            for (const LiftedOperatorId& op_id:applicable) {
                DBState s = generator.generate_successor(op_id, action, state);
                int dist = g + action.get_cost();
                auto &child_node =
                    space.insert_or_get_previous_node(packer.pack(s), op_id, node.state_id);
//                bool is_preferred = is_useful_operator(task, s, heuristic.get_useful_atoms());  // TODO for gnns
                if (child_node.status==SearchNode::Status::NEW) {
                    // Inserted for the first time in the map
                    child_node.open(dist, -1);  // important to set h=-1 to know when to evaluate
                    if (check_goal(task, generator, timer_start, state, node, space))
                        return utils::ExitCode::SUCCESS;

                    open_list.do_insertion(child_node.state_id, make_pair(h, dist));
//                    if (all_operators_preferred or is_preferred) {
//                        preferred_open_list.do_insertion(child_node.state_id, make_pair(h, dist));
//                    } else if (not is_preferred and not prune_relaxed_useless_operators) {
//                        regular_open_list.do_insertion(child_node.state_id, make_pair(h, dist));
//                    }
                }
                // no reopening
//                else {
//                    if (dist < child_node.g) {
//                        child_node.open(dist, h); // Reopening
//                        statistics.inc_reopened();
//                        open_list.do_insertion(child_node.state_id, make_pair(h, dist));
////                        if (all_operators_preferred or is_preferred) {
////                            preferred_open_list.do_insertion(child_node.state_id, make_pair(h, dist));
////                        } else if (not is_preferred and not prune_relaxed_useless_operators) {
////                            regular_open_list.do_insertion(child_node.state_id, make_pair(h, dist));
////                        }
//                    }
//                }
            }
        }
    }

    print_no_solution_found(timer_start);

    return utils::ExitCode::SEARCH_UNSOLVABLE;
}

template <class PackedStateT>
void BatchLazySearch<PackedStateT>::print_statistics() const {
    statistics.print_detailed_statistics();
    space.print_statistics();
}


// explicit template instantiations
template class BatchLazySearch<SparsePackedState>;
template class BatchLazySearch<ExtensionalPackedState>;