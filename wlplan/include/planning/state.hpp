#ifndef PLANNING_STATE_HPP
#define PLANNING_STATE_HPP

#include "atom.hpp"

#include <vector>

namespace planning {
  using State = std::vector<planning::Atom>;
}  // namespace planning

#endif  // PLANNING_STATE_HPP
