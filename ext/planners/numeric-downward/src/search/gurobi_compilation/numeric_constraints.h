#ifndef GUROBI_NUMERIC_CONSTRAINTS_H
#define GUROBI_NUMERIC_CONSTRAINTS_H

#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "ip_constraint_generator.h"

namespace numeric_helper {
class NumericTaskProxy;
}

namespace gurobi_ip_compilation {
class NumericConstraints : public GurobiIPConstraintGenerator {
 protected:
  int current_horizon;
  int num_repetition;
  bool restrict_mutex;
  bool has_linear_effects;
  std::vector<std::vector<GRBVar>> y;
  std::vector<std::vector<double>> large_m;
  std::vector<std::vector<double>> small_m;
  std::vector<std::vector<bool>> numeric_mutex;
  std::vector<bool> repetable;

  numeric_helper::NumericTaskProxy numeric_task;

  void initialize_repetable_actions();
  void add_variables(const std::shared_ptr<AbstractTask> task,
                     std::shared_ptr<GRBModel> model, int t_min, int t_max,
                     bool first);
  void modify_x(const std::shared_ptr<AbstractTask> task,
                std::shared_ptr<GRBModel> model,
                std::vector<std::vector<GRBVar>> &x, int t_min, int t_max);
  void initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                std::shared_ptr<GRBModel> model);
  void goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                             std::shared_ptr<GRBModel> model, int t_max,
                             bool first);
  void simple_effect_constraint(const std::shared_ptr<AbstractTask> task,
                                std::shared_ptr<GRBModel> model,
                                std::vector<std::vector<GRBVar>> &x, int t_min,
                                int t_max);
  virtual void initialize_numeric_mutex(
      std::vector<std::vector<bool>> &action_mutex);
  virtual void compute_big_m_values(const std::shared_ptr<AbstractTask> task,
                                    int t_min, int t_max, bool first);
  virtual void precondition_constraint(const std::shared_ptr<AbstractTask> task,
                                       std::shared_ptr<GRBModel> model,
                                       std::vector<std::vector<GRBVar>> &x,
                                       int t_min, int t_max);
  virtual void linear_effect_constraint(
      const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
      std::vector<std::vector<GRBVar>> &x, int t_min, int t_max);

 public:
  NumericConstraints(const Options &opts);

  virtual void initialize(const int horizon,
                          const std::shared_ptr<AbstractTask> task,
                          std::vector<std::vector<bool>> &action_mutex,
                          bool use_linear_effects) override;
  virtual void update(const int horizon,
                      const std::shared_ptr<AbstractTask> task,
                      std::shared_ptr<GRBModel> model,
                      std::vector<std::vector<GRBVar>> &x) override;
  virtual void dump() override;
};
}  // namespace gurobi_ip_compilation
#endif
