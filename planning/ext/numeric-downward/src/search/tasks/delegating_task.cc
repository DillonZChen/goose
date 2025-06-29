#include <cassert>

#include "delegating_task.h"

using namespace std;

namespace tasks {
DelegatingTask::DelegatingTask(const shared_ptr<AbstractTask> &parent)
    : parent(parent) {
}

int DelegatingTask::get_num_variables() const {
    return parent->get_num_variables();
}

const string &DelegatingTask::get_variable_name(int var) const {
    return parent->get_variable_name(var);
}

int DelegatingTask::get_variable_domain_size(int var) const {
    return parent->get_variable_domain_size(var);
}

const string &DelegatingTask::get_fact_name(const Fact &fact) const {
    return parent->get_fact_name(fact);
}

bool DelegatingTask::are_facts_mutex(const Fact &fact1, const Fact &fact2) const {
    return parent->are_facts_mutex(fact1, fact2);
}

ap_float DelegatingTask::get_operator_cost(int index, bool is_axiom) const {
    return parent->get_operator_cost(index, is_axiom);
}

const string &DelegatingTask::get_operator_name(int index, bool is_axiom) const {
    return parent->get_operator_name(index, is_axiom);
}

int DelegatingTask::get_num_operators() const {
    return parent->get_num_operators();
}

int DelegatingTask::get_num_operator_preconditions(int index, bool is_axiom) const {
    return parent->get_num_operator_preconditions(index, is_axiom);
}

Fact DelegatingTask::get_operator_precondition(
    int op_index, int fact_index, bool is_axiom) const {
    return parent->get_operator_precondition(op_index, fact_index, is_axiom);
}

int DelegatingTask::get_num_operator_effects(int op_index, bool is_axiom) const {
    return parent->get_num_operator_effects(op_index, is_axiom);
}

int DelegatingTask::get_num_operator_effect_conditions(
    int op_index, int eff_index, bool is_axiom) const {
    return parent->get_num_operator_effect_conditions(op_index, eff_index, is_axiom);
}

Fact DelegatingTask::get_operator_effect_condition(
    int op_index, int eff_index, int cond_index, bool is_axiom) const {
    return parent->get_operator_effect_condition(op_index, eff_index, cond_index, is_axiom);
}

Fact DelegatingTask::get_operator_effect(
    int op_index, int eff_index, bool is_axiom) const {
    return parent->get_operator_effect(op_index, eff_index, is_axiom);
}

const GlobalOperator *DelegatingTask::get_global_operator(int index, bool is_axiom) const {
    return parent->get_global_operator(index, is_axiom);
}

int DelegatingTask::get_num_axioms() const {
    return parent->get_num_axioms();
}

int DelegatingTask::get_num_goals() const {
    return parent->get_num_goals();
}

Fact DelegatingTask::get_goal_fact(int index) const {
    return parent->get_goal_fact(index);
}

std::vector<int> DelegatingTask::get_initial_state_values() const {
    return parent->get_initial_state_values();
}

vector<int> DelegatingTask::get_state_values(const GlobalState &global_state) const {
    return parent->get_state_values(global_state);
}


int DelegatingTask::get_num_ass_axioms() const {
    return parent->get_num_ass_axioms();
}

int DelegatingTask::get_num_numeric_variables() const {
	return parent->get_num_numeric_variables();
}

const std::string& DelegatingTask::get_numeric_variable_name(int var) const {
	//cerr << "Warning: virtual method 'get_numeric_variable_name(" << var << ")' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
    return g_numeric_var_names[var];
}

int DelegatingTask::get_num_operator_ass_effects(int op_index,
		bool is_axiom) const {
	//cerr << "Warning: virtual method 'get_num_operator_ass_effects("<< op_index<<", "<< is_axiom
	//		<< ")' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
	return parent->get_num_operator_ass_effects(op_index,is_axiom);
}

int DelegatingTask::get_num_operator_ass_effect_conditions(int op_index,
		int ass_eff_index, bool is_axiom) const {
	//cerr << "Warning: virtual method 'get_num_operator_ass_effect_conditions(" << op_index <<", "<< ass_eff_index<<", "<< is_axiom
	//		<< ")' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
	return parent->get_num_operator_ass_effect_conditions(op_index,ass_eff_index,is_axiom);
}

Fact DelegatingTask::get_operator_ass_effect_condition(
		int op_index, int ass_eff_index, int cond_index, bool is_axiom) const {
	//cerr << "Warning: virtual method 'get_operator_ass_effect_condition("<< op_index << ", "
	//		<< ass_eff_index << ", " << cond_index << ", " << is_axiom<< ")' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
    return parent->get_operator_ass_effect_condition(op_index,ass_eff_index,cond_index,is_axiom); //Fact(0,0);
}

AssEffect DelegatingTask::get_operator_ass_effect(int op_index,
		int eff_index, bool is_axiom) const {
	//cerr << "Warning: virtual method 'get_operator_ass_effect("<< op_index <<", " <<
	//	eff_index<<", "<< is_axiom<< ")' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
    return parent->get_operator_ass_effect(op_index,eff_index,is_axiom);//AssEffect(1, assign, 1);
}


int DelegatingTask::get_num_cmp_axioms() const {
	//cerr << "Warning: virtual method 'get_num_cmp_axioms()' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
	return parent->get_num_cmp_axioms();
}

std::vector<ap_float> DelegatingTask::get_initial_state_numeric_values() const {
	//cerr << "Warning: virtual method 'get_initial_state_numeric_values()' not implemented deriving from AbstractTask" << endl;
    //assert(false);
    // TODO modify this
    return parent->get_initial_state_numeric_values();//vector<ap_float>();
}

Fact DelegatingTask::get_comparison_axiom_effect(int axiom_index,
		bool evaluation_result) const {
	return parent->get_comparison_axiom_effect(axiom_index, evaluation_result);
}

int DelegatingTask::get_comparison_axiom_argument(int axiom_index,
		bool left) const {
	return parent->get_comparison_axiom_argument(axiom_index,left);
}

comp_operator DelegatingTask::get_comparison_axiom_operator(
		int axiom_index) const {
	return parent->get_comparison_axiom_operator(axiom_index);
}

int DelegatingTask::get_assignment_axiom_effect(int axiom_index) const {
	return parent->get_assignment_axiom_effect(axiom_index);
}

int DelegatingTask::get_assignment_axiom_argument(int axiom_index,
		bool left) const {
	return parent->get_assignment_axiom_argument(axiom_index, left);
}

cal_operator DelegatingTask::get_assignment_axiom_operator(
		int axiom_index) const {
	return parent->get_assignment_axiom_operator(axiom_index);
}

std::vector<ap_float> DelegatingTask::get_numeric_state_values(
		const GlobalState& global_state) const {
	//cerr << "Warning: virtual method 'get_state_numeric_values(GlobalState object)' not implemented deriving from AbstractTask, dumping global state" << endl;
	//global_state.dump_fdr();
	//assert(false);
    //TODO: modify this
	//return vector<ap_float>();
    return parent->get_numeric_state_values(global_state);

    
}
    
    numType DelegatingTask::get_numeric_var_type(int index) const{
        return parent->get_numeric_var_type(index);
    }
}
