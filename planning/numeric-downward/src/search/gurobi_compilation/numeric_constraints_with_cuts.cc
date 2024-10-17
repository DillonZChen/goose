#include "numeric_constraints_with_cuts.h"

#include "../globals.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace gurobi_ip_compilation;
using namespace numeric_helper;

NumericConstraintsWithCuts::NumericConstraintsWithCuts(const Options &opts)
    : NumericConstraints(opts),
      precondition_relaxation(
          opts.get<bool>("precondition_relaxation")),
      sequence_linear_effects(opts.get<bool>("sequence_linear_effects")) {}

void NumericConstraintsWithCuts::initialize(
    const int horizon, const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex, bool use_linear_effects) {
  cout << "initializing numeric with cuts" << endl;
  NumericConstraints::initialize(horizon, task, action_mutex, use_linear_effects);
}

void NumericConstraintsWithCuts::initialize_numeric_mutex(
    std::vector<std::vector<bool>> &action_mutex) {
  size_t n_actions = numeric_task.get_n_actions();
  int n_numeric_variables = numeric_task.get_n_numeric_variables();
  precondition_to_negative.resize(n_actions, std::vector<bool>(n_actions, false));
  if (has_linear_effects) {
    precondition_to_linear.resize(n_actions, std::vector<bool>(n_actions, false));
    if (sequence_linear_effects)
      simple_to_linear.resize(n_actions, std::vector<bool>(n_actions, false));
  }

  for (size_t op_id1 = 0; op_id1 < n_actions; ++op_id1) {
    for (size_t op_id2 = 0; op_id2 < n_actions; ++op_id2) {
      if (op_id1 == op_id2 || action_mutex[op_id1][op_id2]) continue;
      // pre(op_id1) vs. num(op_id2)
      for (int pre : numeric_task.get_action_num_list(op_id1)) {
        for (int i : numeric_task.get_numeric_conditions_id(pre)) {
          auto key = std::make_pair(i, op_id2);
          // pre(op_id1) vs. simple(op_id2)
          auto result = net_values.find(key);
          ap_float net = 0;
          if (result == net_values.end()) {
            if (net_effect_actions.find(i) == net_effect_actions.end())
              net_effect_actions[i] = std::vector<int>();
            const LinearNumericCondition &lnc = numeric_task.get_condition(i);
            for (int nv_id = 0; nv_id < n_numeric_variables; ++nv_id) {
              net += lnc.coefficients[nv_id] *
                     numeric_task.get_action_eff_list(op_id2)[nv_id];
            }
            net_values[key] = net;
            net_effect_actions[i].push_back(op_id2);
          } else {
            net = result->second;
          }
          if (net < 0.0) {
            precondition_to_negative[op_id1][op_id2] = true;
          }

          // pre(op_id1) vs. linear(op_id2)
          if (has_linear_effects) {
            const LinearNumericCondition &lnc = numeric_task.get_condition(i);
            for (int j = 0; j < numeric_task.get_action_n_linear_eff(op_id2);
                 ++j) {
              if (!precondition_to_negative[op_id1][op_id1]) {
                int lhs = numeric_task.get_action_linear_lhs(op_id2)[j];
                if (fabs(lnc.coefficients[lhs]) > 0) {
                  precondition_to_linear[op_id1][op_id2] = true;
                  break;
                }
              }
            }
          }
        }
      }
      // rhs(op_id1) vs. lhs(op_id2)
      if (has_linear_effects) {
        if (action_mutex[op_id1][op_id2]) continue;
        // linear rhs vs. lhs
        for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id1); ++i) {
          for (int rhs = 0; rhs < n_numeric_variables; ++rhs) {
            if (fabs(numeric_task.get_action_linear_coefficients(op_id1)[i][rhs]) > 0) {
              // linear rhs vs. linear lhs
              for (int j = 0; j < numeric_task.get_action_n_linear_eff(op_id2); ++j) {
                if (fabs(numeric_task.get_action_linear_coefficients(op_id2)[j][rhs] > 0)) {
                  action_mutex[op_id1][op_id2] = true;
                  action_mutex[op_id2][op_id1] = true;
                  break;
                }
              }
              if (action_mutex[op_id1][op_id2]) break;
              // linear rhs vs. simple lhs
              if (fabs(numeric_task.get_action_eff_list(op_id2)[rhs]) > 0) {
                if (sequence_linear_effects) {
                  simple_to_linear[op_id2][op_id1] = true;
                  break;
                } else {
                  action_mutex[op_id1][op_id2] = true;
                  action_mutex[op_id2][op_id1] = true;
                  break;
                }
              }
            }
          }
          if (action_mutex[op_id1][op_id2]) break;
        }
        if (action_mutex[op_id1][op_id2]) continue;
        // simple rhs vs. linear lhs
        for (int rhs = 0; rhs < n_numeric_variables; ++rhs) {
          for (int i = 0; i < numeric_task.get_action_n_linear_eff(op_id2); ++i) {
            int lhs = numeric_task.get_action_linear_lhs(op_id2)[i];
            if (fabs(numeric_task.get_action_eff_list(op_id1)[lhs]) > 0) {
              if (sequence_linear_effects) {
                simple_to_linear[op_id1][op_id2] = true;
                break;
              } else {
                action_mutex[op_id1][op_id2] = true;
                action_mutex[op_id2][op_id1] = true;
                break;
              }
            }
          }
          if (action_mutex[op_id1][op_id2] || (sequence_linear_effects && simple_to_linear[op_id1][op_id2]))
            break;
        }
      }
    }
  }
}

void NumericConstraintsWithCuts::compute_big_m_values(
    const std::shared_ptr<AbstractTask> task, int t_min, int t_max,
    bool first) {
  if (has_linear_effects && sequence_linear_effects) {
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
                double k_over = large_m[t - 1][nv_id2];
                double k_under = small_m[t - 1][nv_id2];
                for (size_t op_id2 = 0; op_id2 < n_actions; ++op_id2) {
                  if (op_id != op_id2 && action_precedence_inner[op_id2][op_id]) {
                    double k = numeric_task.get_action_eff_list(op_id2)[nv_id2];
                    if (fabs(k) > 0) {
                      if (k > 0.0) {
                        k_over += k;
                      } else {
                        k_under += k;
                      }
                    }
                  }
                }
                if (coefficient > 0) {
                  a_over += coefficient * k_over;
                  a_under += coefficient * k_under;
                }
                if (coefficient < 0) {
                  a_over += coefficient * k_under;
                  a_under += coefficient * k_over;
                }
              }
              break;
            }
          }
          ub = std::max(ub, a_over);
          lb = std::min(lb, a_under);
        }
        large_m[t][nv_id] = ub;
        small_m[t][nv_id] = lb;
      }
    }
  } else {
    NumericConstraints::compute_big_m_values(task, t_min, t_max, first);
  }
}

void NumericConstraintsWithCuts::add_action_precedence(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_precedence,
    std::vector<std::vector<bool>> &action_mutex) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  size_t n_actions = numeric_task.get_n_actions();
  action_precedence_inner.resize(n_actions, std::vector<bool>(n_actions, false));
  // detect mutex action pairs and precedence releations
  for (size_t op_id1 = 0; op_id1 < n_actions - 1; ++op_id1) {
    for (size_t op_id2 = op_id1 + 1; op_id2 < n_actions; ++op_id2) {
      // mandatory precedence
      bool edge1 = action_precedence[op_id1][op_id2];
      bool edge2 = action_precedence[op_id2][op_id1];
      if (has_linear_effects) {
        if (!edge1)
          edge1 = precondition_to_linear[op_id1][op_id2] || (sequence_linear_effects && simple_to_linear[op_id1][op_id2]);
        if (!edge2)
          edge2 = precondition_to_linear[op_id2][op_id1] || (sequence_linear_effects && simple_to_linear[op_id2][op_id1]);
      }
      if (edge1 && edge2) {
        action_mutex[op_id1][op_id2] = true;
        action_mutex[op_id2][op_id1] = true;
      } else if (edge1) {
        if (precondition_relaxation) {
          action_precedence[op_id1][op_id2] = true;
        } else if (precondition_to_negative[op_id2][op_id1]) {
          action_mutex[op_id1][op_id2] = true;
          action_mutex[op_id2][op_id1] = true;
        } else {
          action_precedence[op_id1][op_id2] = true;
        }
      } else if (edge2) {
        if (precondition_relaxation) {
          action_precedence[op_id2][op_id1] = true;
        } else if (precondition_to_negative[op_id1][op_id2]) {
          action_mutex[op_id1][op_id2] = true;
          action_mutex[op_id2][op_id1] = true;
        } else {
          action_precedence[op_id2][op_id1] = true;
        }
      } else if (precondition_to_negative[op_id1][op_id2] && precondition_to_negative[op_id2][op_id1]) {
        action_mutex[op_id1][op_id2] = true;
        action_mutex[op_id2][op_id1] = true;
      } else if (precondition_to_negative[op_id1][op_id2]) {
        action_precedence[op_id1][op_id2] = true;
      } else if (precondition_to_negative[op_id2][op_id1]) {
        action_precedence[op_id2][op_id1] = true;
      }
      if (action_mutex[op_id1][op_id2]) {
        action_precedence[op_id1][op_id2] = false;
        action_precedence[op_id2][op_id1] = false;
      }
      action_precedence_inner[op_id1][op_id2] = action_precedence[op_id1][op_id2];
      action_precedence_inner[op_id2][op_id1] = action_precedence[op_id2][op_id1];
      //if (action_precedence[op_id1][op_id2]) {
      //  std::cout << ops[op_id1].get_name() << " -> " << ops[op_id2].get_name() << std::endl;
      //}
      //if (action_precedence[op_id2][op_id1]) {
      //  std::cout << ops[op_id2].get_name() << " -> " << ops[op_id1].get_name() << std::endl;
      //}
      //if (action_mutex[op_id1][op_id2]) {
      //  std::cout << ops[op_id1].get_name() << " <-> " << ops[op_id2].get_name() << std::endl;
      //}
    }
  }
}

void NumericConstraintsWithCuts::precondition_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  if (precondition_relaxation) {
    int n_numeric_variables = numeric_task.get_n_numeric_variables();
    int n_actions = numeric_task.get_n_actions();
    for (int op_id = 0; op_id < n_actions; ++op_id) {
      for (int pre : numeric_task.get_action_num_list(op_id)) {
        for (int i : numeric_task.get_numeric_conditions_id(pre)) {
          auto effect_actions = net_effect_actions.find(i);

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

            GRBLinExpr lhs(lnc.constant);

            for (size_t var = 0; var < numeric_task.get_n_numeric_variables();
                 ++var) {
              double coefficient = lnc.coefficients[var];
              if (fabs(coefficient) > 0)
                lhs.addTerms(&coefficient, &y[t][var], 1);
            }

            if (effect_actions != net_effect_actions.end()) {
              for (auto op_id2 : effect_actions->second) {
                if (op_id2 != op_id && action_precedence_inner[op_id2][op_id]) {
                  auto result = net_values.find(std::make_pair(i, op_id2));
                  if (result != net_values.end()) {
                    double net_value = result->second;
                    lhs.addTerms(&net_value, &x[t][op_id2], 1);
                    if (net_value < 0) big_m += net_value;
                  }
                }
              }
            }

            GRBLinExpr rhs(big_m * (1 - x[t][op_id]));
            model->addConstr(lhs >= rhs + numeric_task.get_epsilon(i));
          }
        }
      }
    }
  } else {
    NumericConstraints::precondition_constraint(task, model, x, t_min, t_max);
  }
}

void NumericConstraintsWithCuts::linear_effect_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  if (has_linear_effects && sequence_linear_effects) {
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
              double k_simple_over = large_m[t][rhs_var];
              double k_simple_under = small_m[t][rhs_var];

              for (size_t op_id2 = 0; op_id2 < numeric_task.get_n_actions();
                   ++op_id2) {
                if (op_id != op_id2 && action_precedence_inner[op_id2][op_id]) {
                  double k = numeric_task.get_action_eff_list(op_id2)[rhs_var];
                  if (fabs(k) > 0) {
                    double coefficient2 = coefficient * k;
                    rhs.addTerms(&coefficient2, &x[t][op_id2], 1);
                    if (k > 0.0) {
                      k_simple_over += k;
                    } else {
                      k_simple_under += k;
                    }
                  }
                }
              }

              if (coefficient > 0.0) {
                large_m_a -= coefficient * k_simple_under;
                small_m_a -= coefficient * k_simple_over;
              } else {
                large_m_a -= coefficient * k_simple_over;
                small_m_a -= coefficient * k_simple_under;
              }
            }
          }
          model->addConstr(y[t + 1][var] <=
                           rhs + large_m_a * (1 - x[t][op_id]));
          model->addConstr(y[t + 1][var] >=
                           rhs + small_m_a * (1 - x[t][op_id]));
        }
      }
    }
  } else {
    NumericConstraints::linear_effect_constraint(task, model, x, t_min, t_max);
  }
}

static shared_ptr<GurobiIPConstraintGenerator> _parse(OptionParser &parser) {
  parser.document_synopsis(
      "Numeric constraints with cuts.",
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
  parser.add_option<int>(
      "num_repetition",
      "Maximum number of the same actions at the same time step", "1");
  parser.add_option<bool>("restrict_mutex",
                          "Whether to further restrict mutex actions", "false");
  parser.add_option<bool>("precondition_relaxation",
                          "Enable relaxed precondition constraints", "false");
  parser.add_option<bool>("sequence_linear_effects",
                          "Enforce that actions with linear effects are "
                          "executed after actions with simple effects",
                          "false");
  Options opts = parser.parse();

  if (parser.dry_run()) return nullptr;
  return make_shared<NumericConstraintsWithCuts>(opts);
}

static PluginShared<GurobiIPConstraintGenerator> _plugin("numeric_cut", _parse);
