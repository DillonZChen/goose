#ifndef SRC_ALGS_REACHABLE_STATES_H_
#define SRC_ALGS_REACHABLE_STATES_H_

#include <iostream>
#include <queue>

#include "models/container_templates.h"
#include "models/concepts.h"

/**
 * Template to compute the reachable states from a given state to the goal set in an SSP
 *
 * @param ssp a problem satisfying the SSP concept
 * @param root the state serving as the root of the search
 *
 * This template requires an SSP although it could be generalize to other models.
 ************************************************************************************************/
template<class Problem> requires SSP<Problem>
HashState<typename Problem::State> reachableStatesFrom(Problem const& ssp,
                                                       typename Problem::State const& root)
{
  using State = Problem::State;
  using QueueState = std::queue<State>;
  QueueState queue;
  HashState<State> reachable;

  /// Invariant: every state in queue is also in reachable
  queue.push(root);
  reachable.insert(root);
  bool goal_reached = false;

  std::cout << "[reachableStatesFrom] Starting from s = " << root << std::endl;

  while (!queue.empty()) {
    State const& s = queue.front();
    std::cout << "  s = " << s << std::endl;

    if (ssp.isGoal(s)) {
      goal_reached = true;
      std::cout << "    is goal\n";
      queue.pop();
      continue;
    }

    for (auto const& a : ssp.allActions()) {
      if (!a.isApplicable(s)) continue;
      std::cout << "    a = " << a.name() << std::endl;
      for (auto const& ip : ssp.successors(s, a)) {
        State const& s_prime = ip.first;
        std::cout << "      " << ip.second << ": " << s_prime << std::endl;
        if (reachable.find(s_prime) == reachable.end()) {
          reachable.insert(s_prime);
          queue.push(s_prime);
        }
      }
    }
    queue.pop();
  }
  std::cout << "[reachableStatesFrom] Total states = " << reachable.size() << " -- "
            << (goal_reached ? "Goal reached" : "NO GOAL reached") << std::endl;

  return reachable;
}

#endif  // SRC_ALGS_REACHABLE_STATES_H_
