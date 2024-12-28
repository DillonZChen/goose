#ifndef PLANNING_NUMERIC_CONDITION_HPP
#define PLANNING_NUMERIC_CONDITION_HPP

#include "numeric_expression.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace planning {
  enum ComparatorType {
    GreaterThan,
    GreaterThanOrEqual,
    Equal,
  };

  class NumericCondition {
   private:
    ComparatorType comparator_type;
    std::shared_ptr<NumericExpression> expression;
    std::function<bool(double)> compare;
    std::function<double(double)> error;

   public:
    NumericCondition(ComparatorType comparator_type, std::shared_ptr<NumericExpression> expression);

    ComparatorType get_comparator_type() const { return comparator_type; }

    std::vector<int> get_fluent_ids() const { return expression->get_fluent_ids(); }

    bool evaluate_formula(const std::vector<double> &values) const;

    double evaluate_error(const std::vector<double> &values) const;

    std::pair<bool, double> evaluate_formula_and_error(const std::vector<double> &values) const;

    std::string to_string() const;
  };
}  // namespace planning

#endif  // PLANNING_NUMERIC_CONDITION_HPP
