#ifndef PLANNING_STATE_STORER_HPP
#define PLANNING_STATE_STORER_HPP

#include "state.hpp"

#include <set>
#include <unordered_set>
#include <vector>

namespace planning {
  class StateStorer {
    std::unordered_set<SGState, StateHash> states;

   public:
    StateStorer() = default;

    void add(const SGState &state) { states.insert(state); }
    bool contains(const SGState &state) { return states.find(state) != states.end(); }
  };

}  // namespace planning

#endif  // PLANNING_STATE_STORER_HPP
