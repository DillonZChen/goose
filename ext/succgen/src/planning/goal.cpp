#include "../../include/planning/goal.hpp"

#include <algorithm>

namespace wlplan {
namespace planning {
  SGGoal::SGGoal(const Atoms &pos_goals, const Atoms &neg_goals, const std::vector<std::shared_ptr<GroundBooleanExpression>> &numeric_goals)
      : pos_goals(pos_goals), neg_goals(neg_goals), numeric_goals(numeric_goals) {}

  bool SGGoal::satisfied_by(const SGState &state) {
    for (const auto &pos_goal : pos_goals) {
      if (!state.atoms.count(pos_goal)) {
        return false;
      }
    }
    for (const auto &neg_goal : neg_goals) {
      if (state.atoms.count(neg_goal)) {
        return false;
      }
    }
    for (const auto &numeric_goal : numeric_goals) {
      if (!numeric_goal->eval_bool(state.values)) {
        return false;
      }
    }
    return true;
  }
}  // namespace planning
} // namespace wlplan
