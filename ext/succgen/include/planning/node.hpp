#ifndef PLANNING_NODE_HPP
#define PLANNING_NODE_HPP

#include "state.hpp"

#include <set>
#include <vector>

namespace wlplan {
namespace planning {
  struct Node {
    const SGState state;
    const std::pair<int, std::vector<int>> achieving_action;
    const int s_id;
    const int parent_s_id;
  };
}  // namespace planning
} // namespace wlplan

#endif  // PLANNING_NODE_HPP
