#include "numeric_lm_cut_landmarks.h"

#include <algorithm>
#include <limits>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "../numeric_operator_counting/numeric_helper.h"


using namespace std;
using namespace numeric_helper;

bool debug = false;

namespace numeric_lm_cut_heuristic {
    // construction and destruction
    LandmarkCutLandmarks::LandmarkCutLandmarks(const TaskProxy &task_proxy, bool ceiling_less_than_one, bool ignore_numeric,
                                               bool use_random_pcf, bool use_irmax, bool disable_ma,
                                               bool use_second_order_simple, ap_float precision, ap_float epsilon,
                                               bool use_constant_assignment, int bound_iterations)
        : numeric_task(NumericTaskProxy(task_proxy, use_constant_assignment, false, epsilon, precision)),
          n_infinite_operators(0),
          n_second_order_simple_operators(0),
          ceiling_less_than_one(ceiling_less_than_one),
          ignore_numeric_conditions(ignore_numeric),
          use_random_pcf(use_random_pcf),
          use_irmax(use_irmax),
          disable_ma(disable_ma),
          use_second_order_simple(use_second_order_simple),
          use_constant_assignment(use_constant_assignment),
          precision(precision),
          epsilon(epsilon),
          use_bounds(bound_iterations > 0),
          numeric_bound(numeric_task, precision) {
        //verify_no_axioms(task_proxy);
        //verify_no_conditional_effects(task_proxy);
        // Build propositions.
        num_propositions = 2; // artificial goal and artificial precondition
        artificial_precondition.is_numeric_condition = false;
        artificial_precondition.name = "artificial";
        artificial_goal.is_numeric_condition = false;
        artificial_goal.name = "goal";
        VariablesProxy variables = task_proxy.get_variables();
        n_var = variables.size();
        propositions.resize(n_var + numeric_task.get_n_numeric_conditions());
        for (FactProxy fact : variables.get_facts()) {
            int var_id = fact.get_variable().get_id();
            RelaxedProposition prop;
            prop.is_numeric_condition = false;//numeric_task.is_numeric_axiom(fact.get_variable().get_id());
            prop.id_numeric_condition = -1;
            prop.name = fact.get_name();
            propositions[var_id].push_back(prop);
            ++num_propositions;
        }

        OperatorsProxy ops = task_proxy.get_operators();
        AxiomsProxy axioms = task_proxy.get_axioms();

        if (!ignore_numeric_conditions) {
            // add numeric conditions
            for (size_t i = 0; i < numeric_task.get_n_numeric_conditions(); i++){
                //LinearNumericCondition &num_values = numeric_task.get_condition(i);
                size_t var_id = n_var + i;
                RelaxedProposition prop;
                prop.is_numeric_condition = true;
                prop.id_numeric_condition = i;
                stringstream name;
                LinearNumericCondition lnc = numeric_task.get_condition(i);
                name << "numeric (" << lnc << ")";
                conditions.push_back(std::move(lnc));
                epsilons.push_back(numeric_task.get_epsilon(i));
                prop.name =  name.str();
                propositions[var_id].push_back(prop);
                ++num_propositions;
                //cout << "adding numeric precondition " << num_values << " : " << num_propositions << " " << var_id << endl;
            }

            linear_effect_to_conditions_plus = std::vector<std::vector<std::vector<int>>>(ops.size());
            linear_effect_to_conditions_minus = std::vector<std::vector<std::vector<int>>>(ops.size());
            condition_to_op_id = std::vector<int>(conditions.size(), -1);

            for (OperatorProxy op : ops)
                add_linear_conditions(op);
        }

        size_t n_numeric_variables = numeric_task.get_n_numeric_variables();

        if (use_bounds) {
            auto start = utils::g_timer();
            std::vector<double> numeric_state(n_numeric_variables);
            auto initial_state = task_proxy.get_initial_state();

            for (size_t var_id = 0; var_id < n_numeric_variables; ++var_id) {
              auto id = numeric_task.get_numeric_variable(var_id).id_abstract_task;
              numeric_state[var_id] = initial_state.nval(id);
            }

            numeric_bound.calculate_bounds(numeric_state, bound_iterations);
            if (true) numeric_bound.dump(task_proxy);
            auto end = utils::g_timer();
            cout << "Extracting bounds takes " << end - start << endl;
        }

        op_base_cost = std::vector<ap_float>(ops.size() + axioms.size(), 0.0);

        for (OperatorProxy op : ops)
            op_base_cost[op.get_id()] = calculate_base_operator_cost(op.get_id());

        // Build relaxed operators for operators and axioms.
        for (OperatorProxy op : ops)
            build_relaxed_operator(op, op.get_id());

        for (OperatorProxy op : axioms) {
            build_relaxed_operator(op, ops.size() + op.get_id());
        }

        if (!ignore_numeric) {
            has_sose = std::vector<std::vector<bool>>(ops.size(), std::vector<bool>(conditions.size(), false));
            operator_condition_to_composite_coefficients = std::vector<std::vector<std::vector<ap_float>>>(
                ops.size(), std::vector<std::vector<ap_float>>(conditions.size(), std::vector<ap_float>(n_numeric_variables, 0.0))
            );

            if (use_bounds) {
                operator_condition_to_has_upper_bound = std::vector<std::vector<bool>>(
                    ops.size(), std::vector<bool>(conditions.size(), false));
                operator_condition_to_upper_bound = std::vector<std::vector<ap_float>>(
                    ops.size(), std::vector<ap_float>(conditions.size(), std::numeric_limits<ap_float>::max()));
            }

            for (OperatorProxy op : ops)
                build_linear_operators(task_proxy, op);

            if (n_second_order_simple_operators == 0)
                this->use_second_order_simple = false;

            build_simple_effects();
            delete_noops();

            std::cout << "Infinite operators: " << n_infinite_operators << std::endl;
            std::cout << "Second-order simple operators: " << n_second_order_simple_operators << std::endl;
        }

        size_t op_size = task_proxy.get_operators().size() + task_proxy.get_axioms().size();
        original_to_relaxed_operators.resize(op_size, vector<RelaxedOperator*>());
        
        // Simplify relaxed operators.
        // simplify();
        /* TODO: Put this back in and test if it makes sense,
         but only after trying out whether and how much the change to
         unary operators hurts. */
        
        // Build artificial goal proposition and operator.
        vector<RelaxedProposition *> goal_op_pre, goal_op_eff;
        for (FactProxy goal : task_proxy.get_goals()) {
            if(!numeric_task.is_numeric_axiom(goal.get_variable().get_id())){
                goal_op_pre.push_back(get_proposition(goal));
            }
        }

        if (!ignore_numeric_conditions) {
            // add numeric goal conditions
            for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
                for (pair<int, int> var_value: numeric_task.get_propositional_goals(id_goal)) {
                    FactProxy goal = task_proxy.get_variables()[var_value.first].get_fact(var_value.second);
                    goal_op_pre.push_back(get_proposition(goal));
                }
                for (int id_n_con : numeric_task.get_numeric_goals(id_goal)) {
                    //LinearNumericCondition &num_values = numeric_task.get_condition(id_n_con);
                    goal_op_pre.push_back(get_proposition(id_n_con));
                    //cout << "Goal : " << num_values << " is a goal condition" << endl;
                }
            }
        }
        
        goal_op_eff.push_back(&artificial_goal);
        /* Use the invalid operator id -1 so accessing
         the artificial operator will generate an error. */
        string name_goal = "goal";
        RelaxedOperator goal_op(move(goal_op_pre), move(goal_op_eff), -1, 0, name_goal, false);
        if (goal_op.preconditions.empty()) goal_op.preconditions.push_back(&artificial_precondition);
        relaxed_operators.push_back(goal_op);

        for (RelaxedOperator &relaxed_op : relaxed_operators) {
            if (relaxed_op.original_op_id_1 != -1)
                original_to_relaxed_operators[relaxed_op.original_op_id_1].push_back(&relaxed_op);
            if (relaxed_op.original_op_id_2 != -1)
                original_to_relaxed_operators[relaxed_op.original_op_id_2].push_back(&relaxed_op);
        }

        // Cross-reference relaxed operators.
        for (RelaxedOperator &op : relaxed_operators) {
            for (RelaxedProposition *pre : op.preconditions){
                pre->precondition_of.push_back(&op);
            }
            for (RelaxedProposition *eff : op.effects)
                eff->effect_of.push_back(&op);
        }
        std::cout << "ops " <<  op_size << ", prop: " << num_propositions << ", numeric conditions " <<  conditions.size() << endl;
    }
    
    LandmarkCutLandmarks::~LandmarkCutLandmarks() {
    }


    std::vector<RelaxedProposition*> LandmarkCutLandmarks::build_precondition(const OperatorProxy &op, size_t op_id) {
        vector<RelaxedProposition *> precondition;
        unordered_set<RelaxedProposition *> added_precondition;
        for (FactProxy pre : op.get_preconditions()) {
            if(!numeric_task.is_numeric_axiom(pre.get_variable().get_id())) {
                RelaxedProposition *prop = get_proposition(pre);
                if (added_precondition.find(prop) == added_precondition.end()){
                    precondition.push_back(prop);
                    added_precondition.insert(prop);
                    //cout << "adding precondition " << get_proposition(pre)->name << " to action " << op.get_name() << endl;
                }
            }
        }

        if (!ignore_numeric_conditions) {
            // numeric precondition
            for (int pre : numeric_task.get_action_num_list(op_id)){
                for (int i : numeric_task.get_numeric_conditions_id(pre)){
                    RelaxedProposition *prop = get_proposition(i);
                    if (added_precondition.find(prop) == added_precondition.end()){
                        precondition.push_back(prop);
                        added_precondition.insert(prop);
                        //cout << "adding precondition " << get_proposition(i)->name << " to action " << op.get_name() << endl;
                    }
                }
            }
        }
        
        return precondition;
    }

    ap_float LandmarkCutLandmarks::calculate_base_operator_cost(size_t op_id) const {
        ap_float op_cost = numeric_task.get_action_cost(op_id);

        if (numeric_task.is_action_linear_cost(op_id) && use_bounds) {
            size_t n_numeric_variables = numeric_task.get_n_numeric_variables();
            auto coefficients = numeric_task.get_action_cost_coefficients(op_id);
            op_cost = numeric_task.get_action_cost_constant(op_id);

            for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
                ap_float w = coefficients[num_id];
                if (w >= precision && numeric_bound.get_variable_before_action_has_lb(num_id, op_id)) {
                    op_cost += w * numeric_bound.get_variable_before_action_lb(num_id, op_id);
                } else if (w <= -precision && numeric_bound.get_variable_before_action_has_ub(num_id, op_id)) {
                    op_cost += w * numeric_bound.get_variable_before_action_ub(num_id, op_id);
                } else if (fabs(w) >= precision) {
                    op_cost = 0.0;
                    break;
                }
            }
        }

        return std::max(op_cost, 0.0);
    }
    
    void LandmarkCutLandmarks::build_relaxed_operator(const OperatorProxy &op, size_t op_id) {
        auto precondition = build_precondition(op, op_id);
        vector<RelaxedProposition *> effects;

        for (EffectProxy eff : op.get_effects()) {
            if (eff.get_conditions().size() > 0) continue;
            // check if it's numeric axiom
            if(!numeric_task.is_numeric_axiom(eff.get_fact().get_variable().get_id())){
                effects.push_back(get_proposition(eff.get_fact()));
            }
        }

        string name = op.get_name();
        ap_float op_cost = op_base_cost[op_id];

        for (int i = 0; i < numeric_task.get_action_n_conditional_eff(op_id); ++i) {
            std::vector<RelaxedProposition *> conditional_effects;
            int e = numeric_task.get_action_conditional_add_list(op_id)[i];
            std::pair<int, int> e_var_value = numeric_task.get_var_val(e);
            conditional_effects.push_back(&propositions[e_var_value.first][e_var_value.second]);

            std::vector<RelaxedProposition *> extended_precondition = precondition;
            unordered_set<RelaxedProposition *> added_extended_precondition;
            added_extended_precondition.insert(precondition.begin(), precondition.end());

            for (int c : numeric_task.get_action_eff_conditions(op_id)[i]) {
                std::pair<int, int> c_var_value = numeric_task.get_var_val(c);
                RelaxedProposition *prop = &propositions[c_var_value.first][c_var_value.second];
                if (added_extended_precondition.find(prop) == added_extended_precondition.end()) {
                    extended_precondition.push_back(prop);
                    added_extended_precondition.insert(prop);
                }
            }

            if (!ignore_numeric_conditions) {
                for (int c : numeric_task.get_action_eff_num_conditions(op_id)[i]) {
                    for (int j : numeric_task.get_numeric_conditions_id(c)){
                        RelaxedProposition *prop = get_proposition(j);
                        if (added_extended_precondition.find(prop) == added_extended_precondition.end()) {
                            extended_precondition.push_back(prop);
                            added_extended_precondition.insert(prop);
                        }
                    }
                }
            }

            string conditional_name = name + " " + conditional_effects[0]->name;
            RelaxedOperator conditional_op(move(extended_precondition), move(conditional_effects), op_id, op_cost, conditional_name, true);
            relaxed_operators.push_back(conditional_op);
        }

        RelaxedOperator relaxed_op(move(precondition), move(effects), op_id, op_cost, name, false);
        if (relaxed_op.preconditions.empty()) relaxed_op.preconditions.push_back(&artificial_precondition);
        relaxed_operators.push_back(relaxed_op);
    }

    size_t LandmarkCutLandmarks::add_numeric_condition(LinearNumericCondition lnc) {
        size_t prop_id = conditions.size();
        size_t var_id = propositions.size();
        RelaxedProposition new_prop;
        new_prop.is_numeric_condition = true;
        new_prop.id_numeric_condition = prop_id;
        stringstream prop_name;
        prop_name << "numeric (" << lnc << ")";
        new_prop.name = prop_name.str();
        propositions.push_back(std::vector<RelaxedProposition>());
        propositions[var_id].push_back(new_prop);
        ++num_propositions;
        conditions.push_back(lnc);

        if (lnc.is_strictly_greater)
            epsilons.push_back(epsilon);
        else
            epsilons.push_back(0);

        return prop_id;
    }

    void LandmarkCutLandmarks::add_linear_conditions(const OperatorProxy &op) {
        int op_id = op.get_id();
        auto preconditions = numeric_task.get_action_num_list(op_id);
        size_t n_linear_eff = numeric_task.get_action_n_linear_eff(op_id);
        linear_effect_to_conditions_plus[op_id].resize(n_linear_eff);
        linear_effect_to_conditions_minus[op_id].resize(n_linear_eff);

        for (size_t i = 0; i < n_linear_eff; ++i) {
            int lhs = numeric_task.get_action_linear_lhs(op_id)[i];
            auto coefficients_plus = numeric_task.get_action_linear_coefficients(op_id)[i];
            coefficients_plus[lhs] -= 1.0;
            LinearNumericCondition lnc_plus(coefficients_plus, 0);
            lnc_plus.is_strictly_greater = true;
            size_t lnc_plus_id = add_numeric_condition(lnc_plus);
            condition_to_op_id.push_back(op_id);
            linear_effect_to_conditions_plus[op_id][i].push_back(lnc_plus_id);

            std::vector<ap_float> coefficients_minus(coefficients_plus);

            for (auto &c : coefficients_minus)
                c *= -1.0;

            LinearNumericCondition lnc_minus(coefficients_minus, 0);
            lnc_minus.is_strictly_greater = true;
            size_t lnc_minus_id = add_numeric_condition(lnc_minus);
            condition_to_op_id.push_back(op_id);
            linear_effect_to_conditions_minus[op_id][i].push_back(lnc_minus_id);

            if (numeric_task.redundant_constraints) {
                auto extended_preconditions = numeric_task.get_action_linear_eff_num_conditions(op_id)[i];
                extended_preconditions.insert(preconditions.begin(), preconditions.end());
                auto redundant_plus = make_redundant_conditions(lnc_plus, extended_preconditions);
                auto redundant_minus = make_redundant_conditions(lnc_minus, extended_preconditions);

                for (auto lnc : redundant_plus) {
                    size_t lnc_id = add_numeric_condition(lnc);
                    condition_to_op_id.push_back(op_id);
                    linear_effect_to_conditions_plus[op_id][i].push_back(lnc_id);
                }

                for (auto lnc : redundant_minus) {
                    size_t lnc_id = add_numeric_condition(lnc);
                    condition_to_op_id.push_back(op_id);
                    linear_effect_to_conditions_minus[op_id][i].push_back(lnc_id);
                }
            }
        }
    }

    std::vector<LinearNumericCondition> LandmarkCutLandmarks::make_redundant_conditions(const LinearNumericCondition &lnc, const std::set<int> &condition_ids) const {
        std::vector<LinearNumericCondition> redundant_conditions;

        for (auto lnc_id : condition_ids) {
            auto other = conditions[lnc_id];
            auto new_lnc = lnc + other;
            new_lnc.is_strictly_greater = lnc.is_strictly_greater || other.is_strictly_greater;
            redundant_conditions.push_back(new_lnc);
        }

        return redundant_conditions;
    }

    bool LandmarkCutLandmarks::has_effect(int op_id, const std::vector<ap_float> &coefficients) const {
        if (has_linear_effect(op_id, coefficients, false, false)) return true;

        // constant assignment effect
        if (use_constant_assignment && has_constant_assignment_effect(op_id, coefficients, false))
            return true;

        auto result = calculate_simple_effect_constant(op_id, coefficients, false);

        return result.first;
    }

    bool LandmarkCutLandmarks::has_constant_assignment_effect(int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear) const {
        size_t n_numeric_variables = numeric_task.get_n_numeric_variables();

        for (size_t n_id = 0; n_id < n_numeric_variables; ++n_id) {
            ap_float w = coefficients[n_id];

            // constant assignment effect
            if (fabs(w) >= precision
                && numeric_task.get_action_is_assignment(op_id)[n_id]
                && (!use_bounds || w < precision || !numeric_bound.has_no_increasing_assignment_effect(op_id, n_id))
                && (!use_bounds || w > -precision || !numeric_bound.has_no_decreasing_assignment_effect(op_id, n_id))) {
                return true;
            }
        }

        for (auto var_value : numeric_task.get_action_conditional_assign_list(op_id)) {
            size_t n_id = var_value.first;
            ap_float w = coefficients[n_id];

            // conditional constant assignment effect
            if (fabs(w) >= precision
                && (!use_bounds || w < precision || !numeric_bound.has_no_increasing_assignment_effect(op_id, n_id))
                && (!use_bounds || w > -precision || !numeric_bound.has_no_decreasing_assignment_effect(op_id, n_id))) {
                return true;
            }
        }

        if (use_bounded_linear) {
            for (auto n_id : numeric_task.get_action_linear_lhs(op_id)) {
                ap_float w = coefficients[n_id];

                // linear effect
                if ((w >= precision
                     && numeric_bound.get_assignment_has_ub(op_id, n_id)
                     && (!use_bounds || !numeric_bound.has_no_increasing_assignment_effect(op_id, n_id)))
                    || (w <= -precision
                        && numeric_bound.get_assignment_has_lb(op_id, n_id)
                        && (!use_bounds || !numeric_bound.has_no_decreasing_assignment_effect(op_id, n_id)))) {
                    return true;
                }
            }
        }

        return false;
    }

    bool LandmarkCutLandmarks::has_linear_effect(int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear, bool only_conditional) const {
        for (size_t i = 0; i < numeric_task.get_action_n_linear_eff(op_id); ++i) {
            int n_id = numeric_task.get_action_linear_lhs(op_id)[i];
            ap_float w = coefficients[n_id];
            // linear effect
            if (((!use_bounded_linear && fabs(w) >= precision)
                  || (w >= precision && (!numeric_bound.get_effect_has_ub(op_id, n_id)
                                          && (!use_constant_assignment || !numeric_bound.get_assignment_has_ub(op_id, n_id))))
                  || (w <= -precision && (!numeric_bound.get_effect_has_lb(op_id, n_id)
                                          && (!use_constant_assignment || !numeric_bound.get_assignment_has_lb(op_id, n_id)))))
                && (!only_conditional || numeric_task.get_action_linear_is_conditional(op_id, i))) {
                return true;
            }
        }

        return false;
    }

    std::vector<ap_float> LandmarkCutLandmarks::calculate_composite_coefficients(int op_id, const LinearNumericCondition &lnc) const {
        size_t n_numeric_variables = numeric_task.get_n_numeric_variables();
        std::vector<ap_float> new_coefficients(n_numeric_variables, 0.0);

        for (size_t i = 0; i < numeric_task.get_action_n_linear_eff(op_id); ++i) {
            int lhs = numeric_task.get_action_linear_lhs(op_id)[i];
            ap_float w = lnc.coefficients[lhs];
            auto &effect_coefficients = numeric_task.get_action_linear_coefficients(op_id)[i];

            for (size_t n_id = 0; n_id < n_numeric_variables; ++n_id) {
                ap_float k = effect_coefficients[n_id];
                if (static_cast<size_t>(lhs) == n_id) k -= 1.0;
                new_coefficients[n_id] += w * k;
            }
        }

        return new_coefficients;
    }

    std::pair<bool, ap_float> LandmarkCutLandmarks::calculate_simple_effect_constant(int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear) const {
        bool has_simple_effect = false;

        auto eff_list = numeric_task.get_action_eff_list(op_id);
        size_t n_numeric_variables = numeric_task.get_n_numeric_variables();
        ap_float net = 0.0;

        for (size_t n_id = 0; n_id < n_numeric_variables; ++n_id) {
            net += coefficients[n_id] * eff_list[n_id]; 
        }

        for (auto var_value : numeric_task.get_action_conditional_eff_list(op_id)) {
            ap_float e = coefficients[var_value.first] * var_value.second;
            if (e >= precision) net += e;
        }

        for (size_t i = 0; i < numeric_task.get_action_n_linear_eff(op_id); ++i) {
            int n_id = numeric_task.get_action_linear_lhs(op_id)[i];
            bool conditional = numeric_task.get_action_linear_is_conditional(op_id, i);
            ap_float w = coefficients[n_id];
            ap_float e = w * numeric_task.get_action_linear_constants(op_id)[i];

            // bounded linear effect
            if (use_bounded_linear
                && w >= precision
                && numeric_bound.get_effect_has_ub(op_id, n_id)
                && (!use_constant_assignment || !numeric_bound.get_assignment_has_ub(op_id, n_id))) {
                e = w * numeric_bound.get_effect_ub(op_id, n_id);
            } else if (use_bounded_linear
                       && w <= -precision
                       && numeric_bound.get_effect_has_lb(op_id, n_id)
                       && (!use_constant_assignment || !numeric_bound.get_assignment_has_lb(op_id, n_id))) {
                e = w * numeric_bound.get_effect_lb(op_id, n_id);
            } else if (use_bounded_linear
                       && use_constant_assignment
                       && ((w >= precision && numeric_bound.get_assignment_has_ub(op_id, n_id))
                           || (w <= -precision && numeric_bound.get_assignment_has_lb(op_id, n_id)))) {
                e = 0.0;
                has_simple_effect = true;
            }

            if (!conditional || e >= precision) net += e;
        }

        if (!has_simple_effect) has_simple_effect = net >= precision;

        return std::make_pair(has_simple_effect, net);
    }

    std::pair<bool, std::vector<std::pair<int, ap_float>>> LandmarkCutLandmarks::get_sose_supporters(const TaskProxy &task_proxy, int op_id,
                                                                                                     const LinearNumericCondition &lnc) const {
        std::vector<std::pair<int, ap_float>> supporters;

        // check if SOSE is used, it has a linear effect, and it is not conditional
        if (!use_second_order_simple
            || !has_linear_effect(op_id, lnc.coefficients, false, false)
            || has_linear_effect(op_id, lnc.coefficients, false, true))
            return std::make_pair(false, supporters);

        auto composite_coefficients = calculate_composite_coefficients(op_id, lnc);
        // check self loop
        if (has_effect(op_id, composite_coefficients)) {
            return std::make_pair(false, supporters);
        }

        for (auto op_1 : task_proxy.get_operators()) {
            int op_1_id = op_1.get_id();

            // check if linear supporter
            if (has_linear_effect(op_1_id, composite_coefficients, use_bounds, false)) {
                supporters.clear();
                return std::make_pair(false, supporters);
            }

            // check if simple supporter
            auto result = calculate_simple_effect_constant(op_1_id, composite_coefficients, use_bounds);

            if (result.first || has_constant_assignment_effect(op_1_id, composite_coefficients, use_bounds)) {
                // check parallel effect
                if (has_effect(op_1_id, lnc.coefficients)) {
                    supporters.clear();
                    return std::make_pair(false, supporters);
                };

                supporters.push_back(std::make_pair(op_1_id, result.second));
            }
        }

        return std::make_pair(true, supporters);
    }

    void LandmarkCutLandmarks::build_linear_operators(const TaskProxy &task_proxy, const OperatorProxy &op) {
        int op_id = op.get_id();
        size_t n_linear_eff = numeric_task.get_action_n_linear_eff(op_id);
        if (n_linear_eff == 0) return;

        auto precondition = build_precondition(op, op_id);
        std::unordered_set<RelaxedProposition *> added_precondition;
        added_precondition.insert(precondition.begin(), precondition.end());

        std::vector<RelaxedOperator> infinite_plus_operators;
        std::vector<RelaxedOperator> infinite_minus_operators;
        string op_name = op.get_name();
        ap_float op_cost = op_base_cost[op_id];

        for (size_t i = 0; i < n_linear_eff; ++i) {
            vector<RelaxedProposition *> extended_precondition = precondition;
            unordered_set<RelaxedProposition *> added_extended_precondition = added_precondition;

            for (int c : numeric_task.get_action_linear_eff_conditions(op_id)[i]) {
                std::pair<int, int> c_var_value = numeric_task.get_var_val(c);
                RelaxedProposition *prop = &propositions[c_var_value.first][c_var_value.second];
                if (added_extended_precondition.find(prop) == added_extended_precondition.end()) {
                    extended_precondition.push_back(prop);
                    added_extended_precondition.insert(prop);
                }
            }

            for (int c : numeric_task.get_action_linear_eff_num_conditions(op_id)[i]) {
                for (int j : numeric_task.get_numeric_conditions_id(c)){
                    RelaxedProposition *prop = get_proposition(j);
                    if (added_extended_precondition.find(prop) == added_extended_precondition.end()) {
                        extended_precondition.push_back(prop);
                        added_extended_precondition.insert(prop);
                    }
                }
            }

            vector<RelaxedProposition *> extended_precondition_plus = extended_precondition;

            for (int j : linear_effect_to_conditions_plus[op_id][i]) {
                RelaxedProposition *prop = get_proposition(j);
                extended_precondition_plus.push_back(prop);
            }

            vector<RelaxedProposition *> extended_precondition_minus = extended_precondition;

            for (int j : linear_effect_to_conditions_minus[op_id][i]) {
                RelaxedProposition *prop = get_proposition(j);
                extended_precondition_minus.push_back(prop);
            }

            int lhs = numeric_task.get_action_linear_lhs(op_id)[i];

            string name_plus = op_name + " " + std::to_string(lhs) + " +inf";
            infinite_plus_operators.emplace_back(RelaxedOperator(move(extended_precondition_plus), op_id, op_cost, name_plus, true, true));

            string name_minus = op_name + " " + std::to_string(lhs) + " -inf";
            infinite_minus_operators.emplace_back(RelaxedOperator(move(extended_precondition_minus), op_id, op_cost, name_minus, true, true));
        }

        size_t n_operators = task_proxy.get_operators().size();
        std::vector<std::vector<int>> op_1_to_lnc_ids(n_operators, std::vector<int>());
        std::vector<std::vector<ap_float>> op_1_to_sose_constants(n_operators, std::vector<ap_float>(conditions.size(), 0.0));
        std::set<int> op_1_ids;

        for (size_t lnc_id = 0; lnc_id < conditions.size(); ++lnc_id) {
            auto lnc = conditions[lnc_id];
            auto result = get_sose_supporters(task_proxy, op_id, lnc);

            if (result.first) {
                // SOSE
                has_sose[op_id][lnc_id] = true;
                auto coefficients = calculate_composite_coefficients(op_id, lnc);
                operator_condition_to_composite_coefficients[op_id][lnc_id] = coefficients;

                if (use_bounds) {
                    bool has_bound = true;
                    ap_float ub = 0.0;

                    for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id) {
                        ap_float w = coefficients[n_id];

                        if (w >= precision && numeric_bound.get_variable_before_action_has_ub(n_id, op_id)) {
                            ub += w * numeric_bound.get_variable_before_action_ub(n_id, op_id);
                        } else if (w <= -precision && numeric_bound.get_variable_before_action_has_lb(n_id, op_id)) {
                            ub += w * numeric_bound.get_variable_before_action_lb(n_id, op_id);
                        } else if (fabs(w) >= precision) {
                            has_bound = false;
                            break;
                        }
                    }

                    if (has_bound) {
                        operator_condition_to_has_upper_bound[op_id][lnc_id] = true;
                        operator_condition_to_upper_bound[op_id][lnc_id] = ub;
                    }
                }

                for (auto id_effect : result.second) {
                    size_t op_1_id = id_effect.first;
                    op_1_to_lnc_ids[op_1_id].push_back(lnc_id);
                    op_1_to_sose_constants[op_1_id][lnc_id] = id_effect.second;
                    op_1_ids.insert(op_1_id);
                }
            } else if (has_linear_effect(op_id, lnc.coefficients, use_bounds, false)) {
                // non-SOSE
                for (size_t i = 0; i < n_linear_eff; ++i) {
                    int lhs = numeric_task.get_action_linear_lhs(op_id)[i];

                    if (lnc.coefficients[lhs] >= precision) {
                        infinite_plus_operators[i].effects.push_back(get_proposition(lnc_id));
                    } else if (lnc.coefficients[lhs] <= -precision) {
                        infinite_minus_operators[i].effects.push_back(get_proposition(lnc_id));
                    }
                }
            }
        }

        // add infinite operators
        for (auto infinite_op : infinite_plus_operators) {
            if (infinite_op.effects.size() > 0) {
                relaxed_operators.push_back(infinite_op);
                ++n_infinite_operators;
            }
        }

        for (auto infinite_op : infinite_minus_operators) {
            if (infinite_op.effects.size() > 0) {
                relaxed_operators.push_back(infinite_op);
                ++n_infinite_operators;
            }
        }

        // add SOSE operators
        for (int op_1_id : op_1_ids) {
            auto op_1 = task_proxy.get_operators()[op_1_id];
            auto precondition_1 = build_precondition(op_1, op_1_id);
            auto sose_constants = op_1_to_sose_constants[op_1_id];
            ap_float op_1_cost = op_base_cost[op_1_id];
            string op_1_name = op_1.get_name();

            std::vector<RelaxedProposition*> eff;

            for (size_t lnc_id : op_1_to_lnc_ids[op_1_id])
                eff.push_back(get_proposition(lnc_id));

            RelaxedOperator sose_op(move(precondition_1), precondition, move(eff), move(sose_constants),
                                    op_1_id, op_id, op_1_cost, op_cost, op_1_name, op_name);

            if (sose_op.preconditions.empty())
                sose_op.preconditions.push_back(&artificial_precondition);

            relaxed_operators.push_back(sose_op);
            ++n_second_order_simple_operators;
        }
    }

    void LandmarkCutLandmarks::build_simple_effects() {
        operator_to_simple_effects = std::vector<std::vector<ap_float>>(numeric_task.get_n_actions(),
                                                                        std::vector<ap_float>(conditions.size(), 0.0));

        for (auto &relaxed_op : relaxed_operators) {
            int op_id_1 = relaxed_op.original_op_id_1;
            int op_id_2 = relaxed_op.original_op_id_2;
            if (!relaxed_op.conditional && op_id_1 == -1 && static_cast<size_t>(op_id_2) < numeric_task.get_n_actions()) {
                for (size_t i = 0; i < conditions.size(); ++i){
                    LinearNumericCondition& lnc = conditions[i];
                    auto result = calculate_simple_effect_constant(op_id_2, lnc.coefficients, use_bounds && !has_sose[op_id_2][i]);

                    // operator with simple effects
                    if (result.first
                        || has_sose[op_id_2][i]
                        || has_constant_assignment_effect(op_id_2, lnc.coefficients, use_bounds && !has_sose[op_id_2][i])) {
                        operator_to_simple_effects[op_id_2][i] = result.second;
                        relaxed_op.effects.push_back(get_proposition(i));
                    } 
                }
            }
        }
    }

    void LandmarkCutLandmarks::delete_noops() {
        for (auto itr = relaxed_operators.begin(); itr != relaxed_operators.end();) {
            if (itr->effects.empty()) {
                itr = relaxed_operators.erase(itr);
            } else {
                ++itr;
            }
        }
    }

    RelaxedProposition *LandmarkCutLandmarks::get_proposition(
                                                              const FactProxy &fact) {
        int var_id = fact.get_variable().get_id();
        int val = fact.get_value();
        return &propositions[var_id][val];
    }
    
    RelaxedProposition *LandmarkCutLandmarks::get_proposition(
                                                              const int &n_condition) {
        int propositions_size = propositions.size();
        if (propositions_size <= n_condition + n_var) std::cout << "wrong vector size " << propositions.size() << " " << n_var << " " << n_condition << endl;
        if (propositions[n_condition + n_var].size() < 1) std::cout << "no proposition " << endl;
        return &propositions[n_condition + n_var][0];
    }
    
    // heuristic computation
    void LandmarkCutLandmarks::setup_exploration_queue() {
        priority_queue.clear();
        
        for (auto &var_props : propositions) {
            for (RelaxedProposition &prop : var_props) {
                prop.status = UNREACHED;
                prop.explored = false;
            }
        }
        
        artificial_goal.status = UNREACHED;
        artificial_precondition.status = UNREACHED;
        artificial_goal.explored = false;
        artificial_precondition.explored = false;
        
        for (RelaxedOperator &op : relaxed_operators) {
            op.unsatisfied_preconditions = op.preconditions.size();
            op.h_max_supporter = 0;
            op.h_max_supporter_cost = numeric_limits<int>::max();
        }
    }
    
    void LandmarkCutLandmarks::setup_exploration_queue_state(const State &state) {
        // propositions
        for (FactProxy init_fact : state) {
            if (numeric_task.is_numeric_axiom(init_fact.get_variable().get_id())) continue;
            enqueue_if_necessary(get_proposition(init_fact), 0);
            if (debug) std::cout << "initial state: " << get_proposition(init_fact)->name << endl;
        }
        numeric_initial_state.assign(conditions.size(),0);
        
        if (!ignore_numeric_conditions) {
            // numeric_conditions
            for (size_t i = 0; i < conditions.size(); ++i){
                LinearNumericCondition& lnc = conditions[i];
                ap_float net = lnc.constant - epsilons[i];
                for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
                    int id_num = numeric_task.get_numeric_variable(n_id).id_abstract_task;
                    net += lnc.coefficients[n_id]*state.nval(id_num);
                    //cout << n_id << " " << state.nval(n_id) << " " << lnc.coefficients[n_id] << endl;
                }
                //cout << lnc << " evaluated in the initial state " << net << endl;
                numeric_initial_state[i] = -net;
                if (net > -precision) {
                        if (debug) std::cout << lnc << " epsilon " << epsilons[i] << " is satisfied in initial state " << net << endl;
                    enqueue_if_necessary(get_proposition(i), 0);
                } else {
                    if (debug) std::cout << lnc << " epsilon " << epsilons[i] << " not satisfied " << net << endl;
                }
            }
        }
        
        enqueue_if_necessary(&artificial_precondition, 0);
    }
    
    void LandmarkCutLandmarks::first_exploration(const State &state) {
        if (debug) std::cout << "  first exploration : " <<endl;
        assert(priority_queue.empty());
        setup_exploration_queue();
        setup_exploration_queue_state(state);

        while (!priority_queue.empty()) {
            pair<ap_float, RelaxedProposition *> top_pair = priority_queue.pop();
            ap_float popped_cost = top_pair.first;
            RelaxedProposition *prop = top_pair.second;
            ap_float prop_cost = prop->h_max_cost;
            assert(prop_cost <= popped_cost);

            if (prop_cost < popped_cost)
                continue;

            if (debug) std::cout << "\tReached " << prop->name << " with cost " << prop->h_max_cost << endl;

            prop->explored = true;
            const vector<RelaxedOperator *> &triggered_operators = prop->precondition_of;

            for (RelaxedOperator *relaxed_op : triggered_operators) {
                --relaxed_op->unsatisfied_preconditions;
                assert(relaxed_op->unsatisfied_preconditions >= 0);

                if (relaxed_op->unsatisfied_preconditions == 0) {
                    if (use_random_pcf) {
                        relaxed_op->select_random_supporter();
                    } else {
                        relaxed_op->h_max_supporter = prop;
                        relaxed_op->h_max_supporter_cost = prop_cost;
                    }

                    for (RelaxedProposition *effect : relaxed_op->effects)
                        update_queue(state, prop, effect, relaxed_op);
                } 
            }
        }
    }
    
    void LandmarkCutLandmarks::first_exploration_incremental(const State &state, vector<RelaxedOperator *> &cut) {
        assert(priority_queue.empty());
        if (debug) std::cout << "  incremental exploration : " << endl;
        for (RelaxedOperator *relaxed_op : cut) {
            if (relaxed_op->original_op_id_1 != -1) {
                for (RelaxedOperator *relaxed_op_2 : original_to_relaxed_operators[relaxed_op->original_op_id_1]) {
                    if (relaxed_op_2->unsatisfied_preconditions == 0) {
                        for (RelaxedProposition *effect : relaxed_op_2->effects)
                            update_queue(state, relaxed_op_2->h_max_supporter, effect, relaxed_op_2);
                    }
                }
            }
            for (RelaxedOperator *relaxed_op_2 : original_to_relaxed_operators[relaxed_op->original_op_id_2]) {
                if (relaxed_op_2->unsatisfied_preconditions == 0) {
                    for (RelaxedProposition *effect : relaxed_op_2->effects)
                        update_queue(state, relaxed_op_2->h_max_supporter, effect, relaxed_op_2);
                }
            }
        }
        if (debug) std::cout << "  pushed operators in the cut" << endl;
        while (!priority_queue.empty()) {
            pair<ap_float, RelaxedProposition *> top_pair = priority_queue.pop();
            ap_float popped_cost = top_pair.first;
            RelaxedProposition *prop = top_pair.second;
            ap_float prop_cost = prop->h_max_cost;
            assert(prop_cost <= popped_cost);
            if (prop_cost < popped_cost)
                continue;
            const vector<RelaxedOperator *> &triggered_operators = prop->precondition_of;

            for (RelaxedOperator *relaxed_op : triggered_operators) {
                if (relaxed_op->h_max_supporter == prop) {
                    ap_float old_supp_cost = relaxed_op->h_max_supporter_cost;
                    if (old_supp_cost > prop_cost) {
                        if (use_random_pcf)
                            relaxed_op->select_random_supporter();
                        else
                            relaxed_op->update_h_max_supporter();
                        ap_float new_supp_cost = relaxed_op->h_max_supporter_cost;
                        if (new_supp_cost != old_supp_cost) {
                            if (debug) std::cout << "\t  " << prop->name <<" "<< relaxed_op->name << " " << new_supp_cost << " " << old_supp_cost << " " << prop_cost << endl;
                            for (RelaxedProposition *effect : relaxed_op->effects)
                                update_queue(state, relaxed_op->h_max_supporter, effect, relaxed_op);
                        }
                    }
                }
            }
        }
    }
    
    void LandmarkCutLandmarks::second_exploration(const State &state, vector<RelaxedProposition *> &second_exploration_queue,
                                                  vector<RelaxedOperator *> &cut, vector<pair<ap_float, ap_float>> &m_list) {
        assert(second_exploration_queue.empty());
        assert(cut.empty());
        if (debug) std::cout << "  second exploration" << endl;
        artificial_precondition.status = BEFORE_GOAL_ZONE;
        second_exploration_queue.push_back(&artificial_precondition);
        
        for (FactProxy init_fact : state) {
            if (numeric_task.is_numeric_axiom(init_fact.get_variable().get_id())) continue;
            RelaxedProposition *init_prop = get_proposition(init_fact);
            init_prop->status = BEFORE_GOAL_ZONE;
            if (debug) std::cout << "\t\t  adding " << init_prop->name << " to the queue " << endl;
            second_exploration_queue.push_back(init_prop);
        }

        if (!ignore_numeric_conditions) { 
            for (size_t i = 0; i < conditions.size(); ++i) {
                if (numeric_initial_state[i] < precision) {
                    RelaxedProposition *init_prop = get_proposition(i);
                    init_prop->status = BEFORE_GOAL_ZONE;
                    if (debug) std::cout << "\t\t  adding " << init_prop->name << " to the queue " << endl;
                    second_exploration_queue.push_back(init_prop);
                }
            }
        }
        
        int n_iterations = 0;
        while (!second_exploration_queue.empty()) {
            n_iterations++;
            RelaxedProposition *prop = second_exploration_queue.back();
            second_exploration_queue.pop_back();
            const vector<RelaxedOperator *> &triggered_operators = prop->precondition_of;

            for (RelaxedOperator *relaxed_op : triggered_operators) {
                ap_float min_cut_cost = std::numeric_limits<ap_float>::max();

                if (relaxed_op->h_max_supporter == prop && std::find(cut.begin(), cut.end(), relaxed_op) == cut.end()) {
                    for (RelaxedProposition *effect : relaxed_op->effects) {
                        if (effect->status == GOAL_ZONE) {
                            std::pair<ap_float, ap_float> ms = calculate_numeric_times(state, effect, relaxed_op, !disable_ma);

                            if ((relaxed_op->original_op_id_1 != -1 && ms.first >= precision)
                                || (relaxed_op->original_op_id_1 == -1 && ms.second >= precision)) {
                                if (debug) {
                                    std::cout << "\t\t  adding " << relaxed_op->name << " to the cut with"; 

                                    if (relaxed_op->original_op_id_1 != -1)
                                        std::cout << " cost1: " << relaxed_op->cost_1;

                                    std::cout << " cost2: " << relaxed_op->cost_2;

                                    if (relaxed_op->original_op_id_1 != -1)
                                        std::cout << " m1: " << ms.first;

                                    std::cout << " m2: " << ms.second << std::endl;
                                    std::cout << "\t\t" << prop->name << " -> " << effect->name << std::endl;
                                }

                                cut.push_back(relaxed_op);
                                m_list.push_back(ms);

                                ap_float edge_cost = ms.second * relaxed_op->cost_2;
                                if (relaxed_op->original_op_id_1 != -1) edge_cost += ms.first * relaxed_op->cost_1;
                                min_cut_cost = std::min(min_cut_cost, edge_cost);
                            }
                        }
                    }

                    for (RelaxedProposition *effect : relaxed_op->effects) {
                        if (effect->status != BEFORE_GOAL_ZONE && effect->status != GOAL_ZONE) {
                            std::pair<ap_float, ap_float> ms = calculate_numeric_times(state, effect, relaxed_op, !disable_ma);

                            if ((relaxed_op->original_op_id_1 != -1 && ms.first >= precision)
                                || (relaxed_op->original_op_id_1 == -1 && ms.second >= precision)) {
                                assert(effect->status == REACHED);
                                ap_float edge_cost = ms.second * relaxed_op->cost_2;
                                if (relaxed_op->original_op_id_1 != -1) edge_cost += ms.first * relaxed_op->cost_1;

                                if (edge_cost < min_cut_cost) {
                                    effect->status = BEFORE_GOAL_ZONE;
                                    if (debug) std::cout << "\t\t  adding " << effect->name << " to the queue " << endl;
                                    second_exploration_queue.push_back(effect);
                                }
                            }
                        }
                    }
                }
            }
        }
        //cout << "\tsecond it " << n_iterations << endl;
    }
    
    void LandmarkCutLandmarks::mark_goal_plateau(const State &state, RelaxedProposition *subgoal) {
        // NOTE: subgoal can be null if we got here via recursion through
        // a zero-cost action that is relaxed unreachable. (This can only
        // happen in domains which have zero-cost actions to start with.)
        // For example, this happens in pegsol-strips #01.
        if (subgoal && subgoal->status != GOAL_ZONE) {
            subgoal->status = GOAL_ZONE;

            for (RelaxedOperator *achiever : subgoal->effect_of) {
                if (achiever->cost_1 < precision && achiever->cost_2 < precision && achiever->unsatisfied_preconditions == 0) {
                    std::pair<ap_float, ap_float> ms = calculate_numeric_times(state, subgoal, achiever, !disable_ma);

                    if ((achiever->original_op_id_1 != -1 && ms.first >= precision) || ms.second >= precision) {
                        if (debug) std::cout << "\tadding subgoal " <<  achiever->h_max_supporter->name << " from precondition of " << achiever->name << " which has effect " << subgoal->name << endl;
                        mark_goal_plateau(state, achiever->h_max_supporter);
                    }
                }
            }
        }
    }
    
    void LandmarkCutLandmarks::validate_h_max() const {
#ifndef NDEBUG
        // Using conditional compilation to avoid complaints about unused
        // variables when using NDEBUG. This whole code does nothing useful
        // when assertions are switched off anyway.
        for (const RelaxedOperator &op : relaxed_operators) {
            if (op.unsatisfied_preconditions) {
                bool reachable = true;
                for (RelaxedProposition *pre : op.preconditions) {
                    if (pre->status == UNREACHED) {
                        reachable = false;
                        break;
                    }
                }
                assert(!reachable);
                assert(!op.h_max_supporter);
            } else {
                assert(op.h_max_supporter);
                int h_max_cost = op.h_max_supporter_cost;
                assert(h_max_cost == op.h_max_supporter->h_max_cost);
                for (RelaxedProposition *pre : op.preconditions) {
                    assert(pre->status != UNREACHED);
                    assert(pre->h_max_cost <= h_max_cost);
                }
            }
        }
#endif
    }
    
    bool LandmarkCutLandmarks::compute_landmarks(State state, CostCallback cost_callback,
                                                 LandmarkCallback landmark_callback) {
        for (RelaxedOperator &op : relaxed_operators) {
            op.cost_1 = op.base_cost_1;
            op.cost_2 = op.base_cost_2;
        }
        vector<RelaxedOperator *> cut;
        vector<pair<ap_float, ap_float>> m_list;
        unordered_map<int, ap_float> operator_to_min_cut_cost;
        unordered_map<int, ap_float> operator_to_m;
        Landmark landmark;
        vector<RelaxedProposition *> second_exploration_queue;
        first_exploration(state);
        if (artificial_goal.status == UNREACHED) return true;
        int num_iterations = 0;

        while (artificial_goal.h_max_cost >= precision) {
            ++num_iterations;
            mark_goal_plateau(state, &artificial_goal);
            assert(cut.empty());
            second_exploration(state, second_exploration_queue, cut, m_list);
            assert(!cut.empty());
            ap_float cut_cost = numeric_limits<ap_float>::max();

            for (size_t i = 0; i < cut.size(); ++i) {
                ap_float current_cut_cost = m_list[i].second * cut[i]->cost_2;

                if (cut[i]->original_op_id_1 != -1 && m_list[i].first >= precision) {
                    current_cut_cost += m_list[i].first * cut[i]->cost_1;
                    auto itr = operator_to_min_cut_cost.find(cut[i]->original_op_id_1);

                    if (itr == operator_to_min_cut_cost.end() || current_cut_cost < itr->second)
                        operator_to_min_cut_cost[cut[i]->original_op_id_1] = current_cut_cost;
                }

                auto itr = operator_to_min_cut_cost.find(cut[i]->original_op_id_2);

                if (itr == operator_to_min_cut_cost.end() || current_cut_cost < itr->second)
                    operator_to_min_cut_cost[cut[i]->original_op_id_2] = current_cut_cost;

                cut_cost = std::min(cut_cost, current_cut_cost);
            }

            if (debug) std::cout << "  cut cost " << artificial_goal.h_max_cost << " " << cut_cost << endl;
            
            for (auto itr : operator_to_min_cut_cost) {
                for (RelaxedOperator *relaxed_op : original_to_relaxed_operators[itr.first]) {
                    ap_float m = itr.second;
                    if (m < precision) continue;

                    if (relaxed_op->original_op_id_1 == itr.first && relaxed_op->cost_1 >= precision) {
                        if (debug) std::cout << "\tcut " << relaxed_op->name << " cost1: " << relaxed_op->cost_1;
                        m /= relaxed_op->cost_1;
                        relaxed_op->cost_1 -= cut_cost / m;
                        if (relaxed_op->cost_1 < precision) relaxed_op->cost_1 = 0;
                        if (debug) std::cout << " -> " << relaxed_op->cost_1<< " m1: " << m << endl;
                        operator_to_m[itr.first] = m;
                    }

                    if (relaxed_op->original_op_id_2 == itr.first && relaxed_op->cost_2 >= precision) {
                        if (debug) std::cout << "\tcut " << relaxed_op->name << " cost2: " << relaxed_op->cost_2;
                        m /= relaxed_op->cost_2;
                        relaxed_op->cost_2 -= cut_cost / m;
                        if (relaxed_op->cost_2 < precision) relaxed_op->cost_2 = 0;
                        if (debug) std::cout << " -> " << relaxed_op->cost_2 << " m2: " << m << endl;
                        operator_to_m[itr.first] = m;
                    }
                }
            }

            if (cost_callback) {
                cost_callback(cut_cost);
            }

            if (debug) std::cout << "  cut cost " << cut_cost << endl;

            if (landmark_callback) {
                landmark.clear();

                for (auto itr : operator_to_m) {
                    landmark.push_back({itr.second, itr.first});
                }
                
                landmark_callback(landmark, cut_cost);
            }
            
            first_exploration_incremental(state, cut);
            // validate_h_max();  // too expensive to use even in regular debug mode
            cut.clear();
            m_list.clear();
            operator_to_m.clear();
            operator_to_min_cut_cost.clear();
            
            /*
             Note: This could perhaps be made more efficient, for example by
             using a round-dependent counter for GOAL_ZONE and BEFORE_GOAL_ZONE,
             or something based on total_cost, so that we don't need a per-round
             reinitialization.
             */
            for (auto &var_props : propositions) {
                for (RelaxedProposition &prop : var_props) {
                    if (prop.status == GOAL_ZONE || prop.status == BEFORE_GOAL_ZONE)
                        prop.status = REACHED;
                }
            }
            artificial_goal.status = REACHED;
            artificial_precondition.status = REACHED;
        }

        return false;
    }

    void LandmarkCutLandmarks::update_queue(const State &state, RelaxedProposition *prop, RelaxedProposition *effect, RelaxedOperator *relaxed_op) {
        if (effect->is_numeric_condition) {
            int id_effect = effect->id_numeric_condition;
            if (numeric_initial_state[id_effect] >= precision) {
                std::pair<ap_float, ap_float> ms = calculate_numeric_times(state, effect, relaxed_op, !use_irmax);

                if (relaxed_op->original_op_id_1 != -1 && ms.first >= precision) {
                    ap_float target_cost = prop->h_max_cost + ms.first * relaxed_op->cost_1 + ms.second * relaxed_op->cost_2;
                    bool queued = enqueue_if_necessary(effect, target_cost);
                    if (debug && queued) std::cout << "\t  " << relaxed_op->h_max_supporter->name << " -> " << effect->name <<  " : " << relaxed_op->name << " " << target_cost << endl;
                } else if (relaxed_op->original_op_id_1 == -1 && ms.second >= precision) {
                    ap_float target_cost = prop->h_max_cost + ms.second * relaxed_op->cost_2;
                    bool queued = enqueue_if_necessary(effect, target_cost);
                    if (debug && queued) std::cout << "\t  " << relaxed_op->h_max_supporter->name << " -> " << effect->name <<  " : " << relaxed_op->name << " " << target_cost << endl;
                }
            }
        } else {
            ap_float target_cost = prop->h_max_cost + relaxed_op->cost_2;
            bool queued = enqueue_if_necessary(effect, target_cost);
            if(debug && queued) std::cout << "\t  " << relaxed_op->h_max_supporter->name << " -> " << effect->name <<  " : " << relaxed_op->name << " " << target_cost << endl;
        }
    }
    
    std::pair<ap_float, ap_float> LandmarkCutLandmarks::calculate_numeric_times(const State &state, RelaxedProposition *effect,
                                                                                RelaxedOperator *relaxed_op, bool use_ma){
        if (use_ma && effect->is_numeric_condition && !relaxed_op->infinite) {
            int id_effect = effect->id_numeric_condition;

            if (relaxed_op->original_op_id_1 == -1) {
                const LinearNumericCondition &lnc = conditions[id_effect];
                int op_id = relaxed_op->original_op_id_2;
                ap_float net = operator_to_simple_effects[op_id][id_effect];

                if (has_sose[op_id][id_effect]) {
                    auto &coefficients = operator_condition_to_composite_coefficients[op_id][id_effect];
                    net += calculate_linear_expression(state, coefficients);
                }

                if (use_constant_assignment)
                    net += calculate_constant_assignment_effect(state, op_id, lnc.coefficients, use_bounds && !has_sose[op_id][id_effect]);

                // no effect
                if (net < precision) return std::make_pair(-1, -1);

                ap_float m = numeric_initial_state[id_effect] / net;

                // already satisfied
                if (m < precision) return std::make_pair(0, 0);

                if (ceiling_less_than_one) return std::make_pair(0, std::max(m, 1.0));

                return std::make_pair(0, m);
            } else {
                int op_id_1 = relaxed_op->original_op_id_1;
                int op_id_2 = relaxed_op->original_op_id_2;

                ap_float c_u = relaxed_op->sose_constants[id_effect];
                auto &coefficients = operator_condition_to_composite_coefficients[op_id_2][id_effect];

                if (use_constant_assignment)
                    c_u += calculate_constant_assignment_effect(state, op_id_1, coefficients, use_bounds);
                
                // not simple supporter
                if (c_u < precision) return std::make_pair(-1, -1);

                // zero cost supporter
                if (relaxed_op->cost_1 < precision) return std::make_pair(1, 1);

                ap_float c = operator_to_simple_effects[op_id_2][id_effect];
                ap_float s_u = calculate_linear_expression(state, coefficients);

                // zero cost SOSE
                if (relaxed_op->cost_2 < precision) {
                    // zero -> apply simple supporter once
                    if (std::fabs(c + s_u) < precision) {
                        return std::make_pair(1, 1);
                    // positive -> no simple supporter
                    } else if (c + s_u > 0) {
                        return std::make_pair(-1, -1);
                    // negative -> apply simple supporter
                    } else {
                        ap_float m_1 = -(c + s_u) / c_u;

                        if (ceiling_less_than_one)
                            return std::make_pair(std::max(m_1, 1.0), 1);
                        else
                            return std::make_pair(m_1, 1);
                    }
                }

                ap_float u_target = sqrt(numeric_initial_state[id_effect] * c_u * relaxed_op->cost_2 / relaxed_op->cost_1) - c;

                if (use_bounds && operator_condition_to_has_upper_bound[op_id_2][id_effect])
                    u_target = std::min(u_target, operator_condition_to_upper_bound[op_id_2][id_effect]);

                // no need to use simple effects or the linear effect is non-positive
                if (u_target - s_u < precision || c + u_target < precision) return std::make_pair(-1, -1);

                ap_float m_1 = (u_target - s_u) / c_u;
                ap_float m_2 = numeric_initial_state[id_effect] / (c + u_target);

                if (ceiling_less_than_one) {
                    m_1 = std::max(m_1, 1.0);
                    m_2 = std::max(m_2, 1.0);
                }

                return std::make_pair(m_1, m_2);
            }
        }

        return std::make_pair(0, 1);
    }

    ap_float LandmarkCutLandmarks::calculate_constant_assignment_effect(const State &state, int op_id, const std::vector<ap_float> &coefficients,
                                                                        bool use_bounded_linear) const {
        ap_float net = 0.0;
        size_t n_numeric_variables = numeric_task.get_n_numeric_variables();

        // constant assignment effect
        for (size_t n_id = 0; n_id < n_numeric_variables; ++n_id) {
            if (numeric_task.get_action_is_assignment(op_id)[n_id]) {
                ap_float w = coefficients[n_id];

                if (use_bounds
                    && ((w >= precision && numeric_bound.has_no_increasing_assignment_effect(op_id, n_id))
                        || (w <= -precision && numeric_bound.has_no_decreasing_assignment_effect(op_id, n_id)))) {
                    continue;
                }

                ap_float c = numeric_task.get_action_assign_list(op_id)[n_id];
                int id_num = numeric_task.get_numeric_variable(n_id).id_abstract_task;
                ap_float s_val = state.nval(id_num);

                if ((w >= precision && c > s_val) || (w <= -precision && c < s_val)) {
                    net += w * (c - s_val);
                }
            }
        }

        for (auto var_eff : numeric_task.get_action_conditional_assign_list(op_id)) {
            int n_id = var_eff.first;
            ap_float w = coefficients[n_id];

            if (use_bounds
                && ((w >= precision && numeric_bound.has_no_increasing_assignment_effect(op_id, n_id))
                    || (w <= -precision && numeric_bound.has_no_decreasing_assignment_effect(op_id, n_id)))) {
                continue;
            }

            ap_float c = var_eff.second;
            int id_num = numeric_task.get_numeric_variable(n_id).id_abstract_task;
            ap_float s_val = state.nval(id_num);

            if ((w >= precision && c > s_val) || (w <= -precision && c < s_val)) {
                net += w * (c - s_val);
            }
        }

        // bounded linear assignment effect
        if (use_bounded_linear) {
            size_t n_linear_eff = numeric_task.get_action_n_linear_eff(op_id);

            for (size_t i = 0; i < n_linear_eff; ++i) {
                int lhs = numeric_task.get_action_linear_lhs(op_id)[i];
                ap_float w = coefficients[lhs];

                if (use_bounds
                    && ((w >= precision && numeric_bound.has_no_increasing_assignment_effect(op_id, lhs))
                        || (w <= -precision && numeric_bound.has_no_decreasing_assignment_effect(op_id, lhs)))) {
                    continue;
                }

                if (w >= precision && numeric_bound.get_assignment_has_ub(op_id, lhs)) {
                    int id_num = numeric_task.get_numeric_variable(lhs).id_abstract_task;
                    ap_float c = std::max(0.0, numeric_bound.get_assignment_ub(op_id, lhs) - state.nval(id_num));

                    if (numeric_bound.get_effect_has_ub(op_id, lhs))
                        c = std::min(c, numeric_bound.get_effect_ub(op_id, lhs));

                    net += w * c;
                } else if (w <= -precision && numeric_bound.get_assignment_has_lb(op_id, lhs)) {
                    int id_num = numeric_task.get_numeric_variable(lhs).id_abstract_task;
                    ap_float c = std::min(0.0, numeric_bound.get_assignment_lb(op_id, lhs) - state.nval(id_num));

                    if (numeric_bound.get_effect_has_lb(op_id, lhs))
                        c = std::max(c, numeric_bound.get_effect_lb(op_id, lhs));

                    net += w * c;
                }
            }
        }

        return net;
    }

    ap_float LandmarkCutLandmarks::calculate_linear_expression(const State &state, const std::vector<ap_float> &coefficients) const {
        ap_float value = 0.0;

        for (size_t n_id = 0, n_vars = numeric_task.get_n_numeric_variables(); n_id < n_vars; ++n_id) {
            int id_num = numeric_task.get_numeric_variable(n_id).id_abstract_task;
            value += coefficients[n_id] * state.nval(id_num);
        }

        return value;
    }

}

