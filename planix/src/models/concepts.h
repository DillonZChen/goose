#ifndef SRC_MODELS_CONCEPTS_H_
#define SRC_MODELS_CONCEPTS_H_

#include <concepts>
#include <vector>

#include "models/container_templates.h"

/**
 * This file contains the C++20 concepts for the templates parameters regarding planning model
 * requirements.
 */

/*************************************************************************************************
                                Basic components that defines models
 ************************************************************************************************/
template <typename T>
concept SingleSourceMultipleSinks = requires(T x, typename T::State s) {
  { x.initialState() } -> std::convertible_to<typename T::State>;
  { x.isGoal(s) } -> std::convertible_to<bool>;
};

// TODO(fwt): this requirement might change at some point to x.applicableAction(s, a) -> X. I'm not
// sure exactly what to use as X.
template <typename T>
concept DiscreteActions = requires(T x, typename T::State s, typename T::Action a) {
  { x.allActions() } -> std::same_as<std::vector<typename T::Action>>;
  { x.isApplicable(s, a) } -> std::convertible_to<bool>;
};

template <typename T>
concept SingleCost = requires(T x, typename T::State s, typename T::Action a) {
  { x.cost(s, a) } -> std::convertible_to<double>;
};

template <typename T>
concept MultiCost = requires(T x, typename T::State s, typename T::Action a) {
  { x.numCostFunctions() } -> std::same_as<size_t>;
  { x.cost(s, a) } -> std::same_as<std::vector<double>>;
};

template <typename T>
concept DeterministicDynamics = requires(T x, typename T::State s, typename T::Action a) {
  { x.successor(s, a) } -> std::same_as<typename T::State>;
};

template <typename T>
concept ProbabilisticDynamics = requires(T x, typename T::State s, typename T::Action a) {
  { x.successors(s, a) } -> std::same_as<ProbDist<typename T::State>>;
};



/*************************************************************************************************
                                               MODELS
 ************************************************************************************************/
template <typename T>
concept DetPlanProb = SingleSourceMultipleSinks<T>
                        && DiscreteActions<T>
                        && SingleCost<T>
                        && DeterministicDynamics<T>;

template <typename T>
concept MultiObjDetPlanProb = SingleSourceMultipleSinks<T>
                                && DiscreteActions<T>
                                && MultiCost<T>
                                && DeterministicDynamics<T>;

template <typename T>
concept SSP = SingleSourceMultipleSinks<T>
                && DiscreteActions<T>
                && SingleCost<T>
                && ProbabilisticDynamics<T>;

#endif  // SRC_MODELS_CONCEPTS_H_
