#ifndef NUMERIC_CONSTRAINTS_H
#define NUMERIC_CONSTRAINTS_H

#include "../numeric_operator_counting/numeric_helper.h"
#include "ip_constraint_generator.h"

namespace numeric_helper {
class NumericTaskProxy;
}

namespace operator_counting {
class NumericConstraints : public IPConstraintGenerator {
  virtual void initialize_variables(const std::shared_ptr<AbstractTask> task,
                                    std::vector<lp::LPVariable> &variables,
                                    double infinity);
  virtual void initialize_constraints(
      const std::shared_ptr<AbstractTask> task,
      std::vector<lp::LPConstraint> &constraints, double infinity);
  virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
  virtual bool update_constraints(const int horizon, lp::LPSolver &lp_solver,
                                  const std::shared_ptr<AbstractTask> task,
                                  std::vector<lp::LPVariable> &variables,
                                  double infinity,
                                  std::vector<lp::LPConstraint> &constraints);

 private:
  std::vector<std::vector<int>> index_numeric_var;
  std::vector<int> goal_index;
  std::vector<std::vector<double>> large_m;
  std::vector<std::vector<double>> small_m;
  std::vector<std::vector<double>> k_over;
  std::vector<std::vector<double>> k_under;
  std::vector<std::vector<bool>> numeric_mutex;

  numeric_helper::NumericTaskProxy numeric_task;

  void initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                std::vector<lp::LPConstraint> &constraints);
  void goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                             std::vector<lp::LPConstraint> &constraints,
                             double infinity, int t_max);
  void compute_big_m_values(const std::shared_ptr<AbstractTask> task, int t_min,
                            int t_max);
  void action_precondition_constraint(
      const std::shared_ptr<AbstractTask> task,
      std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
      int t_max);
  void action_simple_effect_constraint(
      const std::shared_ptr<AbstractTask> task,
      std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
      int t_max);
  void action_linear_effect_constraint(
      const std::shared_ptr<AbstractTask> task,
      std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
      int t_max);
  void initialize_numeric_mutex();
  void mutex_relaxtion_constraint(const std::shared_ptr<AbstractTask> task,
                                  std::vector<lp::LPConstraint> &constraints,
                                  double infinity, int t_min, int t_max);

  void add_variables(const std::shared_ptr<AbstractTask> task,
                     std::vector<lp::LPVariable> &variables, double infinity,
                     int t_min, int t_max);

 public:
  void print_solution(std::vector<double> &solution,
                      const std::shared_ptr<AbstractTask> task){};
};
}  // namespace operator_counting
#endif
