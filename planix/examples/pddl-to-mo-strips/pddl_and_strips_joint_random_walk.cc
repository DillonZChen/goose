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

#include <iostream>
#include <cstdlib>

#include "ext/mdpsim-parser/pddl_to_strips.h"

// Needed for the random walk
#include "ext/mdpsim-parser/problems.h"
#include "ext/mdpsim-parser/domains.h"
#include "ext/mdpsim-parser/rational.h"
#include "representations/strips_problems.h"

#define MAX_TURNS 1000

using PPDDL::Action;
using PPDDL::ActionList;
using PPDDL::AtomSet;
using PPDDL::Problem;
using PPDDL::ValueMap;
using PPDDL::State;
using PPDDL::Rational;
using PPDDL::PrState;
using PPDDL::Domain;


/* Selects an action for the given state. */
static const Action* randomAction(State const& s, const Problem& problem) {
  ActionList actions;
  problem.enabledActions(actions, s);
  if (actions.empty()) {
    return nullptr;
  }

  size_t i = size_t(rand()/(RAND_MAX + 1.0) * actions.size());
  return actions[i];
}


void randomWalk(Problem const& problem, StripsMultiObjectiveProblem const& strips,
                size_t turn_limit)
{
  using STRIPS::StripsState;
  using STRIPS::StripsMultiCostAction;

  std::cout << "simulating problem `" << problem.name() << "'"
            << " until a goal or dead-end state is found OR "
            << turn_limit << " actions are applied"
            << std::endl;

  State s = problem.initialState();
  StripsState strips_state = strips.initialState();
  std::vector<double> strips_costs(strips.numCostFunctions(), 0.0);
  auto const& strips_all_actions = strips.allActions();

  size_t time = 0;
  while (time < turn_limit && !problem.isGoal(s)) {
    std::cout << std::endl << time << ": " << s << std::endl;
    Action const* action = randomAction(s, problem);
    if (action == nullptr) {
      // Double checking that the state is a dead end in the STRIPS translation
      for (auto const& a : strips_all_actions) {
        assert(!a.isApplicable(strips_state));
      }
      std::cout << "  State is a dead end (A(s) = {})" << std::endl;
      break;
    }
    assert(action != nullptr);
    std::cout << "  a = " << *action << std::endl;
    std::cout << "  cost = {";
    for (auto const& fname_val : action->cost()) {
      std::cout << fname_val.first << ": " << fname_val.second << ", ";
    }
    std::cout << "}" << std::endl;

    // Finding the equivalent STRIPS action
    auto same_name = [action](StripsMultiCostAction const& a){ return a.name() == action->name(); };

    auto const strips_action_it = std::find_if(strips_all_actions.begin(), strips_all_actions.end(),
                                               same_name);
    assert(strips_action_it != strips_all_actions.end());
    StripsMultiCostAction const& strips_action = *strips_action_it;
    assert(strips_action.isApplicable(strips_state));
    assert(strips_action.numCostFunctions() == strips_costs.size());
    auto const& strips_cost_vec = strips_action.cost();
    for (size_t i = 0; i < strips_costs.size(); ++i) {
      strips_costs[i] += strips_cost_vec[i];
    }


    PrState<Rational> out = action->probTransitionTable<Rational>(s);
    assert(out.size() == 1);

    s = action->apply(s);
    strips_state = strips_action.successor(strips_state);
    time++;
  }
  std::cout << std::endl << time << ": " << s << std::endl;
  if (problem.isGoal(s)) {
    assert(strips.isGoal(strips_state));
    std::cout << "  goal achieved" << std::endl;
  }
  else if (time >= turn_limit) {
    std::cout << "  turn limit reached" << std::endl;
  }
  else {
    std::cout << "  dead end found" << std::endl;
  }

  std::cout << "STRIPS multi-objective cost: {";
  for (double const& c : strips_costs) {
    std::cout << c << ", ";
  }
  std::cout << "}" << std::endl;
}


/* The main program. */
int main(int argc, char* argv[]) {
  size_t seed = time(NULL);
  if (argc == 1) {
    std::cout
      << "Usage: " << argv[0] << " [-s SEED] <single-ppddl-file|domain-file problem-file>\n"
      << "  where single-ppddl-file is a single file containing a PPDDL domain and problem\n"
      << "  alternatively, a pair of domain-file and problem-file can be used.\n\n"
      << argv[0]
      << " will perform a random walk until a goal or dead-end state is reached or "
      << MAX_TURNS << " actions has been applied\n\n";
    return 0;
  }
  else if (argv[1][0] == '-' && argv[1][1] == 's') {
    seed = atoi(argv[2]);
    argv += 2;
    argc -= 2;
  }
  std::cout << "seed = " << seed << std::endl;
  srand(seed);

  Problem const* problem = nullptr;
  if (argc == 2) {
    std::cout << "PDDL file: " << argv[1] << std::endl;
    problem = PPDDL::parsePPDDL(argv[1]);
  }
  else {
    assert(argc == 3);
    problem = PPDDL::parsePPDDL(argv[1], argv[2]);
  }
  assert(problem);

  auto const& metrics = problem->metrics();
  std::cout << "This problem has " << metrics.size() << " metrics to be minimized\n";
  for (auto const& expr_ptr : metrics) {
    std::cout << "  " << *expr_ptr << "\n";
  }

  // If only the STRIPS problem is needed, then call PPDDL::parseToStrips
  StripsMultiObjectiveProblem strips_problem = PPDDL::translateToMoStrips(*problem);

  randomWalk(*problem, strips_problem, MAX_TURNS);

  return 0;
}
