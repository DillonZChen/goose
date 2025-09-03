#include "repetition_relaxation_heuristic.h"

#include "relaxed_interval_helper.h"
#include "../globals.h"
#include "../axioms.h"
#include "../utils/collections.h"

#include <cmath>
#include <limits>
#include <cassert>
#include <vector>
#include <sstream>


using namespace std;

namespace repetition_relaxation_heuristic{

int global_achiever_ordering_rank;

void RepetitionRelaxationHeuristic::build_unary_operators(const OperatorProxy &op, int op_no) {
	//	if (DEBUG) cout << "Building unary Operators for Operator "<< op.get_name() << endl;
	ap_float base_cost = op.get_cost();
	//    if (DEBUG) cout << "Operator "<< op.get_name() << " costs " << base_cost << endl;
	vector<IProposition *> precondition_props;
	for (FactProxy precondition : op.get_preconditions()) {
		precondition_props.push_back(get_proposition(precondition));
	}
	for (EffectProxy effect : op.get_effects()) {
		UnaryEffect seffect = get_effect(effect);
		EffectConditionsProxy eff_conds_p = effect.get_conditions();
		for (FactProxy eff_cond : (EffectConditionsProxy) eff_conds_p) {
			precondition_props.push_back(get_proposition(eff_cond));
		}

		classic_operators.push_back(UnaryOperator(op_no, precondition_props, seffect, base_cost, logic_op));
		//        if(DEBUG) cout << "  " << classic_operators.back().str();
		precondition_props.erase(precondition_props.end() - eff_conds_p.size(), precondition_props.end());
	}
	for (AssEffectProxy effect : op.get_ass_effects()) {
		UnaryEffect seffect = get_effect(effect);
		AssEffectConditionsProxy ass_eff_conds_p = effect.get_conditions();
		for (FactProxy eff_cond : (AssEffectConditionsProxy) ass_eff_conds_p) {
			precondition_props.push_back(get_proposition(eff_cond));
		}
		num_operators.push_back(UnaryOperator(op_no, precondition_props, seffect, base_cost, numeric_op));
		//        if(DEBUG) cout << "  " << num_operators.back().str();
		precondition_props.erase(precondition_props.end() - ass_eff_conds_p.size(), precondition_props.end());
	}

}

// initialization
void RepetitionRelaxationHeuristic::initialize() {
	if(DEBUG) cout << "Initializing common things for all relaxation heuristics " << endl;
	global_achiever_ordering_rank = 0;

	// Build propositions.
	int prop_id = 0;
	VariablesProxy variables = task_proxy.get_variables();
	prop_variables.resize(variables.size());

	NumericVariablesProxy num_variables_p = task_proxy.get_numeric_variables();

	if(DEBUG) cout << "task has " << prop_variables.size() << " propositional variables" << endl;
	for (FactProxy fact : variables.get_facts()) {
		assert(fact.get_variable().get_id() < (int) prop_variables.size());
		prop_variables[fact.get_variable().get_id()].push_back(IProposition(prop_id++));
	}
	// vector prop_variables will not be changed any more, so from now on pointers are stable
	linearized_prop_variables.clear();
	linearized_fact_names.clear();

	if (DEBUG) cout << "Linearized Propositions:" << endl;
	for (size_t var = 0; var < prop_variables.size(); ++var) {
		for (size_t value = 0; value < prop_variables[var].size(); ++value) {
			assert((int) linearized_prop_variables.size()== prop_variables[var][value].id);
			//    		if(DEBUG) cout << "PropID: " << prop_variables[var][value].id << " name: " << g_fact_names[var][value] << endl;
			linearized_prop_variables.push_back(&prop_variables[var][value]);
			linearized_fact_names.push_back(g_fact_names[var][value]);
		}
	}


	if(DEBUG) cout << "task has " << num_variables_p.size() << " numeric variables" << endl;
	prop_id = 0;
	for (NumericVariableProxy numvar : (NumericVariablesProxy) num_variables_p) {
		assert(numvar.get_id() == prop_id);
		++prop_id;
		numeric_variables.push_back(NumericStateVariable(numvar.get_id()));
	}

	// Build goal propositions.
	for (FactProxy goal : task_proxy.get_goals()) {
		IProposition *prop = get_proposition(goal);
		prop->is_goal = true;
		goal_propositions.push_back(prop);
	}

	// Build unary operators for operators and axioms.
	cout << "Building unary operators from " << g_operators.size() << " operators" << endl;
	int op_no = 0;

	for (OperatorProxy op : task_proxy.get_operators()) {
		build_unary_operators(op, op_no++);
	}
	cout << "Building " << g_axioms_as_operator.size() << " unary axioms" << endl;
	for (OperatorProxy axiom : task_proxy.get_axioms())
		build_unary_operators(axiom, -1);
	for (ComparisonAxiomProxy cax : task_proxy.get_comparison_axioms())
		build_unary_comparison_axioms(cax);
	for (AssignmentAxiomProxy aax : task_proxy.get_assignment_axioms())
		build_unary_assignment_axioms(aax);

	build_dummy_operators();

	for (auto & op : prop_var_dummies)
		all_operators.push_back(&op);
	//    for (auto & op : num_var_dummies)
	//    	all_operators.push_back(&op);
	for (auto & op : classic_operators)
		all_operators.push_back(&op);
	for (auto & op : num_operators)
		all_operators.push_back(&op);
	for (auto & op : comparison_axioms)
		all_operators.push_back(&op);
	for (auto & op : ass_axioms)
		all_operators.push_back(&op);

	// Simplify unary operators.
	//simplify();
	// Cross-reference unary operators.
	for (UnaryOperator *op : all_operators) {
		//    	cout << "Handling SingularyOperator " << op->str() << endl;
		for (auto precon : op->precondition)
			precon->precondition_of.push_back(op);
		if (op->op_type == numeric_op) {
			//        	cout << "numeric Effect!!!" << endl;
			assert((int) numeric_variables.size() > op->effect.aff_variable_index);
			assert((int) numeric_variables.size() > op->effect.val_or_ass_var_index);
			NumericStateVariable* aff_var = &numeric_variables[op->effect.aff_variable_index];
			NumericStateVariable* ass_var = &numeric_variables[op->effect.val_or_ass_var_index];
			aff_var->depends_on(ass_var);
			ass_var->precondition_of.push_back(op); // the operator is triggered again, when the assignment variable changes
			//        	cout << g_numeric_var_names[aff_var->id] << " depends on " << g_numeric_var_names[ass_var->id] << endl;
		} else if (op->op_type == ass_axiom) {
			assert(op->effect.numeric);
			assert(op->axiom_left_right.size() == 2);
			op->axiom_left_right[0]->precondition_of.push_back(op);
			op->axiom_left_right[1]->precondition_of.push_back(op);
		} else if (op->op_type == comp_axiom) {
			assert(!op->effect.numeric);
			assert(op->axiom_left_right.size() == 2);
			op->axiom_left_right[0]->precondition_of.push_back(op);
			op->axiom_left_right[1]->precondition_of.push_back(op);
		}
	}
	//    cout << "Handling AssAxioms ... " << endl;
	for (UnaryOperator &ass_ax : ass_axioms) {
		NumericStateVariable* aff_var = &numeric_variables[ass_ax.effect.aff_variable_index];
		assert(ass_ax.axiom_left_right.size() == 2);
		NumericStateVariable* left_var = ass_ax.axiom_left_right[0];
		NumericStateVariable* right_var = ass_ax.axiom_left_right[1];
		aff_var->depends_on(left_var);
		aff_var->depends_on(right_var);
		//		cout << g_numeric_var_names[aff_var->id] << " depends on " << g_numeric_var_names[left_var->id] << " and on " << g_numeric_var_names[right_var->id] << endl;
	}
	initialize_topology();
}

void RepetitionRelaxationHeuristic::initialize_topology() {
	if (DEBUG) cout << "Computing topologies..." << endl;
	vector<int> predecessors; // number of topology parents of each node (index = var id)
	AdaptiveQueue<int> topology_queue; // priority: new topology level, val: variable index
	size_t processed = 0; // number of variables that have been assigned a topology level
	assert(numeric_cycle_breaker_vars.empty());

	for (auto &nvar : numeric_variables) {
		int parents = nvar.top_parent_size();
		predecessors.push_back(parents);
		if (parents == 0) {
			nvar.set_topology(0);
			++processed;
			topology_queue.push(0, nvar.get_id());
		}
	}
	while(processed < predecessors.size()) {
		while(!topology_queue.empty()) {
			pair<ap_float, int> top_pair = topology_queue.pop();
			numeric_variables[top_pair.second].set_topology((int) top_pair.first);
			if (DEBUG) cout << "Setting topology level of " << g_numeric_var_names[top_pair.second] << " to " << top_pair.first << endl;
			for(auto *next : numeric_variables[top_pair.second].get_depending_variables()) {
				//next->set_topology((int) top_pair.first + 1); // topology is at least current topology + 1
				if (--predecessors[next->get_id()] == 0) {
					++processed;
					topology_queue.push(top_pair.first + 1, next->get_id());
				}
			}
		}
		if(processed < predecessors.size()) {
			/* We break topology cycles by introducing a new variable. We prune all children of the original
			 * variable, and add these children to the newly introduced one.
			 * The new variable has a low topology index the old variable will get a higher one
			 * (after this while loop ends)
			 * We then add an artificial cycle breaker action that can alter the newly introduced
			 * variable.
			 */

			// introduce artificial cycle breaker action
			int breaker_var_index = 0;
			for (auto nvar : numeric_variables)
				if (predecessors[breaker_var_index] == 0 // find any cyclic variable
						|| (predecessors[nvar.get_id()] > 0 // among all cyclic variables, use the one with most depending_variables
								&& nvar.get_depending_variables().size() > numeric_variables[breaker_var_index].get_depending_variables().size()))
					breaker_var_index = nvar.get_id();
			NumericStateVariable *breakervar = &numeric_variables[breaker_var_index];


			if (DEBUG) cout << "Adding cycle breaker copy of  " << breakervar->str() << " with " << predecessors[breaker_var_index] << " predecessors " << endl;
			set<NumericStateVariable*> &topology_parents = breakervar->get_topology_parents();
			//    		cout << "I have " << topology_parents.size() << " parents " << endl;
			int breaker_topology = 0;
			for (auto parent : topology_parents) {
				breaker_topology = max(breaker_topology, parent->get_topology());
			}

			// put a copy of the variable into the breaker vars vector
			NumericStateVariable copyvar = (*breakervar);
			copyvar.set_topology(++breaker_topology);

			copyvar.get_topology_parents().clear(); // remove parents from copy
			breakervar->get_depending_variables().clear(); // removing children from original variable

			//    		cout << "*****  Old variables is " << endl;
			//    		breakervar->dump();
			//    		cout << "*****  Numeric variable vector contains (should be old variable) " << endl;
			//    		numeric_variables[breaker_var_index].dump();
			//    		cout << "*****  Copied variables is " << endl;
			//    		copyvar.dump();
			cycle_breaker_index.insert(pair<int,int>(copyvar.get_id(), numeric_cycle_breaker_vars.size()));
			numeric_cycle_breaker_vars.push_back(copyvar);
			UnaryEffect dummy_effect = UnaryEffect(copyvar.get_id(), assign, -1);
			cycle_breakers.push_back(UnaryOperator(dummy_effect));
			//    		if (DEBUG) cout << "Topology level of copied cycle breaker variable " << g_numeric_var_names[breaker_var_index] << " is " << breaker_topology << endl;
			for(auto *next : copyvar.get_depending_variables()) {
				if (--predecessors[next->get_id()] == 0) {
					++processed;
					topology_queue.push(breaker_topology + 1, next->get_id());
				}
			}
		}
	}

	if (DEBUG && numeric_cycle_breaker_vars.size() > 0) cout << "Task has " << numeric_cycle_breaker_vars.size() << " variables with cyclic dependency" << endl;
	// Now the vector/map is settled and we can use pointers to its variables
	assert(numeric_cycle_breaker_vars.size() == cycle_breakers.size());

	size_t corresponding_index = 0;
	for (NumericStateVariable &nsv : numeric_cycle_breaker_vars) {
		// Fixing incoming and outgoing dependencies
		//    	cout << "Handling cycle breaker variable " << nsv.str() << endl;
		int bv_top = numeric_variables[nsv.get_id()].get_topology();
		// remove all Operators in "precondition of" with lower topology from orignial variable
		std::vector<UnaryOperator *> &triggered_operators = numeric_variables[nsv.get_id()].precondition_of;
		for(vector<UnaryOperator *>::iterator precondition = triggered_operators.begin(); precondition != triggered_operators.end();) {
			UnaryOperator *op = (*precondition);
			//    		cout << "Checking whether " << op->str() << " should still be triggered " << endl;
			if (op->op_type == ass_axiom) {
				assert(op->axiom_left_right[0]->get_id() == nsv.get_id() || op->axiom_left_right[1]->get_id() == nsv.get_id());
				//    			cout << "Left = " << op->axiom_left_right[0]->str() << endl;
				//    			cout << "Right = " << op->axiom_left_right[1]->str() << endl;
				NumericStateVariable *ax_eff = &numeric_variables[op->effect.aff_variable_index];
				//    			cout << "Effect = " << ax_eff->str() << endl;
				if (ax_eff->get_topology() < bv_top) {
					//    				cout << "Erasing operator "<< op->str() <<" from trigger list of " << nsv.str() << endl;
					precondition = triggered_operators.erase(precondition);
					if(op->axiom_left_right[0]->get_id() == nsv.get_id()) {
						//    					cout << "Replacing left axiom precondition by dummy variable" << endl;
						op->axiom_left_right[0] = &nsv;
					}
					if(op->axiom_left_right[1]->get_id() == nsv.get_id()) {
						//    					cout << "Replacing right axiom precondition by dummy variable" << endl;
						op->axiom_left_right[1] = &nsv;
					}
				} else {
					//    				cout << op->str() << " can still be executed safely "<<ax_eff->get_topology() << " >= " << bv_top << endl;
					precondition++;
				}
			} else if (op->op_type == numeric_op) {
				NumericStateVariable *ax_eff = &numeric_variables[op->effect.aff_variable_index];
				//    			cout << "Effect = " << ax_eff->str() << endl;
				//    			cout << "Assignment = " << numeric_variables[op->effect.val_or_ass_var_index].str() << endl;
				assert(op->effect.val_or_ass_var_index == nsv.get_id());
				if (ax_eff->get_topology() < bv_top) {
					//    				cout << "Erasing operator "<< op->str() <<" from trigger list of " << nsv.str() << endl;
					precondition = triggered_operators.erase(precondition);
				} else {
					//    				cout << op->str() << " can still be executed safely "<<ax_eff->get_topology() << " >= " << bv_top << endl;
					precondition++;
				}
			} else {
				assert(op->op_type == comp_axiom);
				precondition++;
			}
		}

		// Determining cost of

		// add cycle breaker operator to the list of triggered operators of original variable
		all_operators.push_back(&cycle_breakers[corresponding_index]);
		numeric_variables[nsv.get_id()].precondition_of.push_back(&cycle_breakers[corresponding_index]);
		++corresponding_index;
		//    	cout << "Breaker var triggers now " << numeric_variables[nsv.get_id()].precondition_of.size()
		//    			<< " operators while the copy triggers " << nsv.precondition_of.size() << endl << "---" << endl;
	}
}

// construction and destruction
RepetitionRelaxationHeuristic::RepetitionRelaxationHeuristic(const options::Options &opts) :
								Heuristic(opts){
}

RepetitionRelaxationHeuristic::~RepetitionRelaxationHeuristic() {
}

IProposition* RepetitionRelaxationHeuristic::get_proposition(
		const FactProxy& fact) {
	int var = fact.get_variable().get_id();
	int value = fact.get_value();
	assert(utils::in_bounds(var, prop_variables));
	assert(utils::in_bounds(value, prop_variables[var]));
	//    cout << " return var " << var << "][" << value << endl;
	return &prop_variables[var][value];
}

UnaryEffect RepetitionRelaxationHeuristic::get_effect(EffectProxy effect) {
	const FactProxy& fact = effect.get_fact();
	return UnaryEffect(fact.get_variable().get_id(), fact.get_value());
}

UnaryEffect RepetitionRelaxationHeuristic::get_effect(AssEffectProxy num_effect) {
	const NumAssProxy& assignment = num_effect.get_assignment();
	return UnaryEffect(assignment.get_affected_variable().get_id(),
			assignment.get_assigment_operator_type(),
			assignment.get_assigned_variable().get_id());
}

//size_t RepetitionRelaxationHeuristic::get_best_achiever_index(
//		const std::vector<NumericAchiever>& achievers, int achiever_ordering_bound,
//		ap_float target_val) {
//	size_t min_index = 0;
//	size_t max_index = achievers.size();
//
//	while(min_index < max_index) {
//		size_t middle_index = (min_index + max_index) / 2;
//		assert(middle_index < achievers.size()); // while loop should already have terminated
//		int ordering_number = 0;
//		if (middle_index >= 1)
//			ordering_number = achievers[middle_index - 1].order_pos;
//		if (ordering_number > achiever_ordering_bound) {
//			if (DEBUG) cout << "Assignment #" << middle_index << "'s index is too high " << ordering_number << " > " << achiever_ordering_bound << endl;
//			max_index = middle_index - 1;
//			continue;
//		}
//		// determine the interval at the current index
//		Interval mii = achievers[middle_index].reached_interval;
//		if (mii.contains(target_val)) {
//			max_index = middle_index;
//		} else {
//			min_index = middle_index + 1;
//		}
//	}
//	assert(min_index == max_index);
//	if (min_index < achievers.size()) assert(achievers[min_index].reached_interval.contains(target_val));
//	// we would also like to assert that if the index equals achievers.size() that nvar.val.contains(target_val)
//	// but we do not have access to the variable nvar
//	if (min_index >= 1) assert(achievers[min_index-1].order_pos < achiever_ordering_bound);
//	return min_index;
//}

bool RepetitionRelaxationHeuristic::dead_ends_are_reliable() const {
	return true;
}

void NumericStateVariable::dump() {
	cout << "NSV " << g_numeric_var_names[id]<< " topology level: " << topology_level << endl;
	cout << "0 \t- " << init << endl;
	for (const auto achiever : achievers) {
		cout << achiever.cost << " \t- " << achiever.reached_interval <<
				" from #" << achiever.enque_pos << " by #" << achiever.order_pos << " " << achiever.reached_by->str() << endl;
	}
	cout << "MaxVal \t= " << max_iv << " cost = " << cost() << endl;
	cout << "    depends on:";
	for (auto nvar: topology_parents)
		cout << " " << g_numeric_var_names[nvar->id];
	cout << endl << "    dependant:";
	for (auto nvar: topology_children)
		cout << " " << g_numeric_var_names[nvar->id];
	cout << endl;
}

std::string UnaryOperator::str() {
	stringstream ss;
	string effstr =  effect.numeric?g_numeric_var_names[effect.aff_variable_index]:effect.str();

	if (operator_no == -1) {
		ss << "LogicAxiom on " << effstr;
	} else {
		switch(op_type) {
		case logic_op:
			assert (operator_no < (int) g_operators.size());
			ss << "UnaryOperator on " << effstr << " of " << g_operators[operator_no].get_name();
			break;
		case numeric_op:
			assert (operator_no < (int) g_operators.size());
			ss << "UnaryOperator on " << effstr << " of " << g_operators[operator_no].get_name();
			break;
		case ass_axiom:
			assert(axiom_left_right.size()== 2);
			ss << "Assignment Axiom computing " << effstr;// << " from " << axiom_left_right[0]->str() << " "
			//<< ass_ax_op << " " << axiom_left_right[1]->str();
			break;
		case comp_axiom:
			assert(axiom_left_right.size()== 2);
			ss << "Comparison Axiom computing " << effstr;// << " from " << axiom_left_right[0]->str() << " "
			//<< comp_ax_op << " " << axiom_left_right[1]->str();
			break;
		case dummy:
			ss << "Dummy Operator of " << effstr;
			break;
		default:
			assert(false);
			break;
		}
	}
	return ss.str();
}

std::string UnaryEffect::str() {
	stringstream ss;
	if (numeric) {
		ss << g_numeric_var_names[aff_variable_index] << " " << assign_type;
		if (val_or_ass_var_index >= 0)
			ss << " " << g_numeric_var_names[val_or_ass_var_index];
		else {
			assert (val_or_ass_var_index == -1);
			ss << " [DUMMY EFFECT]";
		}
	} else {
		ss << g_fact_names[aff_variable_index][val_or_ass_var_index];
	}
	return ss.str();
}


void NumericStateVariable::block() { //
	++missing_depending_applicable_op_count;
	if (!blocked) {
		//		if(DEBUG) cout << "  -- blocking " << str() << endl;
		blocked = true;
		for (auto nvar : topology_children) {
			nvar->block();
		}
	}
	//	if (DEBUG) cout << str() << " misses now " << missing_depending_applicable_op_count << endl;
}

void NumericStateVariable::tblock() {
	if(++missing_depending_applicable_op_count == 1) {
		//		if(DEBUG) cout << "Recursively blocking "<< topology_children.size() <<" children of " << str() << endl;
		for (auto nvar : topology_children) {
			nvar->block();
		}
	}
	else if (DEBUG) {
		//		cout << str() << " tmisses now " << missing_depending_applicable_op_count << endl;
		for (auto nvar : topology_children) { // debug assertion can be removed
			_unused(nvar);
			assert(nvar->blocked);
		}
	}

}

void NumericStateVariable::free() {
	assert(missing_depending_applicable_op_count > 0);
	if(--missing_depending_applicable_op_count == 0) {
		//		if(DEBUG) cout << "  -- freeing " << str() << endl;
		assert(blocked);
		blocked = false;
		for (auto nvar : topology_children) {
			nvar->free();
		}
	} // else if (DEBUG) cout << str() << " requires still " << missing_depending_applicable_op_count << endl;
}

bool NumericStateVariable::tfree() {
	assert(missing_depending_applicable_op_count > 0);
	if(--missing_depending_applicable_op_count == 0) { // done altering this variable, free children
		//		if(DEBUG) cout << "   topology children of " << str() << " are now free" << endl;
		// assert(!blocked); assertion must not hold in general if variable is first blocked because of A then modified, then the block from A releases and THEN the modification takes place
		blocked = false;
		for (auto nvar : topology_children) {
			nvar->free();
		}
		return true;
	}
	//	else if (DEBUG) cout << str() << " trequires still " << missing_depending_applicable_op_count << endl;
	return false;
}

void NumericStateVariable::cleanup_and_init(ap_float value) {
	init = Interval(value);
	achievers.clear();
	missing_depending_applicable_op_count = 0;
	blocked = false;
}

void NumericStateVariable::achieved_by(NumericAchiever achiever) {
	assert(achievers.empty() || achiever.reached_interval.extends(achievers.back().reached_interval));
	achievers.push_back(achiever);
}

//std::vector<NumericAchiever>& NumericStateVariable::get_achievers() {
//	return achievers;
//}

void NumericStateVariable::depends_on(NumericStateVariable* other) {
	topology_parents.insert(other);
	other->topology_children.insert(this);
}

int NumericStateVariable::get_id() {
	return id;
}

std::set<NumericStateVariable*>& NumericStateVariable::get_depending_variables() {
	return topology_children;
}

std::set<NumericStateVariable*>& NumericStateVariable::get_topology_parents() {
	return topology_parents;
}


void NumericStateVariable::set_topology(int level) {
	topology_level = level;
}

int NumericStateVariable::get_topology() {
	return topology_level;
}

Interval NumericStateVariable::val_at_cost(ap_float cost) {
	for (auto it = achievers.rbegin(); it !=achievers.rend(); ++it) {
			if ((*it).cost <= cost)
				return (*it).reached_interval;
		}
		return init;
}

Interval NumericStateVariable::val_at_id(int id) {
	for (auto it = achievers.rbegin(); it !=achievers.rend(); ++it) {
		if ((*it).order_pos <= id)
			return (*it).reached_interval;
	}
	return init;
}

ap_float NumericStateVariable::cost() {
	if (achievers.size() > 0)
		return achievers.back().cost;
	return 0;
}

Interval NumericStateVariable::val() {
	if (achievers.size() > 0)
		return achievers.back().reached_interval;
	return init;
}

Interval NumericStateVariable::max_val() {
	if (max_iv.extends(val()))
		return max_iv;
	return val();
}

int NumericStateVariable::index_at_id(int id) {
//	cout << "there are " << achievers.size() << " achievers, have to find the one with id <=" << id << endl;
	for (int i=achievers.size()-1; i >= 0; --i) {
		if (achievers[i].order_pos <= id)
			return i;
	}
	return -1;
}

NumericAchiever* NumericStateVariable::get_achiever(int index) {
	if (index >= 0 && index < (int) achievers.size())
		return &achievers[index];
	else
		return 0;
}

NumericAchiever* NumericStateVariable::get_best_achiever(ap_float target) {
	if (init.contains(target))
		return 0;
	for (auto &achiever : achievers) {
		if (achiever.reached_interval.contains(target))
			return &achiever;
	}
	assert(false); // no achiever contained targetvalue
	return 0;
}

std::string NumericStateVariable::str() {
	return g_numeric_var_names[id];
}

void RepetitionRelaxationHeuristic::build_unary_comparison_axioms(
		const ComparisonAxiomProxy& ax) {
	//	if (DEBUG) cout << "Building unary Operator for Comparison Axiom #"<< ax.get_id() << endl;
	vector<NumericStateVariable *> left_right = vector<NumericStateVariable *>();
	left_right.push_back(&numeric_variables[ax.get_left_variable().get_id()]);
	left_right.push_back(&numeric_variables[ax.get_right_variable().get_id()]);
	FactProxy truefact = ax.get_true_fact();
	UnaryEffect axiom_triggers = UnaryEffect(truefact.get_variable().get_id(), truefact.get_value());
	comparison_axioms.push_back(UnaryOperator(ax.get_id(), left_right, ax.get_comparison_operator_type(), axiom_triggers));

	// TODO: reverse axioms do not appear in preconditions so they do not have to be implemented
	//	comp_operator reverseop = get_inverse_op(ax.get_comparison_operator_type());
	//	FactProxy falsefact = ax.get_false_fact();
	//	assert(truefact.get_variable().get_id() == falsefact.get_variable().get_id());
	//	SingularyEffect axiom_does_not_trigger = SingularyEffect(falsefact.get_variable().get_id(), falsefact.get_value());
	//	comparison_axioms.push_back(SingularyOperator(ax.get_id(), left_right, reverseop, axiom_does_not_trigger));
}

void RepetitionRelaxationHeuristic::build_unary_assignment_axioms(
		const AssignmentAxiomProxy& ax) {
	//	if (DEBUG) cout << "Building unary Operator for Assignment Axiom #"<< ax.get_id() << endl;
	vector<NumericStateVariable *> left_right = vector<NumericStateVariable *>();
	left_right.push_back(&numeric_variables[ax.get_left_variable().get_id()]);
	left_right.push_back(&numeric_variables[ax.get_right_variable().get_id()]);
	UnaryEffect ax_eff = UnaryEffect(ax.get_assignment_variable().get_id(), assign, ax.get_assignment_variable().get_id());
	ass_axioms.push_back(UnaryOperator(ax.get_id(), left_right, ax.get_arithmetic_operator_type(), ax_eff));
}

void RepetitionRelaxationHeuristic::build_dummy_operators() {
	for (int i=0; i < (int) prop_variables.size(); ++i)
		for (int j=0; j < (int) prop_variables[i].size(); ++j) {
			assert(prop_variables[i][j].id == (int) prop_var_dummies.size());
			assert(prop_variables[i][j].id == linearized_prop_variables[prop_variables[i][j].id]->id);
			prop_var_dummies.push_back(UnaryOperator(UnaryEffect(i,j)));
		}
	//	for (int i=0; i < (int) numeric_variables.size(); ++i) {
	//		assert(numeric_variables[i].get_id() == (int) num_var_dummies.size());
	//		num_var_dummies.push_back(SingularyOperator(SingularyEffect(i, assign, i)));
	//	}
}

bool RepetitionRelaxationHeuristic::enqueue_if_necessary(ap_float cost,
		UnaryOperator* op, bool max_instead_of_val) {
	if(DEBUG) cout << " -- Enqueueing " << op->str() << " if necessary" << endl;
	assert(cost >= 0);
	assert(op);
	UnaryEffect* effect = &op->effect;
	if (effect->numeric) {
		NumericStateVariable *aff_pointer = &numeric_variables[op->effect.aff_variable_index];
		if (op->op_type == dummy) {
			aff_pointer = &numeric_cycle_breaker_vars[cycle_breaker_index[op->effect.aff_variable_index]];
		}
		NumericStateVariable &affected_var = (*aff_pointer); // get reference from pointer

		Interval result = repetition_result(op, global_achiever_ordering_rank, max_instead_of_val);
		if (result.extends(affected_var.val())) {
			if(DEBUG) cout << " -- Operator " << op->str() << " extends " << g_numeric_var_names[op->effect.aff_variable_index] << " from "
					<< affected_var.val() << " to " << result << " with cost " << cost << " -> push to queue" << endl;
			// increase missing "topology preconditions"
			if(op->op_type == numeric_op) affected_var.tblock();
			queue.push(cost, ExploredOperator(op, global_achiever_ordering_rank));
			return true;
		} else return false;
	} else { // enqueue propositional effect
		if (op->op_type == comp_axiom) {
			assert(op->axiom_left_right.size() == 2);
			//			cout << "Comp Axiom, comparing ";
			//			cout << op->axiom_left_right[0]->str() << " " << op->axiom_left_right[0]->val << " ";
			//			cout << op->comp_ax_op << " ";
			//			cout << op->axiom_left_right[1]->str() << " " << op->axiom_left_right[1]->val << endl;
			bool result = relaxed_compare(op->axiom_left_right[0]->val(), op->comp_ax_op, op->axiom_left_right[1]->val());
			if (!result) {
				//				cout << " Axiom does not trigger " << endl;
				return false;
			}
			else {
				//				cout << " Axiom triggers" << endl;
			}
		}
		IProposition* eff_prop = &prop_variables[op->effect.aff_variable_index][op->effect.val_or_ass_var_index];
		assert(eff_prop);
		if (eff_prop->cost > cost) {
			eff_prop->cost = cost; // we can set the cost already for propositional variables but not for numeric variables because we have to
			// ensure that the intervals are monotonically nondecreasing for each cost
			eff_prop->reached_by = op;
			if (op->op_type == comp_axiom)
				eff_prop->exploration_index = global_achiever_ordering_rank;
			queue.push(cost, ExploredOperator(op, global_achiever_ordering_rank));
			//			if (DEBUG) cout << " -- "<< op->str() << " enables prop " << linearized_fact_names[eff_prop->id] << " with cost " << cost << endl;
			return true;
		} else return false;
	}
}

void RepetitionRelaxationHeuristic::setup_exploration_queue(const State& state) {
	queue.clear();
	for (auto prop : linearized_prop_variables) {
		assert(prop);
		prop->cost = INF;
		prop->marked = false;
		prop->reached_by = 0;
		prop->exploration_index = -1;
	}

	// enqueue all initially true propositions
	for (FactProxy fact : state) {
		//    	if(DEBUG) cout << fact.get_variable().get_name() << " maps initially to " << fact.get_name() << endl;
		//    	if(DEBUG) cout << g_variable_name[fact.get_variable().get_id()] << " <- g_varname .... g_factname ->" << g_fact_names[fact.get_variable().get_id()][fact.get_value()] << endl;
		IProposition *init_prop = get_proposition(fact);
		assert(init_prop);
		init_prop->cost = 0;
		//        if(DEBUG) cout << "Proposition " << linearized_fact_names[init_prop->id] << " is initially true -> enqueue" << endl;
		//        assert(g_fact_names[fact.get_variable().get_id()][fact.get_value()] == linearized_fact_names[init_prop->id]);
		queue.push(0, ExploredOperator(&prop_var_dummies[init_prop->id],0));
	}

	for (auto &nvar: numeric_variables) {
		nvar.cleanup_and_init(state.nval(nvar.get_id()));
		//    	if (DEBUG) cout << "Numeric variable " << g_numeric_var_names[nvar.get_id()] << " is initially " << nvar.val << endl;
	}
	for (auto &nvar: numeric_cycle_breaker_vars) {
		nvar.cleanup_and_init(state.nval(nvar.get_id()));
		//    	if (DEBUG) cout << "Numeric variable " << g_numeric_var_names[nvar.get_id()] << " is initially " << nvar.val << endl;
	}


	// Deal with operators and axioms without preconditions.
	for (UnaryOperator *op : all_operators) {
		op->unsatisfied_preconditions = op->precondition.size();
		op->precondition_cost = 0;  // will be increased by precondition costs

		if (op->unsatisfied_preconditions == 0 && (op->op_type == logic_op || op->op_type == numeric_op)) {
			//			if (DEBUG) cout << "Operator applicable in initial state: " << op->str() << " -> enqueue with cost " << op->base_cost << endl;
			enqueue_if_necessary(op->base_cost, op, 0);
		}
	}
}


// max_instead_of_val (default: false) is a switch used for h_max that uses the maximally attainable values of each variable
// instead of the value in the current layer.
// This is required for h_max to remain admissible while also being polynomial with variable dependencies
// of the topology.
Interval RepetitionRelaxationHeuristic::repetition_result(UnaryOperator* op, int exploration_time, bool max_instead_of_val) {
	assert(op);
	switch(op->op_type) {
	case logic_op:
		assert(false);
		return Interval();
	case numeric_op: {
		Interval assignment = numeric_variables[op->effect.val_or_ass_var_index].val_at_id(exploration_time);
		if (max_instead_of_val) assignment = numeric_variables[op->effect.val_or_ass_var_index].max_val();
		Interval altered = numeric_variables[op->effect.aff_variable_index].val_at_id(exploration_time);
		return repeat_apply(altered,op->effect.assign_type, assignment);
	}
	case comp_axiom:
		assert(false);
		return Interval();
	case ass_axiom: {
		assert (op->axiom_left_right[0]);
		assert (op->axiom_left_right[1]);
		Interval left = op->axiom_left_right[0]->val_at_id(exploration_time);
		Interval right = op->axiom_left_right[1]->val_at_id(exploration_time);
		if (max_instead_of_val) {
			left = op->axiom_left_right[0]->max_val();
			right = op->axiom_left_right[1]->max_val();
		}
		//		if(DEBUG) {
		//			cout << " axiom left (id "<< op->axiom_left_right[0]->get_id() <<")= \n";
		//			op->axiom_left_right[0]->dump();
		//			cout << " axiom right = \n";
		//			op->axiom_left_right[1]->dump();
		//			cout << "repetition_result of axiom computes "<< op->axiom_left_right[0]->val << " " << op->ass_ax_op << " " << op->axiom_left_right[1]->val << endl;
		//			cout << "0 * inf = ";
		//			cout << (INF * 0) << endl;
		//		}
		return compute(left, op->ass_ax_op, right);
	}
	case dummy:
	{
		//			cout << "Determining repetion result of " << op->str() << endl;
		NumericStateVariable &ori_var = numeric_variables[op->effect.aff_variable_index];
		NumericStateVariable &breaker_var = numeric_cycle_breaker_vars[cycle_breaker_index[op->effect.aff_variable_index]];
		Interval new_value = breaker_var.val_at_id(exploration_time);
		Interval orival = ori_var.val_at_id(exploration_time);
		if (DEBUG) cout << "Repetition result: breaker var " << new_value.precise_str() << endl;
		if (DEBUG) cout << "Repetition result: originl var " << orival.precise_str() << endl;

		assert(breaker_var.get_id() == ori_var.get_id());
		//			cout << "ori = " << ori_var.str() << " val " << ori_var.val.precise_str() << endl;
		//			cout << "breaker = " << breaker_var.str() << " val " << breaker_var.val.precise_str() << endl;
		//TODO: hack! by rounding floats the breaker var can extend its bound even though no operator enabled a new value.
		// Therefore, we require a change of at least EPSILON, hoping that 1. rounding errors do not surpass EPSILON and
		// 2. that "real" changeds DO surpass EPSILON
		if (orival.left < new_value.left - EPSILON) {
			//				 cout << "lower bound extended -> setting it to -infinity" << endl;
			new_value.left = -INF;
			new_value.left_open = true;
		}
		if (orival.right > new_value.right + EPSILON) {
			//							 cout << "upper bound extended -> setting it to +infinity" << endl;
			new_value.right = INF;
			new_value.right_open = true;
		}
		if(DEBUG) cout << "Repeated application with dummy operator yields " << new_value.precise_str() << endl;
		return new_value;
	}
	default:
		assert(false);
		return Interval();
	}
}

//bool interval_contains(const Interval& interval, ap_float target) {
////	cout << "checking if " << interval << " contains " << target << endl;
//	return (
//		(interval.defined()) &&
//		(target > interval.left || (target == interval.left && !interval.left_open)) &&
//		(target < interval.right || (target == interval.right && !interval.right_open)));
//}

bool UnaryOperator::is_axiom() const {
	return (op_type == comp_axiom || op_type == ass_axiom || (op_type == logic_op && operator_no == -1));
}

/**
 * max_iv_relaxation (default: false)
 * uses the "max_val"s instead of the "val"s of numeric variables. This flag is used by h_max
 *
 * ignore_topology (default: false)
 * enqueues variables even though there are topologically higher variables in the queue already this ensures
 * admissibility but impairs polynomiality and should therefore only be used in conjunction with max_iv_relaxation
 * because then, each effect can be successfully enqueued only once and as such, the exponential blowup is prevented
 *
 */
void RepetitionRelaxationHeuristic::relaxed_exploration(bool reachability_only, bool ignore_topology) {
	int unsolved_goals = goal_propositions.size();
	if (DEBUG) cout << "Starting relaxed exploration... " << endl;

	if (reachability_only) {
		if (DEBUG) cout << "Relaxed exploration is only performed to determine reachable intervals; creating artificial goal to prevent termination" << endl;
		++unsolved_goals; // If we ignore the topological ordering, we want to determine
		// the maximally reachable intervals and do not want to stop search if all goals are satisfied
	}

	while (!queue.empty()) {
		pair<ap_float, ExploredOperator> top_pair = queue.pop();
		ap_float distance = top_pair.first;
		UnaryOperator* current_op = top_pair.second.op;
		int exploration_id = top_pair.second.exploration_id;
		UnaryEffect &queue_eff = current_op->effect;
		if (DEBUG) cout << "Pop " << current_op->str() << " at distance " << distance << endl;
		// Numeric Queue Variable
		if(queue_eff.numeric) {
			NumericStateVariable *nvar_pointer = &numeric_variables[queue_eff.aff_variable_index];
			if (current_op->op_type == operatorType::dummy) {
				nvar_pointer = &numeric_cycle_breaker_vars[cycle_breaker_index[nvar_pointer->get_id()]];
				if(DEBUG) cout << "nvar is dummy ";
			}
			NumericStateVariable &nvar = (*nvar_pointer);
			//			NumericStateVariable &effvar = numeric_variables[queue_eff.val_or_ass_var_index];
			if(DEBUG) cout << "popping " << nvar.str() << " cost = " << distance;
			bool enqueue_children = false;
			if (current_op->op_type == operatorType::numeric_op) {
				enqueue_children = nvar.tfree(); // returns true if the variable is now maximally extended
				if (!ignore_topology && nvar.is_blocked()) {
					if (DEBUG) cout << " topology delay, continue" << endl;
					assert(!enqueue_children);
					continue; // nvar still misses topological predecessors
				}
			}
			if(ignore_topology) {
				enqueue_children = false; // don't have to enqueue successors just because of topology blocking
			}
			Interval result = repetition_result(current_op, exploration_id, ignore_topology);
			if(result.extends(nvar.val())) {
				if (DEBUG) cout << " " << nvar.str() << " extended bounds from " << nvar.val() << " to " << result
						<< " with cost " << distance << endl;

				// store operator as new achiever
				nvar.achieved_by(NumericAchiever(current_op, result || nvar.val(), exploration_id, 0, distance));
				enqueue_children = true;
			}
			else if (DEBUG) cout << " no extension " << endl;
			if (enqueue_children) {
				for (UnaryOperator *tr_op : nvar.precondition_of) {
					if(DEBUG) cout << "    Testing triggered operator " << tr_op->str() << endl;
					if (tr_op->unsatisfied_preconditions > 0) {
						if (DEBUG) cout << "misses " << tr_op->unsatisfied_preconditions << " preconditions" << endl;
						continue;
					}
					ap_float explicit_and_implicit_precondition_cost = update_cost(tr_op->assignment_cost, tr_op->precondition_cost);
					ap_float cost_to_achieve_new_value = tr_op->base_cost + update_cost(explicit_and_implicit_precondition_cost, nvar.cost());
					if(DEBUG) cout << "New value " << result << " can be achieved for " << cost_to_achieve_new_value << endl;
					assert(tr_op->op_type == operatorType::ass_axiom || tr_op->op_type == operatorType::comp_axiom || tr_op->op_type == operatorType::numeric_op || tr_op->op_type == operatorType::dummy);
					if (tr_op->op_type == operatorType::comp_axiom || tr_op->op_type == operatorType::ass_axiom)
						cost_to_achieve_new_value = update_cost(tr_op->axiom_left_right[0]->cost(), tr_op->axiom_left_right[1]->cost());
					bool enqueue_successful = enqueue_if_necessary(cost_to_achieve_new_value, tr_op);
					if (enqueue_successful && (tr_op->op_type == operatorType::numeric_op || tr_op->op_type == operatorType::dummy)) {
						if (DEBUG) cout << "Successfully enqueued " << tr_op->str() << " at distance " << cost_to_achieve_new_value << endl;
						tr_op->assignment_cost = nvar.cost();
					} else if (DEBUG) {
						if (enqueue_successful)
							cout << "Enqueued axiom" << endl;
						else
							cout << " do not enqueue - no effect" << endl;
					}
				}
			}
			else if (DEBUG) cout << " do not enqueue - forbidden (topology)" << endl;

		} else { // propositional queue variable
			IProposition* prop = &prop_variables[queue_eff.aff_variable_index][queue_eff.val_or_ass_var_index];
			assert(prop);
			ap_float prop_cost = prop->cost;
			assert(prop_cost >= 0);
			assert(prop_cost <= distance);
			if (prop_cost < distance)
				continue;
			if (prop->is_goal && --unsolved_goals == 0) {
				if (DEBUG) cout << "I am done, just popped goal " << prop->id;
				return;
			}
			for (UnaryOperator *unary_op : prop->precondition_of) {
				unary_op->precondition_cost = update_cost(unary_op->precondition_cost, prop_cost);
				--unary_op->unsatisfied_preconditions;
				//				if (unary_op->unsatisfied_preconditions < 0) {
				//					cout << "Assertion will fail now. for operator #" << unary_op->operator_no << " " << unary_op->str() << endl;
				//				}
				assert(unary_op->unsatisfied_preconditions >= 0);
				if (unary_op->unsatisfied_preconditions == 0)
					enqueue_if_necessary(unary_op->cost(), unary_op);
				//				else if (DEBUG) cout << "Operator " << unary_op->str() << " is missing " << unary_op->unsatisfied_preconditions << " preconditions" << endl;
			}
		}
	}
}

}
