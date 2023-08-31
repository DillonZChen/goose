/*
 * Main program.
 *
 * Copyright 2003-2005 Carnegie Mellon University and Rutgers University
 * Copyright 2007 Håkan Younes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <queue>
#include <iostream>
#include <cstdlib>

#include "pybind_goal_count.h"

#include "ext/mdpsim-parser/pddl_to_strips.h"
#include "representations/strips_problems.h"

using STRIPS::StripsState;
using STRIPS::StripsAction;

class Node {
 public:
  Node(StripsState state, double h,
    Node* parent_node = nullptr, int parent_op_idx = -1)
    : state(state), h(h), parent_node(parent_node), parent_op_idx(parent_op_idx)
  { }

  StripsState state;
  double h;
  Node* parent_node;
  int parent_op_idx;
};


/* Greedy best first search without node reopening */
void goal_count_greedy_best_first_search(StripsProblem const& problem)
{
  goal_count_heuristic::PybindGoalCountHeuristic heuristic =
    goal_count_heuristic::PybindGoalCountHeuristic(problem);

  std::cout << "Starting search..." << std::endl;
  std::chrono::time_point<std::chrono::system_clock> start_time =
    std::chrono::system_clock::now();
  std::chrono::time_point<std::chrono::system_clock> t;

  StripsState state = problem.initialState();
  auto const& actions = problem.allActions();
  double h = heuristic.compute_heuristic(state);

  double best_h = h;
  double expansions = 0;
  std::cout << "Initial h computed by goal count is " << h << std::endl;

  Node* node;

  // lower h means higher priority in priority_queue
  auto compare = [](Node* a, Node* b) { return a->h < b->h; };
  std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> q(compare);
  q.push(new Node(state, h));

  std::set<StripsState> seen;

  while (!q.empty()) {
    node = q.top();
    q.pop();
    state = node->state;

    // log search progress
    h = node->h;
    if (h < best_h) {
      t = std::chrono::system_clock::now();
      double time = std::chrono::duration_cast<std::chrono::milliseconds>(t-start_time).count();
      time /= 1000.;

      std::cout << "Next best h = " << h << " found at "
                << expansions << " expansions after "
                << time << "s" << std::endl;
      best_h = h;
    }

    // extract plan once reached the goal state
    if (problem.isGoal(state)) {
      t = std::chrono::system_clock::now();
      double time = std::chrono::duration_cast<std::chrono::milliseconds>(t-start_time).count();
      time /= 1000.;
      std::cout << "Plan found!" << std::endl;

      std::vector<int> plan;
      double cost = 0;
      while (node->parent_op_idx != -1) {
        int op = node->parent_op_idx;
        plan.push_back(op);
        node = node->parent_node;
      }

      reverse(plan.begin(), plan.end());
      for (size_t i = 0; i < plan.size(); i++) {
        StripsAction op = actions[plan[i]];
        cost += op.cost();
        std::cout << op.name() << " (" << op.cost() << ")" << std::endl;
      }

      std::cout << "cost: " << cost << std::endl;
      std::cout << "expansions: " << expansions << std::endl;
      std::cout << "time: " << time << "s" << std::endl;
      return;
    }

    // generate successors for GBFS
    for (size_t i = 0; i < actions.size(); i++) {
      StripsAction op = actions[i];
      if (op.isApplicable(state)) {
        StripsState succ = problem.successor(state, op);
        if (seen.count(succ) == 0) {
          expansions++;
          double h = heuristic.compute_heuristic(succ);
          q.push(new Node(succ, h, node, i));
          seen.insert(succ);
        }
      }
    }
  }

  std::cout << "Search complete. Problem is unsolvable." << std::endl;
  return;
}


/* The main program. */
int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cout
      << "Usage: " << argv[0] << " <single-ppddl-file|domain-file problem-file>\n"
      << "  where single-ppddl-file is a single file containing a PPDDL domain and problem\n"
      << "  alternatively, a pair of domain-file and problem-file can be used.\n\n";
    return 0;
  }

  // Parse in problem with MdpSim first
  PPDDL::Problem const* problem = nullptr;
  if (argc == 2) {
    std::cout << "PDDL file: " << argv[1] << std::endl;
    problem = PPDDL::parsePPDDL(argv[1]);
  } else {
    assert(argc == 3);
    problem = PPDDL::parsePPDDL(argv[1], argv[2]);
  }
  assert(problem);

  // If only the STRIPS problem is needed, then call PPDDL::parseToStrips
  StripsProblem strips_problem = PPDDL::translateToStrips(*problem);

  // Solve
  goal_count_greedy_best_first_search(strips_problem);

  return 0;
}
