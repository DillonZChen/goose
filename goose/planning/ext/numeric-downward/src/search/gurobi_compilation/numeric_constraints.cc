#include "numeric_constraints.h"

#include "../globals.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace gurobi_ip_compilation;
using namespace numeric_helper;

NumericConstraints::NumericConstraints(const Options &opts)
    : current_horizon(0),
      num_repetition(opts.get<int>("num_repetition")),
      restrict_mutex(opts.get<bool>("restrict_mutex")) {}

void NumericConstraints::dump() {
  int n_vars = numeric_task.get_n_numeric_variables();
  for (int t = 0; t < current_horizon + 1; ++t) {
    std::cout << "t=" << t << std::endl;
    for (int var = 0; var < n_vars; ++var) {
      std::cout << y[t][var].get(GRB_DoubleAttr_X) << " ";
    }
    std::cout << std::endl;
  }
}

void NumericConstraints::initialize(
    const int horizon, const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex, bool use_linear_effects) {
  cout << "initializing numeric" << endl;
  TaskProxy task_proxy(*task);
  has_linear_effects = use_linear_effects;
  numeric_task = NumericTaskProxy(task_proxy, false, true);
  initialize_numeric_mutex(action_mutex);

  if (num_repetition > 1) initialize_repetable_actions();
}

void NumericConstraints::update(const int horizon,
                                const std::shared_ptr<AbstractTask> task,
                                std::shared_ptr<GRBModel> model,
                                std::vector<std::vector<GRBVar>> &x) {
  cout << "adding constraint from numeric" << endl;
  bool first = current_horizon == 0;
  int t_min = current_horizon;
  int t_max = horizon;
  compute_big_m_values(task, t_min, t_max, first);
  add_variables(task, model, t_min, t_max, first);
  modify_x(task, model, x, t_min, t_max);
  goal_state_constraint(task, model, t_max, first);
  precondition_constraint(task, model, x, t_min, t_max);
  simple_effect_constraint(task, model, x, t_min, t_max);
  linear_effect_constraint(task, model, x, t_min, t_max);
  if (first) initial_state_constraint(task, model);
  current_horizon = horizon;
}

void NumericConstraints::initialize_numeric_mutex(
    std::vector<std::vector<bool>> &action_mutex) {
  size_t n_actions = numeric_task.get_n_actions();
  numeric_mutex.resize(n_actions, std::vector<bool>(n_actions, false));
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  for (size_t op_id1 = 0; op_id1 < n_actions; ++op_id1) {
    for (size_t op_id2 = 0; op_id2 < n_actions; ++op_id2) {
      if (op_id1 == op_id2 || action_mutex[op_id1][op_id2]) continue;
      for (int pre : numeric_task.get_action_num_list(op_id1)) {
        for (int i : numeric_task.get_numeric_conditions_id(pre)) {
          const LinearNumericCondition &lnc = numeric_task.get_condition(i);
          ap_float net = 0;
          for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
            net += lnc.coefficients[nv_id] *
                   numeric_task.get_action_eff_list(op_id2)[nv_id];
          }

          if (net < 0.0 || (restrict_mutex && net > 0.0)) {
            action_mutex[op_id1][op_id2] = true;
            action_mutex[op_id2][op_id1] = true;
            break;
          }

          if (has_linear_effects) {
            for (int j = 0; j < numeric_task.get_action_n_linear_eff(op_id2);
                 ++j) {
              int lhs = numeric_task.get_action_linear_lhs(op_id2)[j];
              if (fabs(lnc.coefficients[lhs]) > 0.0) {
                action_mutex[op_id1][op_id2] = true;
                action_mutex[op_id2][op_id1] = true;
                break;
              }
            }
          }
          if (action_mutex[op_id1][op_id2]) break;
        }
        if (action_mutex[op_id1][op_id2]) break;
      }

      if (!action_mutex[op_id1][op_id2] && has_linear_effects) {
        // simple lhs vs. linear rhs
        for (int lhs = 0; lhs < n_numeric_variables; ++lhs) {
          if (fabs(numeric_task.get_action_eff_list(op_id1)[lhs]) > 0.0) {
            for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id2);
                 ++i) {
              ap_float coefficient =
                  numeric_task.get_action_linear_coefficients(op_id2)[i][lhs];
              if (fabs(coefficient) > 0.0) {
                action_mutex[op_id1][op_id2] = true;
                action_mutex[op_id2][op_id1] = true;
                break;
              }
            }
          }
          if (action_mutex[op_id1][op_id2]) break;
        }
        if (action_mutex[op_id1][op_id2]) continue;
        // linear lhs vs. rhs
        for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id1); ++i) {
          int lhs = numeric_task.get_action_linear_lhs(op_id1)[i];
          // linear lhs vs. simple rhs
          if (fabs(numeric_task.get_action_eff_list(op_id2)[lhs]) > 0.0) {
            action_mutex[op_id1][op_id2] = true;
            action_mutex[op_id2][op_id1] = true;
            break;
          }
          // linear lhs vs. linear rhs
          for (int j = 0; j < numeric_task.get_action_n_linear_eff(op_id2);
               ++j) {
            ap_float coefficient =
                numeric_task.get_action_linear_coefficients(op_id2)[j][lhs];
            if (fabs(coefficient) > 0.0) {
              action_mutex[op_id1][op_id2] = true;
              action_mutex[op_id2][op_id1] = true;
              break;
            }
          }
          if (action_mutex[op_id1][op_id2]) break;
        }
      }
    }
  }
}

void NumericConstraints::initialize_repetable_actions() {
  size_t n_actions = numeric_task.get_n_actions();
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  repetable.resize(n_actions, false);

  for (size_t op_id = 0; op_id < n_actions; ++op_id) {
    if (numeric_task.get_action_pre_del_list(op_id).size() > 0 ||
        numeric_task.get_action_n_linear_eff(op_id) > 0)
      continue;
    bool is_repetable = true;
    for (int pre : numeric_task.get_action_num_list(op_id)) {
      for (int i : numeric_task.get_numeric_conditions_id(pre)) {
        const LinearNumericCondition &lnc = numeric_task.get_condition(i);
        ap_float net = 0;
        ap_float coefficent_sum = 0.0;
        for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
          net += lnc.coefficients[nv_id] *
                 numeric_task.get_action_eff_list(op_id)[nv_id];
          coefficent_sum += lnc.coefficients[nv_id];
        }

        if (net < 0.0 || coefficent_sum == 0.0) {
          is_repetable = false;
          break;
        }
      }
      if (!is_repetable) break;
    }

    repetable[op_id] = is_repetable;
  }
}

void NumericConstraints::compute_big_m_values(
    const std::shared_ptr<AbstractTask> task, int t_min, int t_max,
    bool first) {
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  large_m.resize(t_max + 1, std::vector<double>(n_numeric_variables, 0.0));
  small_m.resize(t_max + 1, std::vector<double>(n_numeric_variables, 0.0));

  size_t n_actions = numeric_task.get_n_actions();
  TaskProxy task_proxy(*task);
  State initial_state = task_proxy.get_initial_state();

  if (first) {
    for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
      int id_num = numeric_task.get_numeric_variable(nv_id).id_abstract_task;
      double initial_value = initial_state.nval(id_num);
      large_m[0][nv_id] = initial_value;
      small_m[0][nv_id] = initial_value;
    }
  }

  for (int t = t_min + 1; t < t_max + 1; ++t) {
    for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
      double ub = large_m[t - 1][nv_id];
      double lb = small_m[t - 1][nv_id];
      for (size_t op_id = 0; op_id < n_actions; ++op_id) {
        double k = numeric_task.get_action_eff_list(op_id)[nv_id];
        if (k > 0.0) {
          if (num_repetition > 1 && repetable[op_id])
            ub += num_repetition * k;
          else
            ub += k;
        } else {
          if (num_repetition > 1 && repetable[op_id])
            lb += num_repetition * k;
          else
            lb += k;
        }
      }

      if (has_linear_effects) {
        for (size_t op_id = 0; op_id < n_actions; ++op_id) {
          double a_over = ub;
          double a_under = lb;
          for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id);
               ++i) {
            if (nv_id == numeric_task.get_action_linear_lhs(op_id)[i]) {
              a_over = numeric_task.get_action_linear_constants(op_id)[i];
              a_under = numeric_task.get_action_linear_constants(op_id)[i];
              for (int nv_id2 = 0; nv_id2 < n_numeric_variables; ++nv_id2) {
                double coefficient =
                    numeric_task.get_action_linear_coefficients(
                        op_id)[i][nv_id2];
                if (coefficient > 0) {
                  a_over += coefficient * large_m[t - 1][nv_id2];
                  a_under += coefficient * small_m[t - 1][nv_id2];
                }
                if (coefficient < 0) {
                  a_over += coefficient * small_m[t - 1][nv_id2];
                  a_under += coefficient * large_m[t - 1][nv_id2];
                }
              }
              break;
            }
          }
          ub = std::max(ub, a_over);
          lb = std::min(lb, a_under);
        }
      }
      large_m[t][nv_id] = ub;
      small_m[t][nv_id] = lb;
    }
  }
}

void NumericConstraints::add_variables(const std::shared_ptr<AbstractTask> task,
                                       std::shared_ptr<GRBModel> model,
                                       int t_min, int t_max, bool first) {
  TaskProxy task_proxy(*task);
  int n_numeric_vars = numeric_task.get_n_numeric_variables();
  std::vector<char> types(n_numeric_vars, GRB_CONTINUOUS);
  y.resize(t_max + 1, std::vector<GRBVar>(n_numeric_vars));

  if (first) {
    for (int var = 0; var < n_numeric_vars; ++var) {
      std::string name = "y^" + std::to_string(var) + "_0";
      y[0][var] = model->addVar(small_m[0][var], large_m[0][var], 0,
                                GRB_CONTINUOUS, name);
    }
  }

  // add decision variables
  for (int t = t_min + 1; t < t_max + 1; ++t) {
    // numeric variables
    for (int var = 0; var < n_numeric_vars; ++var) {
      std::string name = "y^" + std::to_string(var) + "_" + std::to_string(t);
      y[t][var] = model->addVar(small_m[t][var], large_m[t][var], 0,
                                GRB_CONTINUOUS, name);
    }
  }
}

void NumericConstraints::modify_x(const std::shared_ptr<AbstractTask> task,
                                  std::shared_ptr<GRBModel> model,
                                  std::vector<std::vector<GRBVar>> &x,
                                  int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  if (num_repetition > 1) {
    OperatorsProxy ops = task_proxy.get_operators();
    for (int t = t_min; t < t_max; ++t) {
      for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
        if (repetable[op_id]) {
          x[t][op_id].set(GRB_CharAttr_VType, GRB_INTEGER);
          x[t][op_id].set(GRB_DoubleAttr_UB, num_repetition);
        }
      }
    }
  }
}

void NumericConstraints::initial_state_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model) {
  TaskProxy task_proxy(*task);
  State initial_state = task_proxy.get_initial_state();
  int n_numeric_vars = numeric_task.get_n_numeric_variables();
  for (int nv_id = 0; nv_id < n_numeric_vars; ++nv_id) {
    int id_num = numeric_task.get_numeric_variable(nv_id).id_abstract_task;
    double initial_value = initial_state.nval(id_num);
    model->addConstr(y[0][nv_id] == initial_value);
  }
}

void NumericConstraints::precondition_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
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

          GRBLinExpr rhs(big_m * (1 - x[t][op_id]));
          GRBLinExpr lhs(lnc.constant);

          for (size_t var = 0; var < numeric_task.get_n_numeric_variables();
               ++var) {
            double coefficient = lnc.coefficients[var];
            if (fabs(coefficient) > 0)
              lhs.addTerms(&coefficient, &y[t][var], 1);
          }

          model->addConstr(lhs >= rhs + numeric_task.get_epsilon(i));
        }
      }
    }
  }
}

void NumericConstraints::simple_effect_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  int num_numeric_variables = numeric_task.get_n_numeric_variables();
  for (int var = 0; var < num_numeric_variables; ++var) {
    for (int t = t_min; t < t_max; ++t) {
      GRBLinExpr rhs(y[t][var]);
      for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
        double coefficient = numeric_task.get_action_eff_list(op_id)[var];
        if (fabs(coefficient) > 0) rhs.addTerms(&coefficient, &x[t][op_id], 1);
      }

      if (has_linear_effects) {
        GRBLinExpr les;
        double coefficient = 1;
        bool use_big_m = false;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
          for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id);
               ++i) {
            if (var == numeric_task.get_action_linear_lhs(op_id)[i]) {
              les.addTerms(&coefficient, &x[t][op_id], 1);
              use_big_m = true;
              break;
            }
          }
        }
        if (use_big_m) {
          double large_m_step = large_m[t + 1][var] - small_m[t][var];
          double small_m_step = small_m[t + 1][var] - large_m[t][var];
          model->addConstr(y[t + 1][var] <= rhs + large_m_step * les);
          model->addConstr(y[t + 1][var] >= rhs + small_m_step * les);
        } else {
          model->addConstr(y[t + 1][var] <= rhs);
          model->addConstr(y[t + 1][var] >= rhs);
        }
      } else {
        model->addConstr(y[t + 1][var] <= rhs);
        model->addConstr(y[t + 1][var] >= rhs);
      }
    }
  }
}

void NumericConstraints::linear_effect_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  if (!has_linear_effects) return;
  int num_numeric_variables = numeric_task.get_n_numeric_variables();
  for (int t = t_min; t < t_max; ++t) {
    for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
      for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id); ++i) {
        int var = numeric_task.get_action_linear_lhs(op_id)[i];
        double constant = numeric_task.get_action_linear_constants(op_id)[i];
        GRBLinExpr rhs(constant);
        double large_m_a = large_m[t + 1][var] - constant;
        double small_m_a = small_m[t + 1][var] - constant;
        for (int rhs_var = 0; rhs_var < num_numeric_variables; ++rhs_var) {
          double coefficient =
              numeric_task.get_action_linear_coefficients(op_id)[i][rhs_var];
          if (fabs(coefficient) > 0.0) {
            rhs.addTerms(&coefficient, &y[t][rhs_var], 1);
            if (coefficient > 0.0) {
              large_m_a -= coefficient * small_m[t][rhs_var];
              small_m_a -= coefficient * large_m[t][rhs_var];
            } else {
              large_m_a -= coefficient * large_m[t][rhs_var];
              small_m_a -= coefficient * small_m[t][rhs_var];
            }
          }
        }
        model->addConstr(y[t + 1][var] <= rhs + large_m_a * (1 - x[t][op_id]));
        model->addConstr(y[t + 1][var] >= rhs + small_m_a * (1 - x[t][op_id]));
      }
    }
  }
}

void NumericConstraints::goal_state_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_max, bool first) {
  TaskProxy task_proxy(*task);
  for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals();
       ++id_goal) {
    list<int> numeric_goals = numeric_task.get_numeric_goals(id_goal);
    if (numeric_goals.empty()) continue;  // this is not a numeric goal
    for (int id_n_con : numeric_goals) {
      std::string name = "numeric_goal_" + std::to_string(id_goal) + "_" +
                         std::to_string(id_n_con);
      if (!first) {
        GRBConstr constraint = model->getConstrByName(name);
        model->remove(constraint);
      }
      const LinearNumericCondition &lnc = numeric_task.get_condition(id_n_con);
      GRBLinExpr lhs(lnc.constant);
      for (size_t var = 0; var < numeric_task.get_n_numeric_variables();
           ++var) {
        double coefficient = lnc.coefficients[var];
        if (fabs(coefficient) > 0)
          lhs.addTerms(&coefficient, &y[t_max][var], 1);
      }
      model->addConstr(lhs >= numeric_task.get_epsilon(id_n_con), name);
    }
  }
}

static shared_ptr<GurobiIPConstraintGenerator> _parse(OptionParser &parser) {
  parser.document_synopsis(
      "Numeric constraints.",
      "For details, see" +
          utils::format_paper_reference(
              {"Chiara Piacentini", "Margarita P. Castro", "Andre A. Cire",
               "J. Chirstopher Beck"},
              "Compiling optimal numeric planning to mixed integer "
              "linear "
              "programming",
              "https://aaai.org/ocs/index.php/ICAPS/ICAPS18/paper/"
              "view/17770",
              "Proceedings of the Twentyeighth International "
              "Conference on"
              " Planning and Scheduling (ICAPS 2018)",
              "383-387", "2018"));
  parser.add_option<int>(
      "num_repetition",
      "Maximum number of the same actions at the same time step", "1");
  parser.add_option<bool>("restrict_mutex",
                          "Whether to further restrict mutex actions", "false");
  Options opts = parser.parse();

  if (parser.dry_run()) return nullptr;
  return make_shared<NumericConstraints>(opts);
}

static PluginShared<GurobiIPConstraintGenerator> _plugin("numeric", _parse);
