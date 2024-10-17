#ifndef GUROBI_NUMERIC_CONSTRAINTS_WITH_CUTS_H
#define GUROBI_NUMERIC_CONSTRAINTS_WITH_CUTS_H

#include <unordered_map>
#include <utility>

#include "../option_parser.h"
#include "numeric_constraints.h"

namespace gurobi_ip_compilation {
class NumericConstraintsWithCuts : public NumericConstraints {
 protected:
  virtual void initialize_numeric_mutex(
      std::vector<std::vector<bool>> &action_mutex) override;
  virtual void compute_big_m_values(const std::shared_ptr<AbstractTask> task,
                                    int t_min, int t_max, bool first) override;
  virtual void precondition_constraint(const std::shared_ptr<AbstractTask> task,
                                       std::shared_ptr<GRBModel> model,
                                       std::vector<std::vector<GRBVar>> &x,
                                       int t_min, int t_max) override;
  virtual void linear_effect_constraint(
      const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
      std::vector<std::vector<GRBVar>> &x, int t_min, int t_max);

  bool precondition_relaxation;
  bool sequence_linear_effects;
  std::vector<std::vector<bool>> action_precedence_inner;
  std::vector<std::vector<bool>> precondition_to_negative;
  std::vector<std::vector<bool>> precondition_to_linear;
  std::vector<std::vector<bool>> simple_to_linear;
  std::unordered_map<int, std::vector<int>> net_effect_actions;
  std::unordered_map<std::pair<int, int>, double> net_values;

 public:
  NumericConstraintsWithCuts(const Options &opts);

  virtual void initialize(const int horizon,
                          const std::shared_ptr<AbstractTask> task,
                          std::vector<std::vector<bool>> &action_mutex,
                          bool use_linear_effects) override;
  virtual void add_action_precedence(
      const std::shared_ptr<AbstractTask> task,
      std::vector<std::vector<bool>> &action_precedence,
      std::vector<std::vector<bool>> &action_mutex) override;
};
}  // namespace gurobi_ip_compilation
#endif
