#include "root_task.h"

#include "../axioms.h"
#include "../global_operator.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"

#include "../utils/collections.h"

#include <cassert>

using namespace std;

namespace tasks {
static GlobalOperator &get_operator_or_axiom(int index, bool is_axiom) {
    if (is_axiom) {
        assert(utils::in_bounds(index, g_axioms_as_operator));
        return g_axioms_as_operator[index];
    } else {
        assert(utils::in_bounds(index, g_operators));
        return g_operators[index];
    }
}

int RootTask::get_num_variables() const {
    return g_variable_domain.size();
}

int RootTask::get_num_numeric_variables() const {
	return g_numeric_var_names.size();
}

const string &RootTask::get_variable_name(int var) const {
    return g_variable_name[var];
}

const std::string& RootTask::get_numeric_variable_name(int var) const {
	assert(var < (int) g_numeric_var_names.size());
	return g_numeric_var_names[var];
}

int RootTask::get_variable_domain_size(int var) const {
    return g_variable_domain[var];
}

const string &RootTask::get_fact_name(const Fact &fact) const {
    return g_fact_names[fact.var][fact.value];
}

bool RootTask::are_facts_mutex(const Fact &fact1, const Fact &fact2) const {
    return are_mutex(fact1, fact2);
}

ap_float RootTask::get_operator_cost(int index, bool is_axiom) const {
//	cout << "Root task returns cost of op " << index << " which is " <<get_operator_or_axiom(index, is_axiom).get_cost()<< endl;
    return get_operator_or_axiom(index, is_axiom).get_cost();
}

const string &RootTask::get_operator_name(int index, bool is_axiom) const {
    return get_operator_or_axiom(index, is_axiom).get_name();
}

int RootTask::get_num_operators() const {
    return g_operators.size();
}

int RootTask::get_num_operator_preconditions(int index, bool is_axiom) const {
    return get_operator_or_axiom(index, is_axiom).get_preconditions().size();
}

Fact RootTask::get_operator_precondition(
    int op_index, int fact_index, bool is_axiom) const {
    const GlobalOperator &op = get_operator_or_axiom(op_index, is_axiom);
    const GlobalCondition &precondition = op.get_preconditions()[fact_index];
    return Fact(precondition.var, precondition.val);
}

int RootTask::get_num_operator_effects(int op_index, bool is_axiom) const {
    return get_operator_or_axiom(op_index, is_axiom).get_effects().size();
}

int RootTask::get_num_operator_ass_effects(int op_index, bool is_axiom) const {
	return get_operator_or_axiom(op_index, is_axiom).get_assign_effects().size();
}

int RootTask::get_num_operator_effect_conditions(
    int op_index, int eff_index, bool is_axiom) const {
    return get_operator_or_axiom(op_index, is_axiom).get_effects()[eff_index].conditions.size();
}

int RootTask::get_num_operator_ass_effect_conditions(int op_index,
		int ass_eff_index, bool is_axiom) const {
	return get_operator_or_axiom(op_index, is_axiom).get_assign_effects()[ass_eff_index].conditions.size();
}

Fact RootTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
    const GlobalEffect &effect = get_operator_or_axiom(op_index, is_axiom).get_effects()[eff_index];
    const GlobalCondition &condition = effect.conditions[cond_index];
    return Fact(condition.var, condition.val);
}

Fact RootTask::get_operator_ass_effect_condition(int op_index,
		int ass_eff_index, int cond_index, bool is_axiom) const {
	const AssignEffect &effect = get_operator_or_axiom(op_index, is_axiom).get_assign_effects()[ass_eff_index];
    const GlobalCondition &condition = effect.conditions[cond_index];
    return Fact(condition.var, condition.val);
}

Fact RootTask::get_operator_effect(
    int op_index, int eff_index, bool is_axiom) const {
    const GlobalEffect &effect = get_operator_or_axiom(op_index, is_axiom).get_effects()[eff_index];
    return Fact(effect.var, effect.val);
}

AssEffect RootTask::get_operator_ass_effect(int op_index,
		int eff_index, bool is_axiom) const {
	vector<AssignEffect> aevec = get_operator_or_axiom(op_index, is_axiom).get_assign_effects();
	assert(eff_index < (int) aevec.size());
    const AssignEffect &effect = aevec[eff_index];
    return AssEffect(effect.aff_var, effect.fop, effect.ass_var);
}


const GlobalOperator *RootTask::get_global_operator(int index, bool is_axiom) const {
    return &get_operator_or_axiom(index, is_axiom);
}

int RootTask::get_num_axioms() const {
	assert(g_axioms_as_operator.size() == g_logic_axioms.size());
    return g_axioms_as_operator.size();
}

int RootTask::get_num_ass_axioms() const {
	return g_ass_axioms.size();
}

int RootTask::get_num_cmp_axioms() const {
	return g_comp_axioms.size();
}


int RootTask::get_num_goals() const {
    return g_goal.size();
}

Fact RootTask::get_goal_fact(int index) const {
    pair<int, container_int> &goal = g_goal[index];
    return Fact(goal.first, goal.second);
}

vector<int> RootTask::get_initial_state_values() const {
    return get_state_values(g_initial_state());
}

std::vector<ap_float> RootTask::get_initial_state_numeric_values() const {
	return g_initial_state_numeric;
}

vector<int> RootTask::get_state_values(const GlobalState &global_state) const {
    // TODO: Use unpacked values directly once issue348 is merged.
    int num_vars = g_variable_domain.size();
    vector<int> values(num_vars);
    for (int var = 0; var < num_vars; ++var)
        values[var] = global_state[var];
    return values;
}

Fact RootTask::get_comparison_axiom_effect(
		int axiom_index, bool evaluation_result) const {
	int var_index= g_comp_axioms[axiom_index].affected_variable;
	int val_index = 1 - (int) evaluation_result; // fact 0: axiom evaluates to true
												 // fact 1: axiom evaluates to false
	return Fact(var_index, val_index);
}

int RootTask::get_comparison_axiom_argument(int axiom_index, bool left) const {
	if (left)
		return g_comp_axioms[axiom_index].var_lhs;
	return g_comp_axioms[axiom_index].var_rhs;
}

comp_operator RootTask::get_comparison_axiom_operator(int axiom_index) const {
	return g_comp_axioms[axiom_index].op;
}

int RootTask::get_assignment_axiom_effect(int axiom_index) const {
	assert(axiom_index < (int) g_ass_axioms.size());
	return g_ass_axioms[axiom_index].affected_variable;
}

int RootTask::get_assignment_axiom_argument(int axiom_index, bool left) const {
	if (left)
			return g_ass_axioms[axiom_index].var_lhs;
		return g_ass_axioms[axiom_index].var_rhs;
}

cal_operator RootTask::get_assignment_axiom_operator(int axiom_index) const {
	return g_ass_axioms[axiom_index].op;
}

vector<ap_float> RootTask::get_numeric_state_values(const GlobalState &global_state) const {
    // TODO: Use unpacked values directly once issue348 is merged.
//    int num_numeric_vars = get_num_numeric_variables();
//    vector<ap_float> values(num_numeric_vars);
//    vector<ap_float> numvars = global_state.get_numeric_vars();
	return global_state.get_numeric_vars();
//    assert((int) numvars.size() == num_numeric_vars);
//    for (int var = 0; var < num_numeric_vars; ++var)
//        values[var] = numvars[var];
//	if(DEBUG) cout << "RootTask returns numeric vector of size " << values.size() << endl;
//    return values;
}
    
    numType RootTask::get_numeric_var_type(int index) const {
        return g_numeric_var_types[index];
    }

static shared_ptr<AbstractTask> _parse(OptionParser &parser) {
    if (parser.dry_run())
        return nullptr;
    else
        return g_root_task();
}

static PluginShared<AbstractTask> _plugin("no_transform", _parse);
}
