#include "additive_interval_based_relaxation.h"

#include <stddef.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "additive_transformation_helper.h"
#include "relaxed_interval_helper.h"
#include "../global_operator.h"
#include "../globals.h"
#include "../options/options.h"
#include "../task_proxy.h"
#include "../utils/collections.h"

using namespace std;
using namespace additive_transformation_helper;

namespace additive_interval_based_relaxation {

AdditiveIntervalBasedRelaxation::AdditiveIntervalBasedRelaxation(
		const options::Options& options)
	: Heuristic(options) {
}

AdditiveIntervalBasedRelaxation::~AdditiveIntervalBasedRelaxation() {
}

// initialization
void AdditiveIntervalBasedRelaxation::initialize() {
		// Build propositions.
		int prop_id = 0;
		VariablesProxy variables = task_proxy.get_variables();
		propositions.resize(variables.size());
		int old_var_id = 0;
		for (FactProxy fact : variables.get_facts()) {
			if (fact.get_variable().get_id() != old_var_id) {
				old_var_id = fact.get_variable().get_id();
			}
			propositions[fact.get_variable().get_id()].push_back(Proposition(prop_id++));
		}
		num_propositions = prop_id;
		// Build goal propositions.
		for (FactProxy goal : task_proxy.get_goals()) {
				Proposition *prop = get_proposition(goal);
				prop->is_goal = true;
				goal_propositions.push_back(prop);
		}

		// Build unary operators for operators and axioms.
//    cout << "Building " << g_operators.size() << " unary operators" << endl;
		int op_no = 0;

		for (OperatorProxy op : task_proxy.get_operators())
				build_unary_operators(op);
//    cout << "Building " << g_axioms_as_operator.size() << " unary axioms" << endl;
		for (OperatorProxy axiom : task_proxy.get_axioms())
				build_unary_axioms(axiom, -1);
		for (AssignmentAxiomProxy aax : task_proxy.get_assignment_axioms())
			build_unary_assignment_axioms(aax);
		for (ComparisonAxiomProxy cax : task_proxy.get_comparison_axioms())
			build_unary_comparison_axioms(cax);


		// Cross-reference unary operators.
		for (size_t i = 0; i < unary_operators.size(); ++i) {
				UnaryOperator *op = &unary_operators[i];
				for (size_t j = 0; j < op->precondition.size(); ++j)
						op->precondition[j]->precondition_of.push_back(op);
		}
		for (size_t i = 0; i < unary_axioms.size(); ++i) {
				UnaryOperator *op = &unary_axioms[i];
				for (size_t j = 0; j < op->precondition.size(); ++j)
						op->precondition[j]->precondition_of.push_back(op);
		}
}

bool AdditiveIntervalBasedRelaxation::trigger_supporter(
	UnaryOperator *op, const NumericState &oldState, NumericState &newState) {		
	Interval oldval = oldState.get_val(op->effect.aff_variable_index);
  Interval ass_val =  oldState.get_val(op->effect.val_or_ass_var_index);
  ap_float aff_cost = oldState.get_cost(op->effect.aff_variable_index);
  ap_float ass_cost = oldState.get_cost(op->effect.val_or_ass_var_index);
  ap_float cost = update_cost(aff_cost, ass_cost);
  cost += op->base_cost;
  cost = update_cost(cost, op->precondition_cost);

	NumericVariablesProxy variables = task_proxy.get_numeric_variables();
	NumericVariableProxy assgin_variable = variables[op->effect.val_or_ass_var_index];

	if (op->effect.assign_type == assign && assgin_variable.get_var_type() == constant) {
		Interval newval = oldval || ass_val;
		newState.new_val_for(op->effect.aff_variable_index, newval, op, cost);
		return true;
	} else  {
		Interval newval = compute_additive_asymptotic_behavior(oldval, op->effect.assign_type, ass_val);
		newState.new_val_for(op->effect.aff_variable_index, newval, op, cost);
		return newval.right == INF && newval.left == -INF;
	} 
}

void AdditiveIntervalBasedRelaxation::setup_exploration(const State& state) {
//	cout << "Preparing heuristic for next calculation " << endl;
	planning_graph.clear();
	applicable_operators.clear();
	applicable_axioms.clear();

	for (vector<Proposition> &props_of_var : propositions) {
			for (Proposition &prop : props_of_var) {
					prop.cost = INF;
					prop.marked = false;
					prop.reached_by = 0;
					prop.reached_in_layer = -1;
			}
	}

	for (auto &op : unary_operators) {
		op.unsatisfied_preconditions = op.precondition.size();
		op.precondition_cost = 0;
	}
	for (auto &op : unary_axioms) {
		op.unsatisfied_preconditions = op.precondition.size();
		op.precondition_cost = 0;
	}

	for (size_t i = 0; i < unary_operators.size(); ++i) {
		if (unary_operators[i].unsatisfied_preconditions == 0) {
//			cout << "Operator without (missing) precons" << unary_operators[i].str() << endl;
			applicable_operators.push_back(&unary_operators[i]);
		}
	}

	// enqueue all initially true propositions
	for (FactProxy fact : state) {
		Proposition *init_prop = get_proposition(fact);
			assert(init_prop);
//      cout << "handling initially true prop " << debug_fact_names[init_prop->id] << endl;
			int dummy;
			handle_prop(init_prop, 0, 0, 0, dummy);
	}

	for (size_t i = 0; i < unary_axioms.size(); ++i) {
		if (unary_axioms[i].unsatisfied_preconditions == 0) {
//			cout << "Axiom without (missing) precons" << unary_axioms[i].str() << endl;
			applicable_axioms.push_back(&unary_axioms[i]);
		}
	}

	NumericState nstate = NumericState(state);
	planning_graph.push_back(nstate);
}

bool AdditiveIntervalBasedRelaxation::relaxed_exploration(bool reachability) {
	int unsolved_goals = goal_propositions.size();
	for (auto goal : goal_propositions)
		if (goal->reached_in_layer == 0) --unsolved_goals; // initially solved goals
	while ((reachability || unsolved_goals > 0) && !applicable_operators.empty()) {
		NumericState nextState = planning_graph.back().duplicate();
		auto it = applicable_operators.begin();
		while (it != applicable_operators.end()) {
			auto op = (*it);
			if (op->is_numeric_operator()) {
				if (trigger_supporter(op, planning_graph.back(), nextState))
					it = applicable_operators.erase(it);
				else
					++it;
			} else {
				Proposition *prop = &propositions[op->effect.aff_variable_index][op->effect.val_or_ass_var_index];
				handle_prop(prop, op->cost(), planning_graph.size(), op, unsolved_goals);
				it = applicable_operators.erase(it);
			}
		}
		for (size_t i = 0; i < numeric_axioms.size(); ++i) {
			auto ax = &numeric_axioms[i];
			if (ax->is_assignment_axiom()) {
				Interval leftval = planning_graph.back().get_val(ax->axiom_left_var);
				Interval rightval = planning_graph.back().get_val(ax->axiom_right_var);
				ap_float leftcost = planning_graph.back().get_cost(ax->axiom_left_var);
				ap_float rightcost = planning_graph.back().get_cost(ax->axiom_right_var);

				Interval newval = compute_asymptotic_behavior(leftval, ax->ass_ax_op, rightval);
				ap_float cost = max(leftcost, rightcost);
				nextState.new_val_for(ax->effect.aff_variable_index, newval, ax, cost);
			} else {
				Interval leftval = planning_graph.back().get_val(ax->axiom_left_var);
				Interval rightval = planning_graph.back().get_val(ax->axiom_right_var);
				ap_float leftcost = planning_graph.back().get_cost(ax->axiom_left_var);
				ap_float rightcost = planning_graph.back().get_cost(ax->axiom_right_var);
				bool result = relaxed_compare(leftval, ax->comp_ax_op, rightval);
				//    			cout << leftval << " " << ax.comp_ax_op << " " <<  rightval << " = " << (result?"TRUE":"FALSE") << endl;
				if (result) {
					ap_float cost = max(leftcost, rightcost);
					Proposition *prop = &propositions[ax->effect.aff_variable_index][ax->effect.val_or_ass_var_index];
					handle_prop(prop, cost, planning_graph.size(), ax, unsolved_goals);
				}
			}
		}
		auto it2 = applicable_axioms.begin();
		while (it2 != applicable_axioms.end()) {
			auto ax = (*it2);
			assert (ax->unsatisfied_preconditions == 0);
			Proposition * ax_prop = &propositions[ax->effect.aff_variable_index][ax->effect.val_or_ass_var_index];
			handle_prop(ax_prop, ax->precondition_cost, planning_graph.size(), ax, unsolved_goals);
			it2 = applicable_axioms.erase(it2);
		}
		planning_graph.push_back(nextState);
	}

	return unsolved_goals == 0;
}

Proposition* AdditiveIntervalBasedRelaxation::get_proposition(
		const FactProxy& fact) {
		int var = fact.get_variable().get_id();
		int value = fact.get_value();
		assert(utils::in_bounds(var, propositions));
		assert(utils::in_bounds(value, propositions[var]));
//    cout << " return var " << var << "][" << value << endl;
		return &propositions[var][value];

}

UnaryEffect AdditiveIntervalBasedRelaxation::get_effect(EffectProxy effect) {
	const FactProxy& fact = effect.get_fact();
	return UnaryEffect(fact.get_variable().get_id(), fact.get_value());
}

UnaryEffect AdditiveIntervalBasedRelaxation::get_effect(AssEffectProxy num_effect) {
	const NumAssProxy& assignment = num_effect.get_assignment();
	return UnaryEffect(assignment.get_affected_variable().get_id(),
			assignment.get_assigment_operator_type(),
			assignment.get_assigned_variable().get_id());
}

void AdditiveIntervalBasedRelaxation::handle_prop(Proposition* prop, ap_float distance, int layer, UnaryOperator* achiever, int &missinggoals) {
	if (prop->cost < distance) {
		return;
	}
	bool first_time_achiever = false;
	if (prop->cost == INF){
		first_time_achiever = true;
	}

	prop->reached_in_layer = layer;
	prop->cost = distance;

	if (first_time_achiever) {
		for (UnaryOperator *unary_op : prop->precondition_of) {
			unary_op->precondition_cost = update_cost(unary_op->precondition_cost, distance);
			--unary_op->unsatisfied_preconditions;
			assert(unary_op->unsatisfied_preconditions >= 0);
			if (unary_op->unsatisfied_preconditions == 0) {
				if (unary_op->operator_no < 0)
					applicable_axioms.push_back(unary_op);
				else
					applicable_operators.push_back(unary_op);
			}
		}
		prop->reached_by = achiever;
		if (prop->is_goal) {
			--missinggoals;
		}
	}
}

void AdditiveIntervalBasedRelaxation::build_unary_operators(const OperatorProxy& op) {
	ap_float base_cost = op.get_cost();
	vector<Proposition *> precondition_props;
	for (FactProxy precondition : op.get_preconditions()) {
		precondition_props.push_back(get_proposition(precondition));
	}
	for (EffectProxy effect : op.get_effects()) {
		UnaryEffect seffect = get_effect(effect);
		EffectConditionsProxy eff_conds_p = effect.get_conditions();
		for (FactProxy eff_cond : (EffectConditionsProxy) eff_conds_p) {
			precondition_props.push_back(get_proposition(eff_cond));
		}

		unary_operators.push_back(UnaryOperator(op.get_id(), precondition_props, seffect, base_cost));
		precondition_props.erase(precondition_props.end() - eff_conds_p.size(), precondition_props.end());
	}
	for (AssEffectProxy effect : op.get_ass_effects()) {
		UnaryEffect seffect = get_effect(effect);
		AssEffectConditionsProxy ass_eff_conds_p = effect.get_conditions();
		for (FactProxy eff_cond : (AssEffectConditionsProxy) ass_eff_conds_p) {
			precondition_props.push_back(get_proposition(eff_cond));
		}
		unary_operators.push_back(UnaryOperator(op.get_id(), precondition_props, seffect, base_cost));
		precondition_props.erase(precondition_props.end() - ass_eff_conds_p.size(), precondition_props.end());


		NumericVariableProxy aff_var = effect.get_assignment().get_affected_variable();
		NumericVariableProxy ass_var = effect.get_assignment().get_assigned_variable();
	}
}

void AdditiveIntervalBasedRelaxation::build_unary_axioms(const OperatorProxy& ax,
		int operator_no) {
	ap_float base_cost = ax.get_cost();
	assert(base_cost == 0);
	vector<Proposition *> precondition_props;
		for (FactProxy precondition : ax.get_preconditions()) {
				precondition_props.push_back(get_proposition(precondition));
		}
		for (EffectProxy effect : ax.get_effects()) {
			UnaryEffect seffect = get_effect(effect);
				EffectConditionsProxy eff_conds = effect.get_conditions();
				for (FactProxy eff_cond : (EffectConditionsProxy) eff_conds) {
						precondition_props.push_back(get_proposition(eff_cond));
				}
				unary_axioms.push_back(UnaryOperator(operator_no, precondition_props, seffect, base_cost));
				precondition_props.erase(precondition_props.end() - eff_conds.size(), precondition_props.end());
		}
}

void AdditiveIntervalBasedRelaxation::build_unary_comparison_axioms(
		const ComparisonAxiomProxy& ax) {
	int left = ax.get_left_variable().get_id();
	int right = ax.get_right_variable().get_id();
	comp_operator comp = ax.get_comparison_operator_type();
	int eff_var = ax.get_true_fact().get_variable().get_id();
	int eff_val = ax.get_true_fact().get_value();
	UnaryOperator axiom = UnaryOperator(left, right, comp, eff_var, eff_val);
	numeric_axioms.push_back(axiom);
}

void AdditiveIntervalBasedRelaxation::build_unary_assignment_axioms(
		const AssignmentAxiomProxy& ax) {
	int left = ax.get_left_variable().get_id();
	int right = ax.get_right_variable().get_id();
	cal_operator ax_op = ax.get_arithmetic_operator_type();
	int eff = ax.get_assignment_variable().get_id();
	UnaryOperator axiom = UnaryOperator(left,right,ax_op,eff);
	numeric_axioms.push_back(axiom);
}

}

