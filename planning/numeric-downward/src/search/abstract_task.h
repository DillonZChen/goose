#ifndef ABSTRACT_TASK_H
#define ABSTRACT_TASK_H

#include "globals.h"
#include "global_state.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

class GlobalOperator;
class GlobalState;

namespace options {
class Options;
}

struct Fact {
private:
    std::pair<int, int> get_pair() const {
        return std::make_pair(var, value);
    }

public:
    int var;
    int value;

    Fact(int var, int value)
        : var(var), value(value) {
    }

    bool operator<(const Fact &other) const {
        return get_pair() < other.get_pair();
    }

    bool operator==(const Fact &other) const {
        return get_pair() == other.get_pair();
    }

    bool operator!=(const Fact &other) const {
        return !(*this == other);
    }
};

struct AssEffect {
	int aff_var;
	f_operator op_type;
	int ass_var;
	AssEffect(int aff_var, f_operator op_type, int ass_var) :
		aff_var(aff_var), op_type(op_type), ass_var(ass_var) {}
};

class AbstractTask {
public:
    AbstractTask() = default;
    virtual ~AbstractTask() = default;
    virtual int get_num_variables() const = 0;
    virtual int get_num_numeric_variables() const = 0;
    virtual const std::string &get_variable_name(int var) const = 0;
    virtual const std::string &get_numeric_variable_name(int var) const = 0;
    virtual int get_variable_domain_size(int var) const = 0;
    virtual const std::string &get_fact_name(const Fact &fact) const = 0;
    virtual bool are_facts_mutex(const Fact &fact1, const Fact &fact2) const = 0;

    virtual ap_float get_operator_cost(int index, bool is_axiom) const = 0;
    virtual const std::string &get_operator_name(int index, bool is_axiom) const = 0;
    virtual int get_num_operators() const = 0;
    virtual int get_num_operator_preconditions(int index, bool is_axiom) const = 0;
    virtual Fact get_operator_precondition(
        int op_index, int fact_index, bool is_axiom) const = 0;
    virtual int get_num_operator_effects(int op_index, bool is_axiom) const = 0;
    virtual int get_num_operator_ass_effects(int op_index, bool is_axiom) const = 0;
    virtual int get_num_operator_effect_conditions(
        int op_index, int eff_index, bool is_axiom) const = 0;
    virtual int get_num_operator_ass_effect_conditions(
        int op_index, int ass_eff_index, bool is_axiom) const = 0;
    virtual Fact get_operator_effect_condition(
        int op_index, int eff_index, int cond_index, bool is_axiom) const = 0;
    virtual Fact get_operator_ass_effect_condition(
        int op_index, int ass_eff_index, int cond_index, bool is_axiom) const = 0;
    virtual Fact get_operator_effect(
        int op_index, int eff_index, bool is_axiom) const = 0;
    virtual Fact get_comparison_axiom_effect(int axiom_index, bool evaluation_result) const = 0;
    virtual int get_comparison_axiom_argument(int axiom_index, bool left) const = 0; // true: left, false: right
    virtual comp_operator get_comparison_axiom_operator(int axiom_index) const = 0;
    virtual int get_assignment_axiom_effect(int axiom_index) const = 0;
    virtual int get_assignment_axiom_argument(int axiom_index, bool left) const = 0; // true: left, false: right
    virtual cal_operator get_assignment_axiom_operator(int axiom_index) const = 0;
    virtual AssEffect get_operator_ass_effect(
        int op_index, int eff_index, bool is_axiom) const = 0;
    virtual const GlobalOperator *get_global_operator(int index, bool is_axiom) const = 0;

    virtual int get_num_axioms() const = 0;
    virtual int get_num_ass_axioms() const = 0;
    virtual int get_num_cmp_axioms() const = 0;

    virtual int get_num_goals() const = 0;
    virtual Fact get_goal_fact(int index) const = 0;

    virtual std::vector<int> get_initial_state_values() const = 0;
    virtual std::vector<ap_float> get_initial_state_numeric_values() const = 0;
    virtual std::vector<int> get_state_values(const GlobalState &global_state) const = 0;
    virtual std::vector<ap_float> get_numeric_state_values(const GlobalState &global_state) const = 0;
    
    // add other numeric informations
    virtual numType get_numeric_var_type(int index) const = 0;
};

const std::shared_ptr<AbstractTask> get_task_from_options(
    const options::Options &opts);

#endif
