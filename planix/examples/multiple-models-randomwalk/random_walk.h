#ifndef EXAMPLES_RANDOM_WALK_H_
#define EXAMPLES_RANDOM_WALK_H_

#include <cassert>
#include <iostream>
#include <vector>

#include "models/container_templates.h"
#include "models/concepts.h"


/**
 * namespace for the randomwalk template function and its helper functions
 */
namespace RandomWalk {

/**
 * Helper functions for RandomWalk. Not intended for usage outside the RandomWalk namespace. Move to
 * a different namespace if needed.
 */
namespace Helpers {

template<class T>
T const& sampleFromVector(std::vector<T> const& v) {
  size_t n = v.size();
  assert(n > 0);
  size_t idx = lrand48() % n;
  assert(idx < n);
  return v[idx];
}


/**
 * template function to sample the result of an action.
 *
 * This template is just a placeholder and needs to be specialized to different model concepts since
 * sampling the effect of a deterministic model vs probabilistic model is different.
 *
 * Notice that this place holder is not necessary and it is useful for finding missing
 * specializations because it will trigger a linkage error with the exact Problem type missing
 * specialization.
 */
template<class Problem>
Problem::State sampleResult(Problem const&, typename Problem::State const&,
                            typename Problem::Action const&);

/**
 * Specialization of sampleResult for probabilistic problems
 *
 * @sa sampleResult
 */
template<class Problem> requires ProbabilisticDynamics<Problem>
Problem::State sampleResult(Problem const& problem,
                                       typename Problem::State const& s,
                                       typename Problem::Action const& a)
{
  double remaining_r = drand48();
  for (auto const& ip : problem.successors(s, a)) {
    if (remaining_r < ip.second) {
      return ip.first;
    }
    remaining_r -= ip.second;
  }
  assert(false);
  return {};
}


/**
 * Specialization of sampleResult for deterministic problems
 *
 * @sa sampleResult
 */
template<class Problem> requires DeterministicDynamics<Problem>
Problem::State sampleResult(Problem const& problem,
                                       typename Problem::State const& s,
                                       typename Problem::Action const& a)
{
  return problem.successor(s, a);
}


/**
 * template to update the accumulated cost. Using the same template specialization technique as
 * sampleResult.
 *
 * Notice that this template is needed to handle both the single-cost and multiple-cost problems
 *
 * @param acc_cost accumulator variable that will be updated
 * @param cost     cost to added to the accumulator
 *
 * @sa sampleResult
 */
template<class T>
void updateCost(T& acc_cost, T const& cost);


/**
 * Specialization of updateCost for single-cost problems
 */
template<>
void updateCost(double& acc_cost, double const& cost) {
  acc_cost += cost;
}


/**
 * Specialization of updateCost for multiple-cost problems
 */
template<>
void updateCost(std::vector<double>& acc_cost, std::vector<double> const& cost) {
  if (acc_cost.size() == 0) {
    acc_cost = cost;
  } else {
    assert(acc_cost.size() == cost.size());
    for (size_t i = 0; i < cost.size(); ++i) {
      acc_cost[i] += cost[i];
    }
  }
}
}  // namespace Helpers


/**
 * Performs a random walk from a given state to the goal set
 *
 * This method requires only the concepts SingleSourceMultipleSinks (for the existence of goals) and
 * DiscreteActions (for the action interface).
 *
 * @param problem planning problem satisfying SingleSourceMultipleSinks and DiscreteActions
 * @param s       state to start the random walk 
 * @return        accumulated cost of the random walk. Same type as the return type of Problem::cost
 */
template<typename Problem> requires SingleSourceMultipleSinks<Problem> && DiscreteActions<Problem>
auto randomWalkFrom(Problem const& problem, typename Problem::State const& s) {
  using State = typename Problem::State;
  using Action = typename Problem::Action;

  // Telling the compiler that CostType is the same type as the return type of the member function
  // Problem::cost. A function pointer is needed because cost is a member function.
  using CostType = std::result_of<decltype(&Problem::cost)(Problem, State, Action)>::type;

  CostType acc_cost;
  std::cout << "RandomWalkPlanner::solve:\n" << s << std::endl;
  State cur_s(s);
  while (!problem.isGoal(cur_s)) {
    std::cout << cur_s;
    std::vector<Action> applicable;
    for (auto const& a : problem.allActions()) {
      if (problem.isApplicable(cur_s, a)) {
        applicable.push_back(a);
      }
    }
    Action const& rnd_a = Helpers::sampleFromVector(applicable);

    // Using this function call to dispatching to the appropriate method through overloading
    Helpers::updateCost(acc_cost, problem.cost(cur_s, rnd_a));

    cur_s = Helpers::sampleResult(problem, cur_s, rnd_a);
    std::cout << " ---" << rnd_a.name() << "---> " << cur_s << std::endl;
  }
  return acc_cost;
}
}  // namespace RandomWalk

#endif  // EXAMPLES_RANDOM_WALK_H_
