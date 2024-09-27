#ifndef SAS_STATE_CHANGE_MODEL_H
#define SAS_STATE_CHANGE_MODEL_H

#include "../numeric_operator_counting/numeric_helper.h"
#include "../operator_counting/constraint_generator.h"
#include "ip_constraint_generator.h"

namespace numeric_helper {
class NumericTaskProxy;
}

namespace operator_counting {
class SASStateChangeModel : public IPConstraintGenerator {
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
  // std::vector<vector<int>> index_opt;
  std::vector<vector<vector<vector<int>>>>
      index_var;  // first index var, second val in, third val out, fourth time
  std::vector<vector<vector<set<int>>>> sc_actions;
  std::vector<vector<set<int>>> add_actions;
  std::vector<vector<pair<int, set<int>>>> effs_actions;
  std::vector<vector<set<int>>> pre_actions;
  std::vector<int> goal_index;
  std::vector<vector<bool>> sc_facts;

  void create_sets(const std::shared_ptr<AbstractTask> task);

  numeric_helper::NumericTaskProxy numeric_task;
  void add_variables(const std::shared_ptr<AbstractTask> task,
                     std::vector<lp::LPVariable> &variables, double infinity,
                     int t_min, int t_max);

  // constraints
  void initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                std::vector<lp::LPConstraint> &constraints);
  void goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                             std::vector<lp::LPConstraint> &constraints,
                             int t_max);
  void update_state_change_constraint(
      const std::shared_ptr<AbstractTask> task,
      std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
      int t_max);
  void effect_constraint(const std::shared_ptr<AbstractTask> task,
                         std::vector<lp::LPConstraint> &constraints,
                         double infinity, int t_min, int t_max);
  void mutex_relaxtion_constraint(const std::shared_ptr<AbstractTask> task,
                                  std::vector<lp::LPConstraint> &constraints,
                                  double infinity, int t_min, int t_max);
  void precondition_constraint(const std::shared_ptr<AbstractTask> task,
                               std::vector<lp::LPConstraint> &constraints,
                               double infinity, int t_min, int t_max);
  void mutex_proposition_constraint(const std::shared_ptr<AbstractTask> task,
                                    std::vector<lp::LPConstraint> &constraints,
                                    double infinity, int t_min, int t_max);

 public:
  // void print_solution(std::vector<double> &solution, const
  // std::shared_ptr<AbstractTask> task);
};
}  // namespace operator_counting
#endif
