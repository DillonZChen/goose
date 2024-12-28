#ifndef PLANNING_NUMERIC_EXPRESSION_HPP
#define PLANNING_NUMERIC_EXPRESSION_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace planning {
  enum OperatorType {
    Plus,
    Minus,
    Multiply,
    Divide,
  };

  class NumericExpression {
   public:
    virtual ~NumericExpression() = default;

    virtual double evaluate(const std::vector<double> &values) const = 0;
    virtual std::vector<int> get_fluent_ids() const = 0;
    virtual std::string to_string() const = 0;
  };

  class FormulaExpression : public NumericExpression {
   private:
    std::function<double(double, double)> op;
    std::string op_symbol;
    const std::shared_ptr<NumericExpression> expr_a;
    const std::shared_ptr<NumericExpression> expr_b;

   public:
    FormulaExpression(OperatorType op_type,
                      std::shared_ptr<NumericExpression> expr_a,
                      std::shared_ptr<NumericExpression> expr_b);
    double evaluate(const std::vector<double> &values) const override;
    std::vector<int> get_fluent_ids() const override;
    std::string to_string() const override;
  };

  class ConstantExpression : public NumericExpression {
   private:
    const double value;

   public:
    ConstantExpression(double value);
    double evaluate(const std::vector<double> &values) const override;
    std::vector<int> get_fluent_ids() const override;
    std::string to_string() const override;
  };

  class FluentExpression : public NumericExpression {
   private:
    int id;
    const std::string fluent_name;

   public:
    FluentExpression(int id, std::string fluent_name);
    double evaluate(const std::vector<double> &values) const override;
    std::vector<int> get_fluent_ids() const override;
    std::string to_string() const override;
  };
}  // namespace planning

#endif  // PLANNING_NUMERIC_EXPRESSION_HPP
