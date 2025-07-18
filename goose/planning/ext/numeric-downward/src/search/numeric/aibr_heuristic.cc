#include "aibr_heuristic.h"

#include <cmath>
#include <iostream>

#include "relaxed_interval_helper.h"
#include "../option_parser.h"
#include "../plugin.h"

using namespace std;

namespace aibr_heuristic {

AIBRHeuristic::AIBRHeuristic(const options::Options& options)
 : AdditiveIntervalBasedRelaxation(options) {}

AIBRHeuristic::~AIBRHeuristic() {}

void AIBRHeuristic::initialize() {
	if(DEBUG) std::cout << "Initializing AIBR heuristic..." << std::endl;
	AdditiveIntervalBasedRelaxation::initialize();
	applicable_operator_to_unary_operator.resize(task_proxy.get_operators().size());
}

ap_float AIBRHeuristic::compute_aibr_estimate() {
	ap_float h = 0;
	int unsolved_goals = goal_propositions.size();
	for (auto goal : goal_propositions)
		if (goal->reached_in_layer == 0) --unsolved_goals; // initially solved goals
	NumericVariablesProxy numeric_vars = task_proxy.get_numeric_variables();
	while (unsolved_goals > 0) {
		for (auto op : applicable_operators) {
			if (numeric_vars[op->effect.aff_variable_index].get_var_type() != instrumentation)
				applicable_operator_to_unary_operator[op->operator_no].push_back(op);
		}
		applicable_operators.clear();
		for (auto unary_ops: applicable_operator_to_unary_operator) {
			NumericState nextState = planning_graph.back().duplicate();
			auto it = unary_ops.begin();
			bool change = false;
			while (it != unary_ops.end()) {
				change = true;
				auto op = (*it);
				if (op->is_numeric_operator()) {
					Interval oldval = planning_graph.back().get_val(op->effect.aff_variable_index);
					ap_float aff_cost = planning_graph.back().get_cost(op->effect.aff_variable_index);
					Interval ass_val =  planning_graph.back().get_val(op->effect.val_or_ass_var_index);
					ap_float ass_cost = planning_graph.back().get_cost(op->effect.val_or_ass_var_index);
					Interval newval = compute(oldval, op->effect.assign_type, ass_val);
					ap_float cost = update_cost(aff_cost, ass_cost);
					cost += op->base_cost;
					cost = update_cost(cost, op->precondition_cost); 
					nextState.new_val_for(op->effect.aff_variable_index, newval, op, cost);
					++it;
					if (!change && nextState.get_val(op->effect.aff_variable_index).extends(oldval)) {
						change = true;
					}
				} else {
					Proposition *prop = &propositions[op->effect.aff_variable_index][op->effect.val_or_ass_var_index];
					handle_prop(prop, op->cost(), planning_graph.size(), op, unsolved_goals);
					it = unary_ops.erase(it);
					change = true;
				}
			}
			if (change) {
				for (size_t i = 0; i < numeric_axioms.size(); ++i) {
					auto ax = &numeric_axioms[i];
					if (ax->is_assignment_axiom()) {
						Interval leftval = planning_graph.back().get_val(ax->axiom_left_var);
						Interval rightval = planning_graph.back().get_val(ax->axiom_right_var);
						ap_float leftcost = planning_graph.back().get_cost(ax->axiom_left_var);
						ap_float rightcost = planning_graph.back().get_cost(ax->axiom_right_var);

						Interval newval = compute(leftval, ax->ass_ax_op, rightval);
						ap_float cost = max(leftcost, rightcost);
						nextState.new_val_for(ax->effect.aff_variable_index, newval, ax, cost);
					} else {
						Interval leftval = planning_graph.back().get_val(ax->axiom_left_var);
						Interval rightval = planning_graph.back().get_val(ax->axiom_right_var);
						ap_float leftcost = planning_graph.back().get_cost(ax->axiom_left_var);
						ap_float rightcost = planning_graph.back().get_cost(ax->axiom_right_var);
						bool result = relaxed_compare(leftval, ax->comp_ax_op, rightval);
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
				++h;
				planning_graph.push_back(nextState);

				if (unsolved_goals == 0) return h;
			}
		}
	}

	return h;
}

ap_float AIBRHeuristic::compute_heuristic(const GlobalState &global_state) {
	State state = convert_global_state(global_state);
	setup_exploration(state);
	assert (planning_graph.size() == 1);
	bool reachable = relaxed_exploration();
	//std::cout << "1 layers: " << planning_graph.size() << std::endl;
	if (!reachable) return DEAD_END;

	setup_exploration(state);
	ap_float h = compute_aibr_estimate();
	//std::cout << "2 layers: " << planning_graph.size() << std::endl;
	return h;
}

std::vector<NumericState> AIBRHeuristic::get_relaxed_reachable_states(const GlobalState &global_state) {
	State state = convert_global_state(global_state);
	setup_exploration(state);
	assert (planning_graph.size() == 1);
	bool reachable = relaxed_exploration();
	if (!reachable) return std::vector<NumericState>();

	NumericState reachable_goal = planning_graph.back();

	setup_exploration(state);
	compute_aibr_estimate();

	std::vector<NumericState> reachable_states = planning_graph;
	reachable_states.push_back(reachable_goal);

	return reachable_states;
}

static Heuristic *_parse(OptionParser &parser) {
		parser.document_synopsis("Additive Interval-Based Relaxation heuristic", "");
		parser.document_language_support("action costs", "supported");
		parser.document_language_support("conditional effects", "supported");
		parser.document_language_support("numeric", "supported");
		parser.document_language_support(
				"axioms",
				"supported (in the sense that the planner won't complain -- "
				"handling of axioms might be very stupid "
				"and even render the heuristic unsafe)");
		parser.document_property("admissible", "no");
		parser.document_property("consistent", "no");
		parser.document_property("safe", "yes tasks without axioms");
		parser.document_property("preferred operators", "no");

		Heuristic::add_options_to_parser(parser);
		Options opts = parser.parse();

		if (parser.dry_run())
				return 0;
		else
				return new AIBRHeuristic(opts);
}

static Plugin<Heuristic> _plugin("aibr", _parse);

}