#ifndef SRC_MODELS_CONTAINER_TEMPLATES_H_
#define SRC_MODELS_CONTAINER_TEMPLATES_H_

#include <unordered_map>
#include <unordered_set>

/**
 * Common templates used on planning algorithms.
 */

template<class State>
using HashState = std::unordered_set<State>;

template<class State>
using ProbDist = std::unordered_map<State, double>;

#endif  // SRC_MODELS_CONTAINER_TEMPLATES_H_
