#ifndef OPERATOR_COUNTING_DELETE_RELAXATION_CONSTRAINTS_H
#define OPERATOR_COUNTING_DELETE_RELAXATION_CONSTRAINTS_H

#include <iostream>
#include <list>
#include <set>

#include "../numeric_landmarks/landmark_factory_scala.h"
#include "../operator_counting/constraint_generator.h"
#include "../operator_counting/state_equation_constraints.h"
#include "numeric_helper.h"

class TaskProxy;

namespace lp {
class LPConstraint;
}

namespace numeric_helper {
class NumericTaskProxy;
}
namespace operator_counting {

enum class LPConstraintType { LP = 0, IP = 1 };

class DeleteRelaxationConstraints : public ConstraintGenerator {
  std::vector<std::vector<int>> index_constraints;
  std::vector<int> index_constraints_numeric;

  double bigM;

  numeric_helper::NumericTaskProxy numeric_task;

  void add_actions_constraints(std::vector<lp::LPConstraint> &constraints,
                               double infinity);
  void add_initial_state_constraints(
      std::vector<lp::LPConstraint> &constraints);
  void add_goal_state_constraints(std::vector<lp::LPConstraint> &constraints,
                                  TaskProxy &task_proxy);
  void add_preconditions_constraints(std::vector<lp::LPConstraint> &constraints,
                                     double infinity);
  void add_preconditions_constraints_inv(
      std::vector<lp::LPConstraint> &constraints, double infinity);

  void add_effects_constraints(std::vector<lp::LPConstraint> &constraints,
                               double infinity);
  void add_sequencing_constraints(std::vector<lp::LPConstraint> &constraints,
                                  double infinity);
  void add_numeric_conditions_constraints(
      std::vector<lp::LPConstraint> &constraints, double infinity);
  void add_numeric_preconditions_constraints(
      std::vector<lp::LPConstraint> &constraints, double infinity);
  void add_numeric_goals_constraints(
      std::vector<lp::LPConstraint> &constraints);
  void add_numeric_effects_constraints(
      std::vector<lp::LPConstraint> &constraints, double infinity);
  void add_numeric_counters_constraints(
      std::vector<lp::LPConstraint> &constraints, double infinity);
  void add_numeric_sequencing_constraints(
      std::vector<lp::LPConstraint> &constraints, double infinity);

  void iterative_variable_elimination(const State &state,
                                      vector<bool> &fact_eliminated,
                                      vector<bool> &action_eliminated);

  void inverse_action_detection();

  bool set_include(const set<int> &first, const set<int> &second);

  // update fact and action eliminated, return true if something is change,
  // false if not;
  bool dominated_action_elimination(const State &state);
  bool dominated_seq_action_elimination(const State &state);
  bool dominated_action_first_condition(int i, int j);
  bool dominated_action_second_condition(int i, int j, const State &state);
  bool dominated_seq_condition(int i, int j);
  bool dominated_action_fourth_condition(int i, int j);
  bool dominated_action_seq_fourth_condition(int i);

  bool relevant_action_reduction(const State &state);

  void build_first_achiever(vector<set<int>> &landmarks_table);
  void build_achiever();

  std::vector<int> indices_m_a;
  std::vector<int> indices_u_a;
  std::vector<int> indices_u_p;
  std::vector<int> indices_u_c;
  std::vector<std::vector<int>> indices_e_a_p;
  std::vector<std::vector<int>> indices_e_a_c;
  std::vector<std::vector<int>> indices_m_a_c;
  std::vector<int> indices_t_a;
  std::vector<int> indices_t_p;
  std::vector<int> indices_t_c;
  // elimination
  std::vector<bool> fact_eliminated;
  std::vector<bool> action_eliminated;
  std::vector<vector<bool>>
      fadd;  // first time it is added: first index action, second index
             // condition, value: true or false
  std::vector<set<int>>
      first_achievers;  // first index condition, value: set of actions
  std::vector<vector<bool>>
      action_landmarks;  // first index action, value: set of fact landmarks

  set<int> goals;
  std::unique_ptr<landmarks::LandmarkFactoryScala> factory;
  vector<set<int>>
      inverse_actions;  // index: action, value: set of inverse actions

  vector<bool> relevant_actions;
  vector<bool> relevant_facts;
  bool numeric_condition_satisfied(int n, const State &state);

 public:
  virtual void initialize_variables(const std::shared_ptr<AbstractTask> task,
                                    std::vector<lp::LPVariable> &variables,
                                    double infinity);

  virtual void initialize_constraints(
      const std::shared_ptr<AbstractTask> task,
      std::vector<lp::LPConstraint> &constraints, double infinity);
  virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
  static bool basic_constraints;
  static bool landmark_constraints;
  static bool enhanced_seq_constraints;
  static bool relevance_constraints;
  static bool dominance_constraints;
  static bool inverse_constraints;
  static bool temporal_constraints;
  static bool ignore_numeric;
};

void add_dr_solver_option_to_parser(OptionParser &parser);
void add_lp_solver_option_to_parser(OptionParser &parser);
}  // namespace operator_counting

#endif
