#include "dqqbs.h"
#include "../search_engines/search.h"
#include "../search_engines/utils.h"

#include "../action.h"

#include "../heuristics/heuristic.h"
#include "../open_lists/greedy_open_list.h"
#include "../states/extensional_states.h"
#include "../states/sparse_states.h"
#include "../successor_generators/successor_generator.h"
#include "../utils/timer.h"

#include "qb_heuristic.h"
#include "wl_utils.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_set>
#include <vector>

using namespace std;

template <class PackedStateT>
utils::ExitCode DualQueueQBSearch<PackedStateT>::search(const Task &task,
                                                        SuccessorGenerator &generator,
                                                        Heuristic &heuristic)
{
    // Cast the heuristic to QbHeuristic
    QbHeuristic* qbh_raw = dynamic_cast<QbHeuristic*>(&heuristic);
    if (!qbh_raw) {
        cerr << "DualQueueQBSearch requires a QbHeuristic!" << endl;
        exit(1);
    }

    // Create shared_ptr from the cast pointer
    std::shared_ptr<QbHeuristic> qbh_ptr(qbh_raw, [](QbHeuristic*){});

    cout << "Starting dual queue WL-novelty search (no re-openings)" << endl;
    clock_t timer_start = clock();
    const auto action_schemas = task.get_action_schemas();
    StatePackerT packer(task);

    GreedyOpenList q1;
    GreedyOpenList q2;
    std::unordered_set<int> popped_ids;
    bool pop_from_1 = true;

    SearchNode &root_node = space.insert_or_get_previous_node(
        packer.pack(task.initial_state), LiftedOperatorId::no_operator, StateID::no_state);
    utils::Timer t;
    qbh_raw->compute_heuristic(task.initial_state, task);
    heuristic_layer = qbh_raw->return_cached_original_heuristic();
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
    q1.do_insertion(root_node.state_id, make_pair(heuristic_layer, 0));

    if (check_goal(task, generator, timer_start, task.initial_state, root_node, space))
        return utils::ExitCode::SUCCESS;

    while (true) {
        // Pop from one of two queues
        bool good = false;  // determined if we successfully popped from a queue
        StateID sid = StateID::no_state;
        if (pop_from_1) {
            while (!q1.empty()) {
                sid = q1.remove_min();
                if (popped_ids.count(sid.id()) == 0) {
                    good = true;
                    pop_from_1 = false;
                    break;
                }
            }
            while (!good && !q2.empty()) {
                sid = q2.remove_min();
                if (popped_ids.count(sid.id()) == 0) {
                    good = true;
                    pop_from_1 = true;
                    break;
                }
            }
        }
        else {
            while (!q2.empty()) {
                sid = q2.remove_min();
                if (popped_ids.count(sid.id()) == 0) {
                    good = true;
                    pop_from_1 = true;
                    break;
                }
            }
            while (!good && !q1.empty()) {
                sid = q1.remove_min();
                if (popped_ids.count(sid.id()) == 0) {
                    good = true;
                    pop_from_1 = false;
                    break;
                }
            }
        }
        if (!good) {
            break;
        }
        popped_ids.insert(sid.id());

        SearchNode &node = space.get_node(sid);
        if (node.status == SearchNode::Status::CLOSED) {
            continue;
        }
        node.close();
        int h = node.h;
        int g = node.g;
        statistics.report_f_value_progress(h);  // In GBFS f = h.
        statistics.inc_expanded();

        if (h < heuristic_layer) {
            heuristic_layer = h;
            cout << "New heuristic value expanded: h=" << h
                 << " [expansions: " << statistics.get_expanded()
                 << ", evaluations: " << statistics.get_evaluations()
                 << ", generations: " << statistics.get_generated()
                 << ", time: " << double(clock() - timer_start) / CLOCKS_PER_SEC << "]" << '\n';
        }
        assert(sid.id() >= 0 && (unsigned)sid.id() < space.size());

        DBState state = packer.unpack(space.get_state(sid));
        if (check_goal(task, generator, timer_start, state, node, space)) {
            heuristic.print_statistics();
            return utils::ExitCode::SUCCESS;
        }

        const auto applicable = generator.get_applicable_actions(action_schemas, state);
        statistics.inc_generated(applicable.size());

        for (const LiftedOperatorId &op_id : applicable) {
            const auto &action = action_schemas[op_id.get_index()];
            DBState s = generator.generate_successor(op_id, action, state);
            auto &child_node =
                space.insert_or_get_previous_node(packer.pack(s), op_id, node.state_id);
            int dist = g + action.get_cost();

            // begin GOOSE time
            int nov_h = qbh_ptr->compute_heuristic(s, task);
            int new_h = qbh_ptr->return_cached_original_heuristic();
            // end GOOSE time

            statistics.inc_evaluations();
            if (new_h == UNSOLVABLE_STATE) {
                if (child_node.status == SearchNode::Status::NEW) {
                    // Only increase statistics for new dead-ends
                    child_node.open(dist, new_h);
                    statistics.inc_dead_ends();
                    statistics.inc_pruned_states();
                }
                continue;
            }

            if (child_node.status == SearchNode::Status::NEW) {
                // Inserted for the first time in the map
                child_node.open(dist, new_h);
                statistics.inc_evaluated_states();
                q1.do_insertion(child_node.state_id, make_pair(new_h, dist));
                q2.do_insertion(child_node.state_id, make_pair(nov_h, dist));
            }
            // else {
            //     if (dist < child_node.g) {
            //         child_node.open(dist, new_h);  // Reopening
            //         statistics.inc_reopened();
            //         q1.do_insertion(child_node.state_id, make_pair(new_h, dist));
            //     }
            // }
        }
    }

    print_no_solution_found(timer_start);

    return utils::ExitCode::SEARCH_UNSOLVABLE;
}

template <class PackedStateT>
void DualQueueQBSearch<PackedStateT>::print_statistics() const
{
    statistics.print_detailed_statistics();
    space.print_statistics();
}

// explicit template instantiations
template class DualQueueQBSearch<SparsePackedState>;
template class DualQueueQBSearch<ExtensionalPackedState>;  //
