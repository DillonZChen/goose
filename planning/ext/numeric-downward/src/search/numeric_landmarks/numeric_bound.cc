#include "numeric_bound.h"

#include <cmath>
#include <iostream>
#include <limits>

namespace numeric_bound {

  void NumericBound::initialize(const numeric_helper::NumericTaskProxy &task, double precision) {
    this->task = std::make_shared<numeric_helper::NumericTaskProxy>(task);
    this->precision = precision;

    size_t n_variables = this->task->get_n_numeric_variables();
    size_t n_actions = this->task->get_n_actions();

    variable_has_ub = std::vector<bool>(n_variables, false);
    variable_has_lb = std::vector<bool>(n_variables, false);
    variable_ub = std::vector<double>(n_variables, std::numeric_limits<double>::max());
    variable_lb = std::vector<double>(n_variables, std::numeric_limits<double>::lowest());

    effect_has_ub = std::vector<std::vector<bool>>(n_actions, std::vector<bool>(n_variables, false));
    effect_has_lb = std::vector<std::vector<bool>>(n_actions, std::vector<bool>(n_variables, false));
    effect_ub = std::vector<std::vector<double>>(n_actions, std::vector<double>(n_variables, std::numeric_limits<double>::max()));
    effect_lb = std::vector<std::vector<double>>(n_actions, std::vector<double>(n_variables, std::numeric_limits<double>::lowest()));
    assignment_has_ub = std::vector<std::vector<bool>>(n_actions, std::vector<bool>(n_variables, false));
    assignment_has_lb = std::vector<std::vector<bool>>(n_actions, std::vector<bool>(n_variables, false));
    assignment_ub = std::vector<std::vector<double>>(n_actions, std::vector<double>(n_variables, std::numeric_limits<double>::max()));
    assignment_lb = std::vector<std::vector<double>>(n_actions, std::vector<double>(n_variables, std::numeric_limits<double>::lowest()));

    variable_before_action_has_ub = std::vector<std::vector<bool>>(n_variables, std::vector<bool>(n_actions, false));
    variable_before_action_has_lb = std::vector<std::vector<bool>>(n_variables, std::vector<bool>(n_actions, false));
    variable_before_action_ub = std::vector<std::vector<double>>(n_variables, std::vector<double>(n_actions, std::numeric_limits<double>::max()));
    variable_before_action_lb = std::vector<std::vector<double>>(n_variables, std::vector<double>(n_actions, std::numeric_limits<double>::lowest()));
  }

  void NumericBound::calculate_bounds(const std::vector<double> &state, int iterations) {
    prepare();
    update_before_action_bounds();
    int i = 0;

    while (i < iterations && (update_variable_bounds(state) || update_action_bounds() || update_before_action_bounds())) {
      ++i;
    }
    std::cout << i << std::endl;
  }

  void NumericBound::dump() const {
    size_t n_numeric_variables = task->get_n_numeric_variables();

    std::cout << std::endl;
    std::cout << "variables" << std::endl;

    for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
      std::cout << var_id << " ";
      if (get_variable_has_lb(var_id)) std::cout << " lb: " << get_variable_lb(var_id) << " ";
      if (get_variable_has_ub(var_id)) std::cout << " ub: " << get_variable_ub(var_id) << " ";
      std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "actions" << std::endl;

    size_t n_actions = task->get_n_actions();

    for (size_t op_id = 0; op_id < n_actions; ++op_id) {
      std::cout << std::endl << op_id << std::endl;

      for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
        if ((get_variable_before_action_has_lb(var_id, op_id) && fabs(get_variable_before_action_lb(var_id, op_id) - get_variable_lb(var_id)) >= precision)
            || (get_variable_before_action_has_ub(var_id, op_id) && fabs(get_variable_before_action_ub(var_id, op_id) - get_variable_ub(var_id)) >= precision)) {
          std::cout << var_id << " before ";
          if (get_variable_before_action_has_lb(var_id, op_id)) std::cout << " lb: " << get_variable_before_action_lb(var_id, op_id) << " ";
          if (get_variable_before_action_has_ub(var_id, op_id)) std::cout << " ub: " << get_variable_before_action_ub(var_id, op_id) << " ";
          std::cout << std::endl;
        }

        if (!get_effect_has_lb(op_id, var_id)
            || !get_effect_has_ub(op_id, var_id)
            || fabs(get_effect_lb(op_id, var_id)) >= precision
            || fabs(get_effect_ub(op_id, var_id)) >= precision) {
          std::cout << var_id << " increment ";
          if (get_effect_has_lb(op_id, var_id)) std::cout << " lb: " << get_effect_lb(op_id, var_id) << " ";
          if (get_effect_has_ub(op_id, var_id)) std::cout << " ub: " << get_effect_ub(op_id, var_id) << " ";
          std::cout << std::endl;
        }

        if (get_assignment_has_lb(op_id, var_id) || get_assignment_has_ub(op_id, var_id)) {
          std::cout << var_id << " assignment ";
          if (get_assignment_has_lb(op_id, var_id)) std::cout << " lb: " << get_assignment_lb(op_id, var_id) << " ";
          if (get_assignment_has_ub(op_id, var_id)) std::cout << " ub: " << get_assignment_ub(op_id, var_id) << " ";
          std::cout << std::endl;
        }
      }
    }
  }

  void NumericBound::dump(const TaskProxy &task_proxy) const {
    size_t n_numeric_variables = task->get_n_numeric_variables();
    auto variables = task_proxy.get_numeric_variables();

    std::cout << std::endl;
    std::cout << "variables" << std::endl;

    for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
      auto v = task->get_numeric_variable(var_id);
      auto id = v.id_abstract_task;
      auto name = variables[id].get_name();
      std::cout << name << " ";
      if (get_variable_has_lb(var_id)) std::cout << " lb: " << get_variable_lb(var_id) << " ";
      if (get_variable_has_ub(var_id)) std::cout << " ub: " << get_variable_ub(var_id) << " ";
      std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "actions" << std::endl;

    auto operators = task_proxy.get_operators();
    size_t n_actions = operators.size();

    for (size_t op_id = 0; op_id < n_actions; ++op_id) {
      auto name = operators[op_id].get_name();
      std::cout << std::endl << name << std::endl;

      for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
        auto v = task->get_numeric_variable(var_id);
        auto id = v.id_abstract_task;
        auto name = variables[id].get_name();

        if ((get_variable_before_action_has_lb(var_id, op_id) && fabs(get_variable_before_action_lb(var_id, op_id) - get_variable_lb(var_id)) >= precision)
            || (get_variable_before_action_has_ub(var_id, op_id) && fabs(get_variable_before_action_ub(var_id, op_id) - get_variable_ub(var_id)) >= precision)) {
          std::cout << name << " before ";
          if (get_variable_before_action_has_lb(var_id, op_id)) std::cout << " lb: " << get_variable_before_action_lb(var_id, op_id) << " ";
          if (get_variable_before_action_has_ub(var_id, op_id)) std::cout << " ub: " << get_variable_before_action_ub(var_id, op_id) << " ";
          std::cout << std::endl;
        }

        if (!get_effect_has_lb(op_id, var_id)
            || !get_effect_has_ub(op_id, var_id)
            || fabs(get_effect_lb(op_id, var_id)) >= precision
            || fabs(get_effect_ub(op_id, var_id)) >= precision) {
          std::cout << name << " increment ";
          if (get_effect_has_lb(op_id, var_id)) std::cout << " lb: " << get_effect_lb(op_id, var_id) << " ";
          if (get_effect_has_ub(op_id, var_id)) std::cout << " ub: " << get_effect_ub(op_id, var_id) << " ";
          std::cout << std::endl;
        }

        if (get_assignment_has_lb(op_id, var_id) || get_assignment_has_ub(op_id, var_id)) {
          std::cout << name << " assignment ";
          if (get_assignment_has_lb(op_id, var_id)) std::cout << " lb: " << get_assignment_lb(op_id, var_id) << " ";
          if (get_assignment_has_ub(op_id, var_id)) std::cout << " ub: " << get_assignment_ub(op_id, var_id) << " ";
          std::cout << std::endl;
        }
      }
    }
  }

  void NumericBound::prepare() {
    size_t n_variables = this->task->get_n_numeric_variables();

    for (size_t var_id = 0; var_id < n_variables; ++var_id) {
      variable_has_ub[var_id] = false;
      variable_has_lb[var_id] = false;
      variable_ub[var_id] = std::numeric_limits<double>::max();
      variable_ub[var_id] = std::numeric_limits<double>::lowest();
    }

    size_t n_actions = this->task->get_n_actions();

    for (size_t op_id = 0; op_id < n_actions; ++op_id) {
      for (size_t var_id = 0; var_id < n_variables; ++var_id) {
        effect_has_ub[op_id][var_id] = true;
        effect_has_lb[op_id][var_id] = true;
        effect_ub[op_id][var_id] = 0.0;
        effect_lb[op_id][var_id] = 0.0;
        assignment_has_ub[op_id][var_id] = false;
        assignment_has_lb[op_id][var_id] = false;
        assignment_ub[op_id][var_id] = std::numeric_limits<double>::max();
        assignment_lb[op_id][var_id] = std::numeric_limits<double>::lowest();

        if (this->task->get_action_is_assignment(op_id)[var_id]) {
          assignment_has_ub[op_id][var_id] = true;
          assignment_has_lb[op_id][var_id] = true;
          double constant = task->get_action_assign_list(op_id)[var_id];
          assignment_ub[op_id][var_id] = constant;
          assignment_lb[op_id][var_id] = constant;
        } else {
          double simple_effect = task->get_action_eff_list(op_id)[var_id];

          if (fabs(simple_effect) >= precision) {
            effect_ub[op_id][var_id] = simple_effect;
            effect_lb[op_id][var_id] = simple_effect;
          }
        }
      }

      for (auto var_eff : this->task->get_action_conditional_assign_list(op_id)) {
        auto var_id = var_eff.first;
        assignment_has_ub[op_id][var_id] = true;
        assignment_has_lb[op_id][var_id] = true;
        double constant = var_eff.second;
        assignment_ub[op_id][var_id] = constant;
        assignment_lb[op_id][var_id] = constant;
      }

      for (auto var_eff : this->task->get_action_conditional_eff_list(op_id)) {
        auto var_id = var_eff.first;
        double simple_effect = var_eff.second;
        effect_ub[op_id][var_id] = simple_effect;
        effect_lb[op_id][var_id] = simple_effect;
      }

      for (auto lhs : this->task->get_action_linear_lhs(op_id)) {
        effect_has_ub[op_id][lhs] = false;
        effect_has_lb[op_id][lhs] = false;
        effect_ub[op_id][lhs] = std::numeric_limits<double>::max();
        effect_lb[op_id][lhs] = std::numeric_limits<double>::lowest();
        assignment_has_ub[op_id][lhs] = false;
        assignment_has_lb[op_id][lhs] = false;
        assignment_ub[op_id][lhs] = std::numeric_limits<double>::max();
        assignment_lb[op_id][lhs] = std::numeric_limits<double>::lowest();
      }
    }

    for (size_t var_id = 0; var_id < n_variables; ++var_id) {
      for (size_t op_id = 0; op_id < n_actions; ++op_id) {
        variable_before_action_has_ub[var_id][op_id] = false;
        variable_before_action_has_lb[var_id][op_id] = false;
        variable_before_action_ub[var_id][op_id] = std::numeric_limits<double>::max();
        variable_before_action_lb[var_id][op_id] = std::numeric_limits<double>::lowest();
      }
    }
  } 

  bool NumericBound::update_before_action_bounds() {
    bool change = false;
    size_t n_actions = task->get_n_actions();
    size_t n_numeric_variables = task->get_n_numeric_variables();

    for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
      for (size_t op_id = 0; op_id < n_actions; ++op_id) {
        bool upper_bounded = get_variable_has_ub(var_id);
        bool lower_bounded = get_variable_has_lb(var_id);
        double ub = upper_bounded ? get_variable_ub(var_id) : std::numeric_limits<double>::max();
        double lb = lower_bounded ? get_variable_lb(var_id) : std::numeric_limits<double>::lowest();

        for (auto pre : task->get_action_num_list(op_id)) {
          for (int nc_id : task->get_numeric_conditions_id(pre)){
            auto lnc = task->get_condition(nc_id);
            double w = lnc.coefficients[var_id];
            double k = lnc.constant - task->get_epsilon(nc_id);

            // possibly bounded
            if (fabs(w) >= precision) {
              bool condition_bounded = true;

              for (size_t another_id = 0; another_id < n_numeric_variables; ++another_id) {
                if (another_id == var_id) continue;
                double another_w = lnc.coefficients[another_id];

                if (another_w >= precision) {
                  if (get_variable_before_action_has_ub(another_id, op_id)) {
                    k += another_w * get_variable_before_action_ub(another_id, op_id);
                  } else {
                    condition_bounded = false;
                    break;
                  }
                } else if (another_w <= -precision) {
                  if (get_variable_before_action_has_lb(another_id, op_id)) {
                    k += another_w * get_variable_before_action_lb(another_id, op_id);
                  } else {
                    condition_bounded = false;
                    break;
                  }
                }
              }

              if (condition_bounded) {
                if (w <= -precision) {
                  if (!upper_bounded) upper_bounded = true;
                  ub = std::min(ub, - k / w);
                }

                if (w >= precision) {
                  if (!lower_bounded) lower_bounded = true;
                  lb = std::max(lb, - k / w);
                }
              }
            }
          }
        }
          
        if (upper_bounded
            && (!variable_before_action_has_ub[var_id][op_id]
                || fabs(variable_before_action_ub[var_id][op_id] - ub) >= precision)) {
          change = true;
          variable_before_action_has_ub[var_id][op_id] = true;
          variable_before_action_ub[var_id][op_id] = ub;
        }

        if (lower_bounded
            && (!variable_before_action_has_lb[var_id][op_id]
                || fabs(variable_before_action_lb[var_id][op_id] - lb) >= precision)) {
          change = true;
          variable_before_action_has_lb[var_id][op_id] = true;
          variable_before_action_lb[var_id][op_id] = lb;
        }
      }
    }

    return change;
  }

  bool NumericBound::update_variable_bounds(const std::vector<double> &state) {
    bool change = false;
    size_t n_actions = task->get_n_actions();
    size_t n_numeric_variables = task->get_n_numeric_variables();

    for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
      bool has_ub = true;
      bool has_lb = true;
      double ub = state[var_id];
      double lb = state[var_id];

      for (size_t op_id = 0; op_id < n_actions; ++op_id) {
        if (has_ub) {
          bool upper_bounded = false;
          bool has_effect = false;
          double local_ub = std::numeric_limits<double>::max();

          if (get_effect_has_ub(op_id, var_id)) {
            double increment = get_effect_ub(op_id, var_id);

            if (increment < precision) {
              upper_bounded = true;
            } else if (get_variable_before_action_has_ub(var_id, op_id)) {
              upper_bounded = true;
              has_effect = true;
              local_ub = increment + get_variable_before_action_ub(var_id, op_id);
            }
          }

          if ((!upper_bounded || has_effect) && get_assignment_has_ub(op_id, var_id)) {
            upper_bounded = true;
            has_effect = true;
            local_ub = std::min(local_ub, get_assignment_ub(op_id, var_id));
          }

          if (has_effect) ub = std::max(ub, local_ub);
          if (!upper_bounded) has_ub = false;
        }

        if (has_lb) {
          bool lower_bounded = false;
          bool has_effect = false;
          double local_lb = std::numeric_limits<double>::lowest();

          if (get_effect_has_lb(op_id, var_id)) {
            double increment = get_effect_lb(op_id, var_id);

            if (increment > -precision) {
              lower_bounded = true;
            } else if (get_variable_before_action_has_lb(var_id, op_id)) {
              lower_bounded = true;
              has_effect = true;
              local_lb = increment + get_variable_before_action_lb(var_id, op_id);
            }
          }

          if ((!lower_bounded || has_effect) && get_assignment_has_lb(op_id, var_id)) {
            lower_bounded = true;
            has_effect = true;
            local_lb = std::max(local_lb, get_assignment_lb(op_id, var_id));
          }

          if (has_effect) lb = std::min(lb, local_lb);
          if (!lower_bounded) has_lb = false;
        }

        if (!has_ub && !has_lb) break;
      }

      if (has_ub) {
        if (!change
            && (!variable_has_ub[var_id]
                || fabs(variable_ub[var_id] - ub) >= precision)) {
          change = true;
        }

        variable_has_ub[var_id] = true;
        variable_ub[var_id] = ub;
      }

      if (has_lb) {
        if (!change
            && (!variable_has_lb[var_id]
                || fabs(variable_lb[var_id] - lb) >= precision)) {
          change = true;
        }

        variable_has_lb[var_id] = true;
        variable_lb[var_id] = lb;
      }
    }

    return change;
  }

  bool NumericBound::update_action_bounds() {
    bool change = false;
    size_t n_actions = task->get_n_actions();
    size_t n_numeric_variables = task->get_n_numeric_variables();

    for (size_t op_id = 0; op_id < n_actions; ++op_id) {
      int n_linear_eff = task->get_action_n_linear_eff(op_id);

      for (int i = 0; i < n_linear_eff; ++i) {
        auto lhs = task->get_action_linear_lhs(op_id)[i];
        auto constant = task->get_action_linear_constants(op_id)[i];
        auto coefficients = task->get_action_linear_coefficients(op_id)[i];
        bool has_lb = true;
        bool has_ub = true;
        double ub = constant;
        double lb = constant;

        for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
          if (var_id != static_cast<size_t>(lhs)) {
            double w = coefficients[var_id];

            if (has_ub) {
              if (w >= precision && get_variable_before_action_has_ub(var_id, op_id)) {
                ub += coefficients[var_id] * get_variable_before_action_ub(var_id, op_id);
              } else if (w <= -precision && get_variable_before_action_has_lb(var_id, op_id)) {
                ub += coefficients[var_id] * get_variable_before_action_lb(var_id, op_id);
              } else if (fabs(w) >= precision) {
                has_ub = false;
              }
            }

            if (has_lb) {
              if (w >= precision && get_variable_before_action_has_lb(var_id, op_id)) {
                lb += coefficients[var_id] * get_variable_before_action_lb(var_id, op_id);
              } else if (w <= -precision && get_variable_before_action_has_ub(var_id, op_id)) {
                lb += coefficients[var_id] * get_variable_before_action_ub(var_id, op_id);
              } else if (fabs(w) >= precision) {
                has_lb = false;
              }
            }
          }

          if (!has_ub && !has_lb) break;
        }

        bool new_assignment_has_ub = false;
        bool new_assignment_has_lb = false;
        double new_assignment_ub = std::numeric_limits<double>::max();
        double new_assignment_lb = std::numeric_limits<double>::lowest();
        bool new_effect_has_ub = false;
        bool new_effect_has_lb = false;
        double new_effect_ub = std::numeric_limits<double>::max();
        double new_effect_lb = std::numeric_limits<double>::lowest();

        if (has_ub) {
          if (fabs(coefficients[lhs]) < precision) {
            new_assignment_has_ub = true;
            new_assignment_ub = ub;
          } else if (coefficients[lhs] >= precision && get_variable_before_action_has_ub(lhs, op_id)) {
            new_assignment_has_ub = true;
            new_assignment_ub = ub + coefficients[lhs] * get_variable_before_action_ub(lhs, op_id);
          } else if (coefficients[lhs] <= -precision && get_variable_before_action_has_lb(lhs, op_id)) {
            new_assignment_has_ub = true;
            new_assignment_ub = ub + coefficients[lhs] * get_variable_before_action_lb(lhs, op_id);
          }

          double increment_coefficient = coefficients[lhs] - 1.0;

          if (fabs(increment_coefficient) < precision) {
            new_effect_has_ub = true;
            new_effect_ub  = ub;
          } else if (increment_coefficient >= precision && get_variable_before_action_has_ub(lhs, op_id)) {
            new_effect_has_ub = true;
            new_effect_ub = ub + increment_coefficient * get_variable_before_action_has_ub(lhs, op_id);
          } else if (increment_coefficient <= -precision && get_variable_before_action_has_lb(lhs, op_id)) {
            new_effect_has_ub = true;
            new_effect_ub = ub + increment_coefficient * get_variable_before_action_lb(lhs, op_id);
          }
        }

        if (has_lb) {
          if (fabs(coefficients[lhs]) < precision) {
            new_assignment_has_lb = true;
            new_assignment_lb = lb;
          } else if (coefficients[lhs] >= precision && get_variable_before_action_has_lb(lhs, op_id)) {
            new_assignment_has_lb = true;
            new_assignment_lb = lb + coefficients[lhs] * get_variable_before_action_lb(lhs, op_id);
          } else if (coefficients[lhs] <= -precision && get_variable_before_action_has_ub(lhs, op_id)) {
            new_assignment_has_lb = true;
            new_assignment_lb = lb + coefficients[lhs] * get_variable_before_action_ub(lhs, op_id);
          }

          double increment_coefficient = coefficients[lhs] - 1.0;

          if (fabs(increment_coefficient) < precision) {
            new_effect_has_lb = true;
            new_effect_lb = lb;
          } else if (increment_coefficient >= precision && get_variable_before_action_has_lb(lhs, op_id)) {
            new_effect_has_lb = true;
            new_effect_lb = lb + increment_coefficient * get_variable_before_action_has_lb(lhs, op_id);
          } else if (increment_coefficient <= -precision && get_variable_before_action_has_ub(lhs, op_id)) {
            new_effect_has_lb = true;
            new_effect_lb = lb + increment_coefficient * get_variable_before_action_ub(lhs, op_id);
          }
        }

        auto assignment_result = check_coefficient_in_preconditions(coefficients, op_id);

        if (assignment_result.first.first) {
          new_assignment_has_ub = true;
          new_assignment_ub = std::min(new_assignment_ub, assignment_result.second.first + constant);
        }

        if (assignment_result.first.second) {
          new_assignment_has_lb = true;
          new_assignment_lb = std::max(new_assignment_lb, assignment_result.second.second + constant);
        }

        auto increment_coefficients = coefficients;
        increment_coefficients[lhs] -= 1.0;
        auto increment_result = check_coefficient_in_preconditions(increment_coefficients, op_id);

        if (increment_result.first.first) {
          new_effect_has_ub = true;
          new_effect_ub = std::min(new_effect_ub, increment_result.second.first + constant);
        }

        if (increment_result.first.second) {
          new_effect_has_lb = true;
          new_effect_lb = std::max(new_effect_lb, increment_result.second.second + constant);
        }

        if (new_assignment_has_ub
            && (!assignment_has_ub[op_id][lhs] || fabs(new_assignment_ub - assignment_ub[op_id][lhs]) >= precision)) {
          change = true;
          assignment_has_ub[op_id][lhs] = true;
          assignment_ub[op_id][lhs] = new_assignment_ub;
        }

        if (new_assignment_has_lb
            && (!assignment_has_lb[op_id][lhs] || fabs(new_assignment_lb - assignment_lb[op_id][lhs]) >= precision)) {
          change = true;
          assignment_has_lb[op_id][lhs] = true;
          assignment_lb[op_id][lhs] = new_assignment_lb;
        }

        if (new_effect_has_ub
            && (!effect_has_ub[op_id][lhs] || fabs(new_effect_ub - effect_ub[op_id][lhs]) >= precision)) {
          change = true;
          effect_has_ub[op_id][lhs] = true;
          effect_ub[op_id][lhs] = new_effect_ub;
        }

        if (new_effect_has_lb
            && (!effect_has_lb[op_id][lhs] || fabs(new_effect_lb - effect_lb[op_id][lhs]) >= precision)) {
          change = true;
          effect_has_lb[op_id][lhs] = true;
          effect_lb[op_id][lhs] = new_effect_lb;
        }
      }
    }

    return change;
  }

  std::pair<std::pair<bool, bool>, std::pair<double, double>> NumericBound::check_coefficient_in_preconditions(
    const std::vector<double> &coefficients, size_t op_id) const {
    bool has_ub = false;
    bool has_lb = false;
    double ub = std::numeric_limits<double>::max();
    double lb = std::numeric_limits<double>::lowest();

    for (auto pre : task->get_action_num_list(op_id)) {
      for (int nc_id : task->get_numeric_conditions_id(pre)){
        auto lnc = task->get_condition(nc_id);
        bool has_scale = true;
        bool scale_initialized = false;
        double scale = 0.0;

        for (size_t n_id = 0; n_id < task->get_n_numeric_variables(); ++n_id) {
          if (fabs(coefficients[n_id]) >= precision && fabs(lnc.coefficients[n_id]) >= precision) {
            double new_scale = coefficients[n_id] / lnc.coefficients[n_id];

            if (!scale_initialized) {
              scale = new_scale;
            } else if (fabs(new_scale - scale) >= precision) {
              has_scale = false;
              break;
            }
          } else if (fabs(coefficients[n_id]) >= precision || fabs(lnc.coefficients[n_id]) >= precision) {
            has_scale = false;
            break;
          }
        }

        if (has_scale) {
          if (scale >= precision) {
            if (!has_lb) has_lb = true;
            lb = std::max(lb, (-lnc.constant + task->get_epsilon(nc_id)) * scale);
          } else if (scale <= -precision) {
            if (!has_ub) has_ub = true;
            ub = std::min(ub, (-lnc.constant + task->get_epsilon(nc_id)) * scale);
          }
        }
      }
    }

    return std::make_pair(std::make_pair(has_ub, has_lb), std::make_pair(ub, lb));
  }
}