#ifndef PLANNING_NUMERIC_EFFECT_HPP
#define PLANNING_NUMERIC_EFFECT_HPP

#include "../parallel_hashmap/phmap.h"
#include "../utils/hash.h"
#include "lifted_expression.hpp"

#include <memory>
#include <set>
#include <vector>

namespace wlplan {
namespace planning {

  class NumericEffect {
    virtual float apply_impl(float input, float evaluation) const = 0;

   public:
    int table;
    std::vector<int> row;
    std::shared_ptr<LiftedExpression> expression;
    NumericEffect(int table, const std::vector<int> &row, std::shared_ptr<LiftedExpression> expression)
        : table(table), row(row), expression(expression){};

    float apply(float input, float evaluation) const { return apply_impl(input, evaluation); }
  };

  class IncreaseEffect : public NumericEffect {
   protected:
    float apply_impl(float input, float evaluation) const override { return input + evaluation; }

   public:
    IncreaseEffect(int table, const std::vector<int> &row, std::shared_ptr<LiftedExpression> expression)
        : NumericEffect(table, row, expression) {}
  };

  class DecreaseEffect : public NumericEffect {
   protected:
    float apply_impl(float input, float evaluation) const override { return input - evaluation; }

   public:
    DecreaseEffect(int table, const std::vector<int> &row, std::shared_ptr<LiftedExpression> expression)
        : NumericEffect(table, row, expression) {}
  };

  class AssignEffect : public NumericEffect {
   protected:
    float apply_impl(float input, float evaluation) const override { return evaluation; }

   public:
    AssignEffect(int table, const std::vector<int> &row, std::shared_ptr<LiftedExpression> expression)
        : NumericEffect(table, row, expression) {}
  };

}  // namespace planning
} // namespace wlplan

#endif  // PLANNING_NUMERIC_EFFECT_HPP
