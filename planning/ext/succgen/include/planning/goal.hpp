#ifndef PLANNING_GOAL_HPP
#define PLANNING_GOAL_HPP

#include "ground_expression.hpp"
#include "state.hpp"

#include <set>
#include <vector>
#include <memory>

namespace planning {

  class Goal {
   public:
    const Atoms pos_goals;
    const Atoms neg_goals;
    const std::vector<std::shared_ptr<GroundBooleanExpression>> numeric_goals;

    Goal(const Atoms &pos_goals, const Atoms &neg_goals, const std::vector<std::shared_ptr<GroundBooleanExpression>> &numeric_goals);

    bool satisfied_by(const State &state);
  };
}  // namespace planning

#endif  // PLANNING_Goal_HPP
