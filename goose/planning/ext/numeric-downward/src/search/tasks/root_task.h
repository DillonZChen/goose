#ifndef TASKS_ROOT_TASK_H
#define TASKS_ROOT_TASK_H

#include "../abstract_task.h"

namespace tasks {
class RootTask : public AbstractTask {
public:
    virtual int get_num_variables() const override;
    virtual int get_num_numeric_variables() const override;
    virtual const std::string &get_variable_name(int var) const override;
    virtual const std::string &get_numeric_variable_name(int var) const override;
    virtual int get_variable_domain_size(int var) const override;
    virtual const std::string &get_fact_name(const Fact &fact) const override;
    virtual bool are_facts_mutex(
        const Fact &fact1, const Fact &fact2) const override;

    virtual ap_float get_operator_cost(int index, bool is_axiom) const override;
    virtual const std::string &get_operator_name(int index, bool is_axiom) const override;
    virtual int get_num_operators() const override;
    virtual int get_num_operator_preconditions(int index, bool is_axiom) const override;
    virtual Fact get_operator_precondition(
        int op_index, int fact_index, bool is_axiom) const override;
    virtual int get_num_operator_effects(int op_index, bool is_axiom) const override;
    virtual int get_num_operator_ass_effects(int op_index, bool is_axiom) const override;
    virtual int get_num_operator_effect_conditions(
        int op_index, int eff_index, bool is_axiom) const override;
    virtual int get_num_operator_ass_effect_conditions(
            int op_index, int ass_eff_index, bool is_axiom) const override;
    virtual Fact get_operator_effect_condition(
        int op_index, int eff_index, int cond_index, bool is_axiom) const override;
    virtual Fact get_operator_ass_effect_condition(
        int op_index, int ass_eff_index, int cond_index, bool is_axiom) const override;
    virtual Fact get_operator_effect(
        int op_index, int eff_index, bool is_axiom) const override;
    virtual AssEffect get_operator_ass_effect(
        int op_index, int eff_index, bool is_axiom) const override;
    virtual Fact get_comparison_axiom_effect(int axiom_index, bool evaluation_result) const override;
    virtual int get_comparison_axiom_argument(int axiom_index, bool left) const override; // true: left, false: right
    virtual comp_operator get_comparison_axiom_operator(int axiom_index) const override;
    virtual int get_assignment_axiom_effect(int axiom_index) const override;
    virtual int get_assignment_axiom_argument(int axiom_index, bool left) const override; // true: left, false: right
    virtual cal_operator get_assignment_axiom_operator(int axiom_index) const override;

    virtual const GlobalOperator *get_global_operator(int index, bool is_axiom) const override;

    virtual int get_num_axioms() const override;
    virtual int get_num_ass_axioms() const override;
    virtual int get_num_cmp_axioms() const override;

    virtual int get_num_goals() const override;
    virtual Fact get_goal_fact(int index) const override;

    virtual std::vector<int> get_initial_state_values() const override;
    virtual std::vector<ap_float> get_initial_state_numeric_values() const override;
    virtual std::vector<int> get_state_values(const GlobalState &global_state) const override;
    virtual std::vector<ap_float> get_numeric_state_values(const GlobalState &global_state) const override;
    
    virtual numType get_numeric_var_type(int index) const override;

};
}

#endif
