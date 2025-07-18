#ifndef NUMERIC_BOUND_H
#define NUMERIC_BOUND_H

#include <memory>
#include <vector>

#include "../numeric_operator_counting/numeric_helper.h"

namespace numeric_bound {

class NumericBound {
  public:
    NumericBound() : task(nullptr), precision(1e-6) {}

    NumericBound(const numeric_helper::NumericTaskProxy &task, double precision = 1e-6) {
      initialize(task, precision);
    }

    void initialize(const numeric_helper::NumericTaskProxy &task, double precision = 1e-6);

    void calculate_bounds(const std::vector<double> &state, int iterations);

    void dump() const;
    void dump(const TaskProxy &task_proxy) const;

    bool get_variable_has_ub(size_t var_id) const { return variable_has_ub[var_id]; }
    bool get_variable_has_lb(size_t var_id) const { return variable_has_lb[var_id]; }
    double get_variable_ub(size_t var_id) const { return variable_ub[var_id]; }
    double get_variable_lb(size_t var_id) const { return variable_lb[var_id]; }

    bool get_effect_has_ub(size_t op_id, size_t var_id) const { return effect_has_ub[op_id][var_id]; }
    bool get_effect_has_lb(size_t op_id, size_t var_id) const { return effect_has_lb[op_id][var_id]; }
    double get_effect_ub(size_t op_id,size_t var_id) const { return effect_ub[op_id][var_id]; }
    double get_effect_lb(size_t op_id,size_t var_id) const { return effect_lb[op_id][var_id]; }
    bool get_assignment_has_ub(size_t op_id, size_t var_id) const { return assignment_has_ub[op_id][var_id]; }
    bool get_assignment_has_lb(size_t op_id, size_t var_id) const { return assignment_has_lb[op_id][var_id]; }
    double get_assignment_ub(size_t op_id, size_t var_id) const { return assignment_ub[op_id][var_id]; }
    double get_assignment_lb(size_t op_id, size_t var_id) const { return assignment_lb[op_id][var_id]; }

    bool get_action_has_ub(size_t op_id, size_t var_id) const {
      return get_effect_has_ub(op_id, var_id) || get_assignment_has_ub(op_id, var_id);
    }

    bool get_action_has_lb(size_t op_id, size_t var_id) const {
      return get_effect_has_lb(op_id, var_id) || get_assignment_has_lb(op_id, var_id);
    }

    bool get_variable_before_action_has_ub(size_t var_id, size_t op_id) const { return variable_before_action_has_ub[var_id][op_id]; }
    bool get_variable_before_action_has_lb(size_t var_id, size_t op_id) const { return variable_before_action_has_lb[var_id][op_id]; }
    double get_variable_before_action_ub(size_t var_id, size_t op_id) const { return variable_before_action_ub[var_id][op_id]; }
    double get_variable_before_action_lb(size_t var_id, size_t op_id) const { return variable_before_action_lb[var_id][op_id]; }

    bool has_no_increasing_assignment_effect(size_t op_id, size_t var_id) const {
      return get_variable_before_action_has_lb(var_id, op_id)
             && get_assignment_has_ub(op_id, var_id)
             && get_variable_before_action_lb(var_id, op_id) >= get_assignment_ub(op_id, var_id);
    }

    bool has_no_decreasing_assignment_effect(size_t op_id, size_t var_id) const {
      return get_variable_before_action_has_ub(var_id, op_id)
             && get_assignment_has_lb(op_id, var_id)
             && get_variable_before_action_ub(var_id, op_id) <= get_assignment_lb(op_id, var_id);
    }

  private:
    void prepare();
    bool update_before_action_bounds();
    bool update_variable_bounds(const std::vector<double> &state);
    bool update_action_bounds();
    std::pair<std::pair<bool, bool>, std::pair<double, double>> check_coefficient_in_preconditions(
      const std::vector<double> &coefficients, size_t op_id) const;

    std::shared_ptr<const numeric_helper::NumericTaskProxy> task;
    double precision;

    std::vector<bool> variable_has_ub;
    std::vector<bool> variable_has_lb;
    std::vector<double> variable_ub;
    std::vector<double> variable_lb;

    std::vector<std::vector<bool>> effect_has_ub;
    std::vector<std::vector<bool>> effect_has_lb;
    std::vector<std::vector<double>> effect_ub;
    std::vector<std::vector<double>> effect_lb;
    std::vector<std::vector<bool>> assignment_has_ub;
    std::vector<std::vector<bool>> assignment_has_lb;
    std::vector<std::vector<double>> assignment_ub;
    std::vector<std::vector<double>> assignment_lb;

    std::vector<std::vector<bool>> variable_before_action_has_ub;
    std::vector<std::vector<bool>> variable_before_action_has_lb;
    std::vector<std::vector<double>> variable_before_action_ub;
    std::vector<std::vector<double>> variable_before_action_lb;
};

}

#endif