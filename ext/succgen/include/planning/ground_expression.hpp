#ifndef PLANNING_GROUND_EXPRESSION_HPP
#define PLANNING_GROUND_EXPRESSION_HPP

#include "../parallel_hashmap/phmap.h"
#include "../utils/hash.h"
#include "utils.hpp"

#include <memory>
#include <set>
#include <vector>

namespace planning {

  class GroundBooleanExpression {
   protected:
    virtual bool eval_bool_impl(const Values &values) const = 0;
    virtual double eval_error_impl(const Values &values) const = 0;

   public:
    bool eval_bool(const Values &values) const { return eval_bool_impl(values); }
    double eval_error(const Values &values) const { return eval_error_impl(values); }
  };

  class GroundNumericExpression {
   protected:
    virtual double eval_real_impl(const Values &values) const = 0;

   public:
    double eval_real(const Values &values) const { return eval_real_impl(values); }
  };

  /* Ground Boolean Conditions */
  class GroundNot : public GroundBooleanExpression {
   protected:
    const std::shared_ptr<GroundBooleanExpression> expr;

    bool eval_bool_impl(const Values &values) const override { return !expr->eval_bool(values); }

    // TODO not sure what would make sense here
    double eval_error_impl(const Values &values) const override { return 0; }

   public:
    GroundNot(std::shared_ptr<GroundBooleanExpression> expr) : expr(expr) {}
  };

  class GroundOr : public GroundBooleanExpression {
   protected:
    const std::vector<std::shared_ptr<GroundBooleanExpression>> exprs;
    bool eval_bool_impl(const Values &values) const override {
      for (const auto &expr : exprs) {
        if (expr->eval_bool(values)) {
          return true;
        }
      }
      return false;
    }

    double eval_error_impl(const Values &values) const override {
      double error = std::numeric_limits<double>::max();
      for (const auto &expr : exprs) {
        error = std::min(error, expr->eval_error(values));
      }
      return error;
    }

   public:
    GroundOr(const std::vector<std::shared_ptr<GroundBooleanExpression>> &exprs) : exprs(exprs) {}
  };

  class GroundBinaryBooleanCondition : public GroundBooleanExpression {
   protected:
    const std::shared_ptr<GroundNumericExpression> lhs;
    const std::shared_ptr<GroundNumericExpression> rhs;

    virtual bool eval_bool_impl(const Values &values) const = 0;
    virtual double eval_error_impl(const Values &values) const = 0;

   public:
    GroundBinaryBooleanCondition(std::shared_ptr<GroundNumericExpression> lhs,
                                 std::shared_ptr<GroundNumericExpression> rhs)
        : lhs(lhs), rhs(rhs) {}
  };
  class GroundLessThanEqualTo : public GroundBinaryBooleanCondition {
   protected:
    bool eval_bool_impl(const Values &values) const override {
      return lhs->eval_real(values) <= rhs->eval_real(values);
    }

    double eval_error_impl(const Values &values) const override {
      double lhs_value = lhs->eval_real(values);
      double rhs_value = rhs->eval_real(values);
      if (lhs_value <= rhs_value) {
        return 0;
      } else {
        return std::abs(lhs_value - rhs_value);
      }
    }

   public:
    GroundLessThanEqualTo(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryBooleanCondition(lhs, rhs) {}
  };
  class GroundLessThan : public GroundBinaryBooleanCondition {
   protected:
    bool eval_bool_impl(const Values &values) const override { return lhs->eval_real(values) < rhs->eval_real(values); }

    double eval_error_impl(const Values &values) const override {
      double lhs_value = lhs->eval_real(values);
      double rhs_value = rhs->eval_real(values);
      if (lhs_value < rhs_value) {
        return 0;
      } else {
        return std::abs(lhs_value - rhs_value);
      }
    }

   public:
    GroundLessThan(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryBooleanCondition(lhs, rhs) {}
  };
  class GroundGreaterThanEqualTo : public GroundBinaryBooleanCondition {
   protected:
    bool eval_bool_impl(const Values &values) const override {
      return lhs->eval_real(values) >= rhs->eval_real(values);
    }

    double eval_error_impl(const Values &values) const override {
      double lhs_value = lhs->eval_real(values);
      double rhs_value = rhs->eval_real(values);
      if (lhs_value >= rhs_value) {
        return 0;
      } else {
        return std::abs(lhs_value - rhs_value);
      }
    }

   public:
    GroundGreaterThanEqualTo(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryBooleanCondition(lhs, rhs) {}
  };
  class GroundGreaterThan : public GroundBinaryBooleanCondition {
   protected:
    bool eval_bool_impl(const Values &values) const override { return lhs->eval_real(values) > rhs->eval_real(values); }

    double eval_error_impl(const Values &values) const override {
      double lhs_value = lhs->eval_real(values);
      double rhs_value = rhs->eval_real(values);
      if (lhs_value > rhs_value) {
        return 0;
      } else {
        return std::abs(lhs_value - rhs_value);
      }
    }

   public:
    GroundGreaterThan(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryBooleanCondition(lhs, rhs) {}
  };
  class GroundEqualTo : public GroundBinaryBooleanCondition {
   protected:
    bool eval_bool_impl(const Values &values) const override {
      return lhs->eval_real(values) == rhs->eval_real(values);
    }

    double eval_error_impl(const Values &values) const override {
      double lhs_value = lhs->eval_real(values);
      double rhs_value = rhs->eval_real(values);
      if (lhs_value == rhs_value) {
        return 0;
      } else {
        return std::abs(lhs_value - rhs_value);
      }
    }

   public:
    GroundEqualTo(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryBooleanCondition(lhs, rhs) {}
  };

  /* Ground Numeric Expressions */
  class GroundValue : public GroundNumericExpression {
   protected:
    const double value;

    double eval_real_impl(const Values &values) const override { return value; }

   public:
    GroundValue(double value) : value(value) {}
  };

  class GroundFunction : public GroundNumericExpression {
   protected:
    const int index;

    double eval_real_impl(const Values &values) const override { return values[index]; }

   public:
    GroundFunction(int index) : index(index) {}
  };

  class GroundBinaryNumericExpression : public GroundNumericExpression {
   protected:
    const std::shared_ptr<GroundNumericExpression> lhs;
    const std::shared_ptr<GroundNumericExpression> rhs;

   public:
    GroundBinaryNumericExpression(std::shared_ptr<GroundNumericExpression> lhs,
                                  std::shared_ptr<GroundNumericExpression> rhs)
        : lhs(lhs), rhs(rhs) {}
  };
  class GroundPlus : public GroundBinaryNumericExpression {
   protected:
    double eval_real_impl(const Values &values) const override {
      return lhs->eval_real(values) + rhs->eval_real(values);
    }

   public:
    GroundPlus(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryNumericExpression(lhs, rhs) {}
  };
  class GroundMinus : public GroundBinaryNumericExpression {
   protected:
    double eval_real_impl(const Values &values) const override {
      return lhs->eval_real(values) - rhs->eval_real(values);
    }

   public:
    GroundMinus(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryNumericExpression(lhs, rhs) {}
  };
  class GroundTimes : public GroundBinaryNumericExpression {
   protected:
    double eval_real_impl(const Values &values) const override {
      return lhs->eval_real(values) * rhs->eval_real(values);
    }

   public:
    GroundTimes(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryNumericExpression(lhs, rhs) {}
  };
  class GroundDivide : public GroundBinaryNumericExpression {
   protected:
    double eval_real_impl(const Values &values) const override {
      return lhs->eval_real(values) / rhs->eval_real(values);
    }

   public:
    GroundDivide(std::shared_ptr<GroundNumericExpression> lhs, std::shared_ptr<GroundNumericExpression> rhs)
        : GroundBinaryNumericExpression(lhs, rhs) {}
  };

}  // namespace planning

#endif  // PLANNING_GROUND_EXPRESSION_HPP
