#include "numeric_constraints.h"

#include "../globals.h"
#include "../lp/lp_solver.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace operator_counting;
using namespace numeric_helper;

void NumericConstraints::initialize_variables(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPVariable> &variables, double infinity) {
  cout << "initializing variables for numeric part" << endl;
  TaskProxy task_proxy(*task);
  numeric_task = NumericTaskProxy(task_proxy);
  int n_numeric_vars = numeric_task.get_n_numeric_variables();
  index_numeric_var.assign(n_numeric_vars, vector<int>(t_max, -1));

  add_variables(task, variables, infinity, t_min, t_max);
}

void NumericConstraints::add_variables(const std::shared_ptr<AbstractTask> task,
                                       std::vector<lp::LPVariable> &variables,
                                       double infinity, int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  numeric_task = NumericTaskProxy(task_proxy);
  int n_numeric_vars = numeric_task.get_n_numeric_variables();

  // resize index
  for (int nv_id = 0; nv_id < n_numeric_vars; ++nv_id) {
    index_numeric_var[nv_id].resize(t_max, -1);
  }

  // add decision variables
  for (int t = t_min; t < t_max; ++t) {
    // numeric variables
    for (int nv_id = 0; nv_id < n_numeric_vars; ++nv_id) {
      stringstream name;
      name << "v_" << nv_id << "_" << t;
      index_numeric_var[nv_id][t] = variables.size();
      variables.push_back(lp::LPVariable(-infinity, infinity, 0, name.str(),
                                         lp::LPVariableType::continous));
    }
  }
}

void NumericConstraints::initialize_constraints(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints, double infinity) {
  cout << "initializing constraints for numeric" << endl;
  initialize_numeric_mutex();
  initial_state_constraint(task, constraints);
  goal_state_constraint(task, constraints, infinity, t_max);
  compute_big_m_values(task, t_min, t_max);
  action_precondition_constraint(task, constraints, infinity, t_min, t_max);
  action_simple_effect_constraint(task, constraints, infinity, t_min, t_max);
  action_linear_effect_constraint(task, constraints, infinity, t_min, t_max);
  mutex_relaxtion_constraint(task, constraints, infinity, t_min, t_max);
  //    noop_preconditions_constraint(task, constraints, infinity,t_min,t_max);
  //    noopt_mutex_constraint(task, constraints, infinity,t_min,t_max);
}

bool NumericConstraints::update_constraints(const State &state,
                                            lp::LPSolver &lp_solver) {
  return false;
}

bool NumericConstraints::update_constraints(
    const int horizon, lp::LPSolver &lp_solver,
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPVariable> &variables, double infinity,
    std::vector<lp::LPConstraint> &constraints) {
  cout << "adding constraint from numeric" << endl;
  t_min = t_max;
  t_max = horizon;
  add_variables(task, variables, infinity, t_min, t_max);
  compute_big_m_values(task, t_min, t_max);
  action_precondition_constraint(task, constraints, infinity, t_min, t_max);
  action_simple_effect_constraint(task, constraints, infinity, t_min - 1,
                                  t_max);
  action_linear_effect_constraint(task, constraints, infinity, t_min - 1,
                                  t_max);

  mutex_relaxtion_constraint(task, constraints, infinity, t_min, t_max);
  //    noop_preconditions_constraint(task, constraints, infinity,t_min,t_max);
  //    noopt_mutex_constraint(task, constraints, infinity,t_min,t_max);
  goal_state_constraint(task, constraints, infinity, t_max);
  return false;
}

void NumericConstraints::initial_state_constraint(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints) {
  TaskProxy task_proxy(*task);
  State initial_state = task_proxy.get_initial_state();
  int n_numeric_vars = numeric_task.get_n_numeric_variables();
  for (int nv_id = 0; nv_id < n_numeric_vars; ++nv_id) {
    int id_num = numeric_task.get_numeric_variable(nv_id).id_abstract_task;
    double initial_value = initial_state.nval(id_num);
    lp::LPConstraint constraint(initial_value, initial_value);
    constraint.insert(index_numeric_var[nv_id][0], 1.);
    constraints.push_back(constraint);
  }
}

void NumericConstraints::compute_big_m_values(
    const std::shared_ptr<AbstractTask> task, int t_min, int t_max) {
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  large_m.resize(t_max, std::vector<double>(n_numeric_variables, 0.0));
  small_m.resize(t_max, std::vector<double>(n_numeric_variables, 0.0));
  k_over.resize(t_max, std::vector<double>(n_numeric_variables, 0.0));
  k_under.resize(t_max, std::vector<double>(n_numeric_variables, 0.0));

  size_t n_actions = numeric_task.get_n_actions();
  TaskProxy task_proxy(*task);
  State initial_state = task_proxy.get_initial_state();
  for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
    int id_num = numeric_task.get_numeric_variable(nv_id).id_abstract_task;
    double initial_value = initial_state.nval(id_num);
    large_m[0][nv_id] = initial_value;
    small_m[0][nv_id] = initial_value;
  }

  for (int t = std::max(t_min, 1); t < t_max; ++t) {
    for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
      k_over[t][nv_id] = large_m[t - 1][nv_id];
      k_under[t][nv_id] = small_m[t - 1][nv_id];
      for (size_t op_id = 0; op_id < n_actions; ++op_id) {
        double k = numeric_task.get_action_eff_list(op_id)[nv_id];
        if (k > 0.0) {
          k_over[t][nv_id] += k;
        } else {
          k_under[t][nv_id] += k;
        }
      }
      large_m[t][nv_id] = k_over[t][nv_id];
      small_m[t][nv_id] = k_under[t][nv_id];
    }
  }
}

void NumericConstraints::action_precondition_constraint(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
    int t_max) {
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
    for (int pre : numeric_task.get_action_num_list(op_id)) {
      for (int i : numeric_task.get_numeric_conditions_id(pre)) {
        for (int t = t_min; t < t_max; ++t) {
          const LinearNumericCondition &lnc = numeric_task.get_condition(i);
          double big_m = lnc.constant - numeric_task.get_epsilon(i);

          for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
            double w = lnc.coefficients[nv_id];
            if (w < 0.0) {
              big_m += w * large_m[t][nv_id];
            } else {
              big_m += w * small_m[t][nv_id];
            }
          }

          double rhs = numeric_task.get_epsilon(i) + big_m - lnc.constant;
          lp::LPConstraint constraint(rhs, infinity);
          constraint.insert((*index_opt)[op_id][t], big_m);
          for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables();
               ++n_id) {
            double coefficient = lnc.coefficients[n_id];
            if (fabs(coefficient) > 0)
              constraint.insert(index_numeric_var[n_id][t], coefficient);
          }
          if (!constraint.empty()) {
            constraints.push_back(constraint);
          }
        }
      }
    }
  }
}

void NumericConstraints::action_simple_effect_constraint(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
    int t_max) {
  for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id) {
    for (int t = t_min; t < t_max - 1; ++t) {
      lp::LPConstraint constraint_less(0., infinity);
      lp::LPConstraint constraint_great(0., infinity);
      constraint_less.insert(index_numeric_var[n_id][t + 1], 1);
      constraint_less.insert(index_numeric_var[n_id][t], -1);
      constraint_great.insert(index_numeric_var[n_id][t + 1], -1);
      constraint_great.insert(index_numeric_var[n_id][t], 1);
      for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
        constraint_less.insert((*index_opt)[op_id][t],
                               -numeric_task.get_action_eff_list(op_id)[n_id]);
        constraint_great.insert((*index_opt)[op_id][t],
                                numeric_task.get_action_eff_list(op_id)[n_id]);
      }
      constraints.push_back(constraint_less);
      constraints.push_back(constraint_great);
    }
  }
}

void NumericConstraints::action_linear_effect_constraint(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
    int t_max) {
  //    for (size_t fact_id = 0; fact_id < numeric_task.get_n_propositions();
  //    ++fact_id){
  //        set<int> achievers = numeric_task.get_achievers(fact_id);
  //        for(int t = t_min; t < t_max-1; ++t){
  //            lp::LPConstraint constraint(0., infinity);
  //            constraint.insert(index_fact[fact_id][t+1], -1.);
  //            for (int a : achievers) {
  //                constraint.insert((*index_opt)[a][t], 1.);
  //            }
  //            constraint.insert(index_noop_fact[fact_id][t],1.);
  //            constraints.push_back(constraint);
  //        }
  //    }
}

void NumericConstraints::goal_state_constraint(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints, double infinity, int t_max) {
  TaskProxy task_proxy(*task);
  static bool first = goal_index.empty();
  if (first) {
    int n_goals = 0;
    for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals();
         ++id_goal) {
      list<int> numeric_goals = numeric_task.get_numeric_goals(id_goal);
      if (numeric_goals.empty()) continue;  // this is not a numeric goal
      for (int id_n_con : numeric_goals) {
        n_goals++;
      }
    }
    goal_index.assign(n_goals, -1);
  }
  int i_goal = 0;
  // cout << "numeric goals " << numeric_task.get_n_numeric_goals() << endl;
  for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals();
       ++id_goal) {
    list<int> numeric_goals = numeric_task.get_numeric_goals(id_goal);
    if (numeric_goals.empty()) continue;  // this is not a numeric goal
    for (int id_n_con : numeric_goals) {
      const LinearNumericCondition &lnc = numeric_task.get_condition(id_n_con);
      lp::LPConstraint constraint(
          numeric_task.get_epsilon(id_n_con) - lnc.constant, infinity);
      for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables();
           ++n_id) {
        double coefficient = lnc.coefficients[n_id];
        // if (fabs(coefficient)>0.00001)
        constraint.insert(index_numeric_var[n_id][t_max - 1], coefficient);
      }
      if (!constraint.empty()) {
        if (first) {
          goal_index[i_goal] = constraints.size();
          constraints.push_back(constraint);
        } else {
          constraints[goal_index[i_goal]] = constraint;
        }
      }
      i_goal++;
    }
  }
  if (first) first = false;
}

void NumericConstraints::initialize_numeric_mutex() {
  size_t n_actions = numeric_task.get_n_actions();
  numeric_mutex.resize(n_actions, std::vector<bool>(n_actions, false));
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  for (size_t op_id1 = 0; op_id1 < n_actions; ++op_id1) {
    for (size_t op_id2 = 0; op_id2 < n_actions; ++op_id2) {
      if (op_id1 == op_id2 || numeric_mutex[op_id1][op_id2]) continue;
      for (int pre : numeric_task.get_action_num_list(op_id1)) {
        for (int i : numeric_task.get_numeric_conditions_id(pre)) {
          const LinearNumericCondition &lnc = numeric_task.get_condition(i);
          ap_float net = 0;
          for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
            net += lnc.coefficients[nv_id] *
                   numeric_task.get_action_eff_list(op_id2)[nv_id];
          }

          if (net < 0.0) {
            numeric_mutex[op_id1][op_id2] = true;
            numeric_mutex[op_id2][op_id1] = true;
            break;
          }
        }
        if (numeric_mutex[op_id1][op_id2]) break;
      }
    }
  }
}

void NumericConstraints::mutex_relaxtion_constraint(
    const std::shared_ptr<AbstractTask> task,
    std::vector<lp::LPConstraint> &constraints, double infinity, int t_min,
    int t_max) {
  size_t n_actions = numeric_task.get_n_actions();
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  for (size_t op_id1 = 0; op_id1 < n_actions - 1; ++op_id1) {
    for (size_t op_id2 = op_id1 + 1; op_id2 < n_actions; ++op_id2) {
      if (numeric_mutex[op_id1][op_id2]) {
        for (int t = t_min; t < t_max; ++t) {
          lp::LPConstraint constraint(-infinity, 1);
          constraint.insert((*index_opt)[op_id1][t], 1);
          constraint.insert((*index_opt)[op_id2][t], 1);
          constraints.push_back(constraint);
        }
      }
    }
  }
}

static shared_ptr<IPConstraintGenerator> _parse(OptionParser &parser) {
  parser.document_synopsis(
      "Numeric constraints.",
      "For details, see" +
          utils::format_paper_reference(
              {"Chiara Piacentini", "Margarita P. Castro", "Andre A. Cire",
               "J. Chirstopher Beck"},
              "Compiling optimal numeric planning to mixed integer linear "
              "programming",
              "https://aaai.org/ocs/index.php/ICAPS/ICAPS18/paper/view/17770",
              "Proceedings of the Twentyeighth International Conference on"
              " Planning and Scheduling (ICAPS 2018)",
              "383-387", "2018"));

  if (parser.dry_run()) return nullptr;
  return make_shared<NumericConstraints>();
}

static PluginShared<IPConstraintGenerator> _plugin("numeric", _parse);
