#include "interval_relaxation_heuristic.h"

#include <stddef.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "relaxed_interval_helper.h"
#include "../global_operator.h"
#include "../globals.h"
#include "../options/options.h"
#include "../task_proxy.h"
#include "../utils/collections.h"

using namespace std;

namespace interval_relaxation_heuristic {

IntervalRelaxationHeuristic::IntervalRelaxationHeuristic(
		const options::Options& options)
	: Heuristic(options) {
}

IntervalRelaxationHeuristic::~IntervalRelaxationHeuristic() {
}

void IntervalRelaxationHeuristic::build_unary_operators(const OperatorProxy& op,
		int operator_no) {

//	cout << "Building unary Operators for Operator "<< op.get_name() << endl;
	ap_float base_cost = op.get_cost();
//	cout << "Operator "<< op.get_name() << " costs " << base_cost << endl;
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

		unary_operators.push_back(UnaryOperator(operator_no, precondition_props, seffect, base_cost));
		//        if(DEBUG) cout << "  " << classic_operators.back().str();
		precondition_props.erase(precondition_props.end() - eff_conds_p.size(), precondition_props.end());
	}
	for (AssEffectProxy effect : op.get_ass_effects()) {
		UnaryEffect seffect = get_effect(effect);
		AssEffectConditionsProxy ass_eff_conds_p = effect.get_conditions();
		for (FactProxy eff_cond : (AssEffectConditionsProxy) ass_eff_conds_p) {
			precondition_props.push_back(get_proposition(eff_cond));
		}
		unary_operators.push_back(UnaryOperator(operator_no, precondition_props, seffect, base_cost));
		//        if(DEBUG) cout << "  " << num_operators.back().str();
		precondition_props.erase(precondition_props.end() - ass_eff_conds_p.size(), precondition_props.end());
	}


}

void IntervalRelaxationHeuristic::build_unary_axioms(const OperatorProxy& ax,
		int operator_no) {
//	if (DEBUG) cout << "Building unary Axioms for "<< ax.get_name() << endl;
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

Proposition* IntervalRelaxationHeuristic::get_proposition(
		const FactProxy& fact) {
    int var = fact.get_variable().get_id();
    int value = fact.get_value();
    assert(utils::in_bounds(var, propositions));
    assert(utils::in_bounds(value, propositions[var]));
//    cout << " return var " << var << "][" << value << endl;
    return &propositions[var][value];

}

void IntervalRelaxationHeuristic::setup_exploration(const State& state) {
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

	// enqueue all initially true propositions
    for (FactProxy fact : state) {
    	Proposition *init_prop = get_proposition(fact);
        assert(init_prop);
//        cout << "handling initially true prop " << debug_fact_names[init_prop->id] << endl;
        int dummy;
        handle_prop(init_prop, 0, 0, 0, dummy);
    }

	for (size_t i = 0; i < unary_operators.size(); ++i) {
		if (unary_operators[i].unsatisfied_preconditions == 0) {
//			cout << "Operator without (missing) precons" << unary_operators[i].str() << endl;
			applicable_operators.push_back(&unary_operators[i]);
		}
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

void IntervalRelaxationHeuristic::relaxed_exploration() {
    int unsolved_goals = goal_propositions.size();
    for (auto goal : goal_propositions)
    	if (goal->reached_in_layer == 0) --unsolved_goals; // initially solved goals
    while (unsolved_goals > 0) {
    	NumericState nextState = planning_graph.back().duplicate();
        //cout << "Phase " << planning_graph.size() << ", applying " << applicable_operators.size() << " operators" << endl;
    	auto it = applicable_operators.begin();
    	while (it != applicable_operators.end()) {
    		auto op = (*it);
            //cout << "applying " << op->str() << endl;
    		if (op->is_numeric_operator()) {
    			Interval oldval = planning_graph.back().get_val(op->effect.aff_variable_index);
    			ap_float aff_cost = planning_graph.back().get_cost(op->effect.aff_variable_index);
    			Interval ass_val =  planning_graph.back().get_val(op->effect.val_or_ass_var_index);
    			ap_float ass_cost = planning_graph.back().get_cost(op->effect.val_or_ass_var_index);
    			Interval newval = compute(oldval, op->effect.assign_type, ass_val);
                //cout << oldval << " " << op->effect.assign_type << " " <<  ass_val << " = " << newval << endl;
    			ap_float cost = update_cost(aff_cost, ass_cost);
    			cost += op->base_cost; // implicit numeric precondition cost
    			cost = update_cost(cost, op->precondition_cost); // cost of "regular" preconditions
    			nextState.new_val_for(op->effect.aff_variable_index, newval, op, cost);
    			++it;
    		} else {
    			Proposition *prop = &propositions[op->effect.aff_variable_index][op->effect.val_or_ass_var_index];
    			handle_prop(prop, op->cost(), planning_graph.size(), op, unsolved_goals);
    			it = applicable_operators.erase(it);
    		}
    	}
        //cout << "Phase " << planning_graph.size() << ", done with ops, " << applicable_operators.size() << " left" << endl;
        //cout << "Phase " << planning_graph.size() << ", applying numeric axioms ("<< numeric_axioms.size() <<")" << endl;
    	for (size_t i = 0; i < numeric_axioms.size(); ++i) {
    		auto ax = &numeric_axioms[i];
    		if (ax->is_assignment_axiom()) {
//    			cout << "have to handle ass axiom " << ax.axiom_left_var << ax.ass_ax_op << ax.axiom_right_var << endl;
    			Interval leftval = planning_graph.back().get_val(ax->axiom_left_var);
    			Interval rightval = planning_graph.back().get_val(ax->axiom_right_var);
    			ap_float leftcost = planning_graph.back().get_cost(ax->axiom_left_var);
    			ap_float rightcost = planning_graph.back().get_cost(ax->axiom_right_var);

    			Interval newval = compute(leftval, ax->ass_ax_op, rightval);
//    			cout << leftval << " " << ax.ass_ax_op << " " <<  rightval << " = " << newval << endl;
    			ap_float cost = max(leftcost, rightcost);
    			nextState.new_val_for(ax->effect.aff_variable_index, newval, ax, cost);
//    			cout << "Applied Assignment Axiom, #achievers of variable " << ax->effect.aff_variable_index << " is " << nextState.get_achievers(ax->effect.aff_variable_index).size() << endl;
    		} else {
//    			cout << "have to handle comp axiom " << ax.axiom_left_var << ax.comp_ax_op << ax.axiom_right_var << endl;
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
//    	cout << "Phase " << planning_graph.size() << ", applying propositional axioms (" << applicable_axioms.size()<<")" << endl;
    	auto it2 = applicable_axioms.begin();
    	while (it2 != applicable_axioms.end()) {
    		auto ax = (*it2);
//    		cout << "looking at you axiom " << ax->str() << " missing " << ax->unsatisfied_preconditions << endl;
    		assert (ax->unsatisfied_preconditions == 0);
    		Proposition * ax_prop = &propositions[ax->effect.aff_variable_index][ax->effect.val_or_ass_var_index];
//    		cout << "prop " << ax_prop->id << ": " << g_fact_names[ax->effect.aff_variable_index][ax->effect.val_or_ass_var_index] <<  " was reached in layer " << ax_prop->reached_in_layer << endl;
    		handle_prop(ax_prop, ax->precondition_cost, planning_graph.size(), ax, unsolved_goals);
    		it2 = applicable_axioms.erase(it2);
    	}
//    	cout << "Phase " << planning_graph.size() << ", done propositional axioms ("<< applicable_axioms.size() <<") left" << endl;
//    	cout << "Phase " << planning_graph.size() << " done. Missing goals : " << unsolved_goals << endl;
//    	for (auto goal: goal_propositions) {
//    		cout << "Goal " << debug_fact_names[goal->id] << ((goal->reached_in_layer >= 0)?" achieved":" missing") << endl;
//    		for (auto ax : unary_axioms) {
//    			if (propositions[ax.effect.aff_variable_index][ax.effect.val_or_ass_var_index].id == goal->id) {
//    				cout << "Could be achieved with " << ax.str() << " with preconditions, missing " << ax.unsatisfied_preconditions << endl;
//    				for (auto precon : ax.precondition)
//    					cout << debug_fact_names[precon->id] << ((precon->reached_in_layer >= 0)?" achieved":" missing") << endl;
//
//    			}
//    		}
//    	}
//    	cout << "Phase " << planning_graph.size() << ", relaxed state = " << endl;
//    	nextState.dump();
    	planning_graph.push_back(nextState);
//    	planning_graph.back().dump();
    }
}

bool IntervalRelaxationHeuristic::dead_ends_are_reliable() const {
	return !has_axioms();
}

// initialization
void IntervalRelaxationHeuristic::initialize() {
    // Build propositions.
    int prop_id = 0;
    VariablesProxy variables = task_proxy.get_variables();
    propositions.resize(variables.size());
    int i = 0; //debug only
    int old_var_id = 0;
    debug_fact_names.clear();
    for (FactProxy fact : variables.get_facts()) {
    	if (fact.get_variable().get_id() != old_var_id) {
    		old_var_id = fact.get_variable().get_id();
    		i=0;
    	}
    	debug_fact_names.push_back(g_fact_names[fact.get_variable().get_id()][i++]); //debug
        propositions[fact.get_variable().get_id()].push_back(Proposition(prop_id++));

    }
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
        build_unary_operators(op, op_no++);
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

NumericState::NumericState(const State& state) {
	vals.resize(g_numeric_var_names.size());
	for (size_t i = 0; i < vals.size(); ++i) {
		vals[i]= Interval(state.nval(i));
	}
	achievers.clear();
	achievers.resize(g_numeric_var_names.size(), vector<UnaryOperator *>());
	costs.clear();
	costs.resize(g_numeric_var_names.size(), 0); // state constructor -> initial state
}

NumericState NumericState::duplicate() {
	return NumericState(vals, vector<vector<UnaryOperator *>>(g_numeric_var_names.size(), vector<UnaryOperator *>()), costs);
}

void NumericState::new_val_for(size_t index, Interval new_val,
		UnaryOperator* achiever, ap_float cost) {
	Interval convex_union = vals[index] || new_val;
//	cout << "Update on numeric variable #" << index << " " << vals[index] << " -> " << convex_union << endl;
	if (convex_union.extends(vals[index])) {
		if (achievers[index].size() == 0) {
			costs[index] = cost;
		} else {
			costs[index] = min(costs[index], cost);
		}
		vals[index] = convex_union;
		achievers[index].push_back(achiever);
//		cout << "Numeric variable #" << index << " nv= " << convex_union << " new_cost = " << costs[index] << " achiever = "  << achiever->str() << endl;
	}
}

std::string UnaryOperator::str() {
	stringstream ss;
	if (operator_no == -1) {
		if (is_comparison_axiom()) {
			ss << "ComparisonAxiom on " << effect.str();
		} else if (is_assignment_axiom()) {
			ss << "AssignmentAxiom on " << effect.str();
		} else {
			ss << "LogicAxiom on " << effect.str();
		}
	} else {
		assert((int) g_operators.size() > operator_no);
		ss << "UnaryOp " << g_operators[operator_no].get_name() << " on " << effect.str();
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

UnaryEffect IntervalRelaxationHeuristic::get_effect(EffectProxy effect) {
	const FactProxy& fact = effect.get_fact();
	return UnaryEffect(fact.get_variable().get_id(), fact.get_value());
}

UnaryEffect IntervalRelaxationHeuristic::get_effect(AssEffectProxy num_effect) {
	const NumAssProxy& assignment = num_effect.get_assignment();
	return UnaryEffect(assignment.get_affected_variable().get_id(),
			assignment.get_assigment_operator_type(),
			assignment.get_assigned_variable().get_id());
}

void IntervalRelaxationHeuristic::handle_prop(Proposition* prop, ap_float distance, int layer, UnaryOperator* achiever, int &missinggoals) {
	// the proposition is "dequeued"
//	cout << "I have to handle prop " << debug_fact_names[prop->id] << endl;
	if (prop->cost < distance) {
//		cout << "prop " << prop->id << " already achieved with cost " << prop->cost << " < " << distance << endl;
		return;
	}
	bool first_time_achiever = false;
	if (prop->cost == INF){
		first_time_achiever = true;
//		cout << "prop "<<debug_fact_names[prop->id] <<" was achieved for the first time" << endl;
	}

	prop->reached_in_layer = layer;
	prop->cost = distance;

	if (first_time_achiever) {
		for (UnaryOperator *unary_op : prop->precondition_of) {
			unary_op->precondition_cost = update_cost(unary_op->precondition_cost, distance);
			--unary_op->unsatisfied_preconditions;
//			cout << "Operator " << unary_op->str() << " at " << &unary_op << " is still missing " << unary_op->unsatisfied_preconditions << " / " << unary_op->precondition.size() << " precons " << endl;
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
//			cout << "Achieved a goal!!!" << endl;
			--missinggoals;
		}
	}
//	else {
//		cout << "proposition already achieved before more expensively. Nothing todo in this implementation" << endl;
//	}
}

void NumericState::dump() {
	for (size_t i = 0; i < vals.size(); ++i)
		cout << "v_" << i << " (" << costs[i] << ") = " << vals[i] << " #a= " << achievers[i].size() << "\t " << g_numeric_var_names[i] << endl;
}

void IntervalRelaxationHeuristic::build_unary_comparison_axioms(
		const ComparisonAxiomProxy& ax) {
	int left = ax.get_left_variable().get_id();
	int right = ax.get_right_variable().get_id();
	comp_operator comp = ax.get_comparison_operator_type();
	int eff_var = ax.get_true_fact().get_variable().get_id();
	int eff_val = ax.get_true_fact().get_value();
	UnaryOperator axiom = UnaryOperator(left, right, comp, eff_var, eff_val);
	numeric_axioms.push_back(axiom);
}

void IntervalRelaxationHeuristic::build_unary_assignment_axioms(
		const AssignmentAxiomProxy& ax) {
	int left = ax.get_left_variable().get_id();
	int right = ax.get_right_variable().get_id();
	cal_operator ax_op = ax.get_arithmetic_operator_type();
	int eff = ax.get_assignment_variable().get_id();
	UnaryOperator axiom = UnaryOperator(left,right,ax_op,eff);
	numeric_axioms.push_back(axiom);
}

}
