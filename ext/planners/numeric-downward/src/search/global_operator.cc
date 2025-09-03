#include "global_operator.h"



#include "utils/collections.h"
#include "utils/system.h"
#include "globals.h"
#include "state_registry.h" // g_state_registry has to be accessed by set_cost
#include "axioms.h" // convert_from_axiom requires this

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;
using utils::ExitCode;

static void check_fact(int var, container_int val) {
    if (!utils::in_bounds(var, g_variable_domain)) {
        cerr << "Invalid variable id: " << var << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
    //if (val < 0 || val >= g_variable_domain[var]) {
    if (val >= g_variable_domain[var]) {
        cerr << "Invalid value for variable " << var << ": " << val << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
}

GlobalCondition::GlobalCondition(istream &in) {
	var = -1; val = -1;
    in >> var >> val;
    check_fact(var, val);
}

GlobalCondition::GlobalCondition(int variable, int value)
    : var(variable),
      val(value) {
    check_fact(var, val);
}

AssignEffect::AssignEffect(istream &in) {
	aff_var = -1;
	ass_var = -1;
	fop = assign;

	int cond_count;
	in >> cond_count;
	if(cond_count > 0) {is_conditional_effect = true;}
	else {is_conditional_effect = false;}
    for (int i = 0; i < cond_count; ++i)
        conditions.push_back(GlobalCondition(in));
    in >> aff_var >> fop >> ass_var;
//    cout << "read effect " << aff_var << " " << fop << " " << ass_var << "!!!" << endl;
}

// TODO if the input file format has been changed, we would need something like this
// Effect::Effect(istream &in) {
//    int cond_count;
//    in >> cond_count;
//    for (int i = 0; i < cond_count; ++i)
//        cond.push_back(Condition(in));
//    in >> var >> post;
//}

GlobalEffect::GlobalEffect(int variable, container_int value, const vector<GlobalCondition> &conds)
    : var(variable),
      val(value),
      conditions(conds) {
    check_fact(var, val);
}

void GlobalOperator::read_pre_post(istream &in) {
    int cond_count, var, pre, post;
    in >> cond_count;
    vector<GlobalCondition> conditions;
    conditions.reserve(cond_count);
    for (int i = 0; i < cond_count; ++i)
        conditions.push_back(GlobalCondition(in));
    in >> var >> pre >> post;
    if (pre != -1)
        check_fact(var, pre);
    check_fact(var, post);
    if (pre != -1)
        preconditions.push_back(GlobalCondition(var, pre));
    effects.push_back(GlobalEffect(var, post, conditions));
}

GlobalOperator::GlobalOperator(istream &in, bool axiom) {
    marked = false;

    is_an_axiom = axiom;
    if (!is_an_axiom) {
    	check_magic(in, "begin_operator");
    	in >> ws;
    	getline(in, name);
//    	cout << "reading operator " << name << endl;
    	int count;
    	in >> count;
    	for (int i = 0; i < count; ++i)
    		preconditions.push_back(GlobalCondition(in));
    	in >> count;
    	for (int i = 0; i < count; ++i)
    		read_pre_post(in);
    	in >> count;
    	for (int i = 0; i < count; i++){
    		AssignEffect effect =  AssignEffect(in);
    		assign_effects.push_back(effect);
    	}
    	ap_float op_cost;
    	in >> op_cost;
    	cost = g_use_metric ? op_cost : 1;

    	g_min_action_cost = min(g_min_action_cost, cost);
    	g_max_action_cost = max(g_max_action_cost, cost);

    	//cout << "Operator has (deprecated) cost " << deprecated_cost << " usemetric? " << (g_use_metric?"Y":"N") << endl;
    	check_magic(in, "end_operator");
    } else {
        name = "<axiom>";
        cost = 0;
        check_magic(in, "begin_rule");
        read_pre_post(in);
        check_magic(in, "end_rule");
    }
}

void GlobalCondition::dump() const {
    cout << g_variable_name[var] << ": " << val;
}

void GlobalEffect::dump() const {
    cout << g_variable_name[var] << ":= " << val;
    if (!conditions.empty()) {
        cout << " if";
        for (size_t i = 0; i < conditions.size(); ++i) {
            cout << " ";
            conditions[i].dump();
        }
    }
}

void AssignEffect::dump() const {
    cout << g_numeric_var_names[aff_var] << " " << fop << " " << g_numeric_var_names[ass_var];
    if (!conditions.empty()) {
        cout << " if";
        for (size_t i = 0; i < conditions.size(); ++i) {
            cout << " ";
            conditions[i].dump();
        }
    }
}

void GlobalOperator::dump() const {
    cout << name << ":";
    cout << " Pre: " << preconditions.size();
    for (size_t i = 0; i < preconditions.size(); ++i) {
        cout << " [";
        preconditions[i].dump();
        cout << "]";
    }
    cout << " Eff: " << effects.size();
    for (size_t i = 0; i < effects.size(); ++i) {
        cout << " [";
        effects[i].dump();
        cout << "]";
    }
    cout << " NumE: "<< assign_effects.size();
    for (size_t i=0; i < assign_effects.size(); ++i) {
    	cout << " [";
    	assign_effects[i].dump();
    	cout << "]";
    }
    cout << " cost :";
    cout << " (" << cost << ")" << endl;
}

void GlobalOperator::set_cost(ap_float init_cost) {
//	if(DEBUG) cout << "Recomputing cost of operator " << name << endl;
	ap_float old_cost = cost;
    if (g_use_metric) {
    	assert(g_state_registry);
    	vector<ap_float> initial_numeric = g_initial_state().get_numeric_vars();
    	vector<ap_float> initial_metric = vector<ap_float>(g_initial_state_numeric.size()); // this is bigger than needed, but we discard it anyways
    	g_state_registry->get_numeric_successor(initial_numeric, initial_metric, (*this));
    	cost = g_state_registry->evaluate_metric(initial_numeric) - init_cost;
    } else
    	cost = 1;
    if (DEBUG && old_cost != cost) cout << "Cost of Operator " << name << " changed from " << old_cost << " to " << cost << endl;
}

GlobalOperator::GlobalOperator(PropositionalAxiom convert_from_axiom) :
		is_an_axiom(true),
		preconditions(convert_from_axiom.conditions),
		effects(convert_from_axiom.effects),
		assign_effects(vector<AssignEffect>()),
		name("OpLogicAxiom"),
		cost(0),
		marked(false)
		{}
