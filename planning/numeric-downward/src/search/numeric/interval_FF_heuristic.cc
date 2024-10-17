#include "interval_FF_heuristic.h"
#include "interval.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "relaxed_interval_helper.h"

using namespace std;
using namespace interval_relaxation_heuristic;

namespace interval_FF_heuristic {


static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Interval Numeric FFitive heuristic", "");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "supported");
    parser.document_language_support("numeric", "supported");
    parser.document_language_support(
        "axioms",
        "supported (in the sense that the planner won't complain -- "
        "handling of axioms might be very stupid "
        "and even render the heuristic unsafe)");
    parser.document_property("admissible", "yes for tasks without axioms");
    parser.document_property("consistent", "yes for tasks without axioms");
    parser.document_property("safe", "yes for tasks without axioms");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();

    if (parser.dry_run())
        return 0;
    else
        return new IntervalFFHeuristic(opts);
}

void IntervalFFHeuristic::initialize() {
    if(DEBUG) cout << "Initializing (old-fashioned) Interval relaxed FF heuristic..." << endl;
    IntervalRelaxationHeuristic::initialize();
}

ap_float IntervalFFHeuristic::compute_heuristic(
		const GlobalState& global_state) {
	State state = convert_global_state(global_state);
	setup_exploration(state);

//	cout << "Starting exploration from initial state s_0" << endl;
	assert (planning_graph.size() == 1);
//	planning_graph.front().dump();
	relaxed_exploration();

	// setup plan extraction
	plan = RelaxedPlan(task_proxy.get_operators().size(), planning_graph.size()); // reset plan
	numeric_markings = vector<vector<bool> > (planning_graph.front().size(), vector<bool> (planning_graph.size(), false));

//	cout << "h_FF: marking operators..." << endl;
	// mark operators
	for (const auto &goal: goal_propositions) {
//		cout << "Goal " << debug_fact_names[goal->id] << " costs " << goal->cost << endl;
		mark_preferred_operators_and_relaxed_plan(state, goal);
	}

	// plan cost extraction
	ap_float plan_cost = 0;

	assert(task_proxy.get_operators().size() == plan.prop_ops.size());
	assert(plan.num_ops.size() == plan.prop_ops.size());

//	cout << "h_FF: extracting plan..." << endl;

	for (size_t op_no = 0; op_no < plan.prop_ops.size(); ++op_no) {
		bool has_to_be_applied = true;
		int has_to_be_applied_up_to_layer = plan.prop_ops[op_no];
		if (has_to_be_applied_up_to_layer == -1)
			has_to_be_applied = false; // operator does not have to be applied for its propositional effects
		for (size_t layer = 0; layer < plan.num_ops[op_no].size(); ++ layer) {
			if (plan.num_ops[op_no][layer]) {
				// apply operator once for each layer where it has to be applied
				plan_cost += task_proxy.get_operators()[op_no].get_cost();
				if (has_to_be_applied && (int) layer <= has_to_be_applied_up_to_layer)
					has_to_be_applied = false; // the propositional effects of the operator already applied as side-effect from applying it for a numeric effect
			}
		}
		if (has_to_be_applied) {
			// apply operator once for propositional effect if it wasn't applied
			plan_cost += task_proxy.get_operators()[op_no].get_cost();
		}
	}
//	cout << "h_FF cost = " << plan_cost << " h_add cost of first goal = " << goal_propositions[0]->cost;
//	assert(false);
	return plan_cost;
}

IntervalFFHeuristic::IntervalFFHeuristic(const options::Options& options)
 : IntervalRelaxationHeuristic(options), plan(0,0)
{}

void IntervalFFHeuristic::mark_preferred_operators_and_relaxed_plan(
		const State& state, Proposition* goal) {
    if (!goal->marked) { // Only consider each subgoal once.
        goal->marked = true;
        UnaryOperator *unary_op = goal->reached_by;
//        cout << "Enabling " << debug_fact_names[goal->id] << " with ";
        if (unary_op) { // We have not yet chained back to a start node.
//        	cout << unary_op->str() << endl;
            for (size_t i = 0; i < unary_op->precondition.size(); ++i)
                mark_preferred_operators_and_relaxed_plan(
                    state, unary_op->precondition[i]);
            int operator_no = unary_op->operator_no;
            if (operator_no != -1) {
                // This is not an axiom.
            	assert(goal->reached_in_layer >= 1);
            	if (plan.prop_ops[operator_no] == -1)
            		plan.prop_ops[operator_no] = goal->reached_in_layer;
            	else
            		plan.prop_ops[operator_no] = min(plan.prop_ops[operator_no],goal->reached_in_layer);
                if (unary_op->precondition_cost == 0) {
                    // This test is implied by the next but cheaper,
                    // so we perform it to save work.
                    // If we had no 0-cost operators and axioms to worry
                    // about, it would also imply applicability.
                    OperatorProxy op = task_proxy.get_operators()[operator_no];
                    if (is_applicable(op, state))
                        set_preferred(op);
                }
            } else if (unary_op->is_numeric_axiom) {
            	assert(goal->reached_in_layer >= 1);
            	int my_layer = goal->reached_in_layer;
            	assert((int) planning_graph.size() > my_layer);
            	Interval left = planning_graph[my_layer].get_val(unary_op->axiom_left_var);
            	Interval right = planning_graph[my_layer].get_val(unary_op->axiom_right_var);
            	ap_float left_init = planning_graph[0].get_val(unary_op->axiom_left_var).left;
            	assert (left_init == planning_graph[0].get_val(unary_op->axiom_left_var).right); // initial value has to be point interval
            	ap_float right_init = planning_graph[0].get_val(unary_op->axiom_right_var).left;
            	assert (right_init == planning_graph[0].get_val(unary_op->axiom_right_var).right); // initial value has to be point interval
            	Targetvalues targetvals = determine_target_values(left, left_init, unary_op->comp_ax_op, right, right_init);
            	assert(left.contains(targetvals.left));
            	mark_preferred_operators_and_relaxed_plan(state, unary_op->axiom_left_var, targetvals.left, goal->reached_in_layer);
            	assert(right.contains(targetvals.right));
            	mark_preferred_operators_and_relaxed_plan(state, unary_op->axiom_right_var, targetvals.right, goal->reached_in_layer);
            }
        }
//        else cout << " INIT" << endl;
    }

}

void IntervalFFHeuristic::mark_preferred_operators_and_relaxed_plan(
		const State& state, size_t num_var, ap_float target_value, size_t layer) {
//	if(numeric_markings.size() <= num_var) cout << "Assertion about to fail. Numeric markings size = " << numeric_markings.size() << " variables, index = " << num_var << endl;
	assert(numeric_markings.size() > num_var);
	assert(numeric_markings[num_var].size() > layer);
	if (!planning_graph[layer].get_val(num_var).contains(target_value)) {
		cout << "Assertion will fail" << endl;
		cout << precise_str(target_value) << " is not contained in the interval of var #" << num_var << " with value " << planning_graph[layer].get_val(num_var).precise_str() << " in layer " << layer << endl;
		cout << "var " << num_var << " = " << g_numeric_var_names[num_var] << endl;
	}
	assert(planning_graph[layer].get_val(num_var).contains(target_value));
	if(!numeric_markings[num_var][layer]) {
		numeric_markings[num_var][layer] = true;
		UnaryOperator* unary_op = determine_achiever(num_var, layer, target_value);
//		cout << "Enabling " << target_value << " for " << g_numeric_var_names[num_var] << " with ";
        if (unary_op) { // We have not yet chained back to a start node.
//        	cout << unary_op->str() << endl;
            for (size_t i = 0; i < unary_op->precondition.size(); ++i)
                mark_preferred_operators_and_relaxed_plan(
                    state, unary_op->precondition[i]);
            int operator_no = unary_op->operator_no;
            if (operator_no != -1) {
                // This is not an axiom.
                if (unary_op->effect.numeric) {
                  	// handle "implicit preconditions"
                  	Interval left = planning_graph[layer-1].get_val(unary_op->effect.aff_variable_index);
                  	ap_float left_init = planning_graph[0].get_val(unary_op->effect.aff_variable_index).left;
                  	assert (left_init == planning_graph[0].get_val(unary_op->effect.aff_variable_index).right); // initial value has to be point interval
                  	Interval right = planning_graph[layer-1].get_val(unary_op->effect.val_or_ass_var_index);
                  	ap_float right_init = planning_graph[0].get_val(unary_op->effect.val_or_ass_var_index).left;
                  	assert (right_init == planning_graph[0].get_val(unary_op->effect.val_or_ass_var_index).right); // initial value has to be point interval
                  	assert((compute(left, unary_op->effect.assign_type, right) || left).contains(target_value));
                  	Targetvalues targetvals = determine_target_values(left, left_init, unary_op->effect.assign_type, right, right_init, target_value);
                  	assert(left.contains(targetvals.left));
                  	mark_preferred_operators_and_relaxed_plan(state, unary_op->effect.aff_variable_index, targetvals.left, layer-1);
                  	assert(right.contains(targetvals.right));
                  	mark_preferred_operators_and_relaxed_plan(state, unary_op->effect.val_or_ass_var_index, targetvals.right, layer-1);
                }
                plan.num_ops[operator_no][layer] = true;
                if (layer == 0 && unary_op->precondition_cost == 0) {
                    // This test is implied by the next but cheaper,
                    // so we perform it to save work.
                    // If we had no 0-cost operators and axioms to worry
                    // about, it would also imply applicability.
                    OperatorProxy op = task_proxy.get_operators()[operator_no];
                    if (is_applicable(op, state))
                        set_preferred(op);
                }
            } else if (unary_op->is_numeric_axiom) {
            	assert(layer >= 1);
            	assert(planning_graph.size() > layer);
            	Interval left = planning_graph[layer].get_val(unary_op->axiom_left_var); // Axioms operate on same layer
            	Interval right = planning_graph[layer].get_val(unary_op->axiom_right_var);
//            	if (!apply_ass_axiom(left, unary_op->ass_ax_op, right).contains(target_value)) {
//            		cout << "Was trying to enable " << target_value << " for " << g_numeric_var_names[num_var] << " with " << unary_op->str() << endl;
//            		cout << "layer " << layer << " varno " << num_var << endl;
//            		cout << "Assertion will fail: " << left << unary_op->ass_ax_op << right << " = " << apply_ass_axiom(left, unary_op->ass_ax_op, right) << " target = " << target_value << endl;
//
//            		for (size_t l = 0; l < planning_graph.size(); ++l) {
//            			auto achs_in_layer = planning_graph[l].get_achievers(num_var);
//            			cout << "Layer " << l << " val= " << planning_graph[l].get_val(num_var) << " cost= " << planning_graph[l].get_cost(num_var)
//            							<< " #achievers = " << achs_in_layer.size()<< endl;
//            			for (auto ac : achs_in_layer) cout << "     - " << ac->str() << endl;
//            		}
//            	}
            	assert(compute(left, unary_op->ass_ax_op, right).contains(target_value));
            	ap_float left_init = planning_graph[0].get_val(unary_op->axiom_left_var).left;
            	assert (left_init == planning_graph[0].get_val(unary_op->axiom_left_var).right); // initial value has to be point interval
            	ap_float right_init = planning_graph[0].get_val(unary_op->axiom_right_var).left;
            	assert (right_init == planning_graph[0].get_val(unary_op->axiom_right_var).right); // initial value has to be point interval
            	Targetvalues targetvals = determine_target_values(left, left_init, unary_op->ass_ax_op, right, right_init, target_value);
            	assert(left.contains(targetvals.left));
            	mark_preferred_operators_and_relaxed_plan(state, unary_op->axiom_left_var, targetvals.left, layer);
            	assert(right.contains(targetvals.right));
            	mark_preferred_operators_and_relaxed_plan(state, unary_op->axiom_right_var, targetvals.right, layer);
            }
        }
//        else cout << " INIT" << endl;
	}
}

UnaryOperator* IntervalFFHeuristic::determine_achiever(int var_index,
		size_t layer, ap_float target) {
//	Interval current_value = planning_graph[layer].get_val(var_index);
//	cout << "Have to determine achiever for target value " << target << " of " << var_index << " : "<< g_numeric_var_names[var_index] <<endl;
//	cout << "    with current value " << current_value << " in layer " << layer
//			<< " #a = " << planning_graph[layer].get_achievers(var_index).size() <<  endl;
	assert(layer < planning_graph.size());
	assert(planning_graph[layer].get_val(var_index).contains(target));
	if (layer == 0) {
		assert(planning_graph[layer].get_val(var_index).contains(target));
		return 0; // Initial value is already achieved, achiever is None
	}
	Interval before = planning_graph[layer-1].get_val(var_index);
//	cout << "iv before = " << before << endl;
	if (before.contains(target)) {
		return determine_achiever(var_index, layer-1, target); // follow "idle arc"
	}
	vector<UnaryOperator *> achievers = planning_graph[layer].get_achievers(var_index);
//	if (achievers.size() == 0) {
//		cout << "Assertion about to fail, dumping complete plangraph for variable " << var_index << " : " << g_numeric_var_names[var_index] << ":" << endl;
//		for (size_t l = 0; l < planning_graph.size(); ++l) {
//			auto achs_in_layer = planning_graph[l].get_achievers(var_index);
//			cout << "Layer " << l << " val= " << planning_graph[l].get_val(var_index) << " cost= " << planning_graph[l].get_cost(var_index)
//							<< " #achievers = " << achs_in_layer.size()<< endl;
//			for (auto ac : achs_in_layer) cout << "     - " << ac->str() << endl;
//		}
//	}
	assert(achievers.size() > 0);
	UnaryOperator* best_achiever = 0;
	ap_float cost = INF;
	for (UnaryOperator* achiever: achievers) {
		Interval first;
		Interval second;
		Interval result;
		if (achiever->is_numeric_axiom) {
			// assignment axiom -> axioms operate on the intervals in the same layer
			first = planning_graph[layer].get_val(achiever->axiom_left_var);
			second = planning_graph[layer].get_val(achiever->axiom_right_var);
			result = compute(first, achiever->ass_ax_op, second);
//			cout << first << " " << achiever->ass_ax_op << " " << second << " = " << result << endl;
		} else {
			// numeric operator -> operators use the intervals from the previous layer
			first = planning_graph[layer-1].get_val(achiever->effect.aff_variable_index);
			second = planning_graph[layer-1].get_val(achiever->effect.val_or_ass_var_index);
			result = compute(first, achiever->effect.assign_type, second);
			result = result || first;
//			cout << first << " " << achiever->effect.assign_type << " " << second << " = " << result << endl;
		}
//		cout << target << " achiever " << achiever->str() <<  endl;
		if (result.contains(target) && achiever->cost() < cost) {
//			cout << "new best achiever!" << endl;
			best_achiever = achiever;
			cost = achiever->cost();
		}
	}
	assert(best_achiever);
	return best_achiever;
}

IntervalFFHeuristic::~IntervalFFHeuristic() {
}

static Plugin<Heuristic> _plugin("iihff", _parse);

RelaxedPlan::RelaxedPlan(int ops, int layers) {
	prop_ops = vector<int>(ops, -1);
	num_ops = vector<vector <bool>> (ops, vector<bool>(layers, false));
}

}
