#ifndef PLANNING_LIFTED_EXPRESSION_HPP
#define PLANNING_LIFTED_EXPRESSION_HPP

#include "utils.hpp"

#include <memory>
#include <set>
#include <vector>

namespace planning {

  class LiftedExpression {
   protected:
    virtual float evaluate_impl(const std::vector<int> &instantiation,
                                const FluentIndexMap &nvars_map,
                                const std::vector<double> &nvals) const = 0;

   public:
    float evaluate(const std::vector<int> &instantiation,
                   const FluentIndexMap &nvars_map,
                   const std::vector<double> &nvals) const {
      return evaluate_impl(instantiation, nvars_map, nvals);
    }
  };

  class LiftedValue : public LiftedExpression {
   protected:
    const float value;
    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      return value;
    }

   public:
    LiftedValue(float value) : value(value) {}
  };

  class LiftedFunction : public LiftedExpression {
   protected:
    const Fluent fluent;
    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      // Create a key from the fluent and the instantiation
      std::vector<int> key_row;
      for (int i : fluent.second) {
        key_row.push_back(instantiation[i]);
      }
      Fluent key = {fluent.first, key_row};
      // Check if the key exists in the nvars_map
      auto it = nvars_map.map.find(key);
      if (it == nvars_map.map.end()) {
        return 0;  // Key not found, return 0
      }
      return nvals[it->second];  // Return the value from nvals
    }

   public:
    LiftedFunction(const Fluent &fluent) : fluent(fluent) {}
  };

  class StaticLiftedFunction : public LiftedExpression {
   protected:
    const Fluent fluent;
    const FluentValueMap values;
    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      // Create a key from the fluent and the instantiation
      std::vector<int> key_row;
      for (int i : fluent.second) {
        key_row.push_back(instantiation[i]);
      }
      Fluent key = {fluent.first, key_row};
      // Check if the key exists in the values
      auto it = values.map.find(key);
      if (it == values.map.end()) {
        return 0;  // Key not found, return 0
      }
      return it->second;  // Return the value from values
    }

   public:
    StaticLiftedFunction(const Fluent &fluent, const FluentValueMap &values) : fluent(fluent), values(values) {}
  };

  class LiftedPlus : public LiftedExpression {
   protected:
    const std::shared_ptr<LiftedExpression> lhs;
    const std::shared_ptr<LiftedExpression> rhs;

    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      float lhs_value = lhs->evaluate(instantiation, nvars_map, nvals);
      float rhs_value = rhs->evaluate(instantiation, nvars_map, nvals);
      return lhs_value + rhs_value;
    }

   public:
    LiftedPlus(std::shared_ptr<LiftedExpression> lhs, std::shared_ptr<LiftedExpression> rhs) : lhs(lhs), rhs(rhs) {}
  };

  class LiftedMinus : public LiftedExpression {
   protected:
    const std::shared_ptr<LiftedExpression> lhs;
    const std::shared_ptr<LiftedExpression> rhs;

    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      float lhs_value = lhs->evaluate(instantiation, nvars_map, nvals);
      float rhs_value = rhs->evaluate(instantiation, nvars_map, nvals);
      return lhs_value - rhs_value;
    }

   public:
    LiftedMinus(std::shared_ptr<LiftedExpression> lhs, std::shared_ptr<LiftedExpression> rhs) : lhs(lhs), rhs(rhs) {}
  };

  class LiftedTimes : public LiftedExpression {
   protected:
    const std::shared_ptr<LiftedExpression> lhs;
    const std::shared_ptr<LiftedExpression> rhs;

    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      float lhs_value = lhs->evaluate(instantiation, nvars_map, nvals);
      float rhs_value = rhs->evaluate(instantiation, nvars_map, nvals);
      return lhs_value * rhs_value;
    }

   public:
    LiftedTimes(std::shared_ptr<LiftedExpression> lhs, std::shared_ptr<LiftedExpression> rhs) : lhs(lhs), rhs(rhs) {}
  };

  class LiftedDivide : public LiftedExpression {
   protected:
    const std::shared_ptr<LiftedExpression> lhs;
    const std::shared_ptr<LiftedExpression> rhs;

    float evaluate_impl(const std::vector<int> &instantiation,
                        const FluentIndexMap &nvars_map,
                        const std::vector<double> &nvals) const override {
      float lhs_value = lhs->evaluate(instantiation, nvars_map, nvals);
      float rhs_value = rhs->evaluate(instantiation, nvars_map, nvals);
      return lhs_value / rhs_value;
    }

   public:
    LiftedDivide(std::shared_ptr<LiftedExpression> lhs, std::shared_ptr<LiftedExpression> rhs) : lhs(lhs), rhs(rhs) {}
  };

}  // namespace planning

#endif  // PLANNING_LIFTED_EXPRESSION_HPP
