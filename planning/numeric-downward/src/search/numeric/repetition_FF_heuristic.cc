#include "repetition_FF_heuristic.h"
#include "interval.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "relaxed_interval_helper.h"

using namespace std;

namespace repetition_ff_heuristic{


void RepetitionFFHeuristic::initialize() {
	if(DEBUG) cout << "Initializing Interval repetition relaxed FF heuristic..." << endl;
	RepetitionRelaxationHeuristic::initialize();
}

ap_float RepetitionFFHeuristic::compute_heuristic(const GlobalState& global_state) {
	State state = convert_global_state(global_state);
	if(DEBUG)
		cout << "Computing heuristic estimate Step 1: relaxed exploration :" << endl;
	setup_exploration_queue(state);
	relaxed_exploration();

	if(DEBUG) {
		cout << "Done with relaxed exploration, dumping numeric variables and all achievers " << endl;
		for(auto nvar: numeric_variables) {
			nvar.dump();
			//		cout << " Achievers: " << endl;
			//		for (size_t i=0; i < nvar.get_achievers().size(); ++i) {
			//			NumericAchiever &achiever = nvar.get_achievers()[i];
			//			assert(achiever.reached_by);
			//			cout << " - " << i << ": " << achiever.reached_from_interval << " extended by "  << achiever.reached_by->str() << endl;
			//		}
			cout << "===================================================---" << endl;
		}
		cout << "Dumping cycle breaker variables " << endl;
		for(auto nvar: numeric_cycle_breaker_vars) {
			nvar.dump();
			cout << "===================================================---" << endl;
		}
		cout << "Number of Achievers is " << global_achiever_ordering_rank << endl;
		cout << "Computing heuristic estimate Step 2: determining target values :" << endl;
	}
	if (DEBUG) cout << "phase 2: marking repetitions and ops" << endl;
	for (size_t i = 0; i < goal_propositions.size(); ++i) {
		mark_preferred_operators_and_determine_repetitions(state, goal_propositions[i]);
		ap_float prop_cost = goal_propositions[i]->cost;
		if(DEBUG) cout << "Goal " << goal_propositions[i]->id << " costs " << prop_cost << endl;
		if (prop_cost == INF) {
			if (DEBUG) cout << "Dead end, heuristic is INF" << endl;
			return DEAD_END;
		}

		//        else {
		//			if (goal_propositions[i]->reached_by) {
		//				if(DEBUG) cout << "Goal reached by " << goal_propositions[i]->reached_by->str() << endl;
		//			} else {
		//				if(DEBUG) cout << "Initially true goal proposition " << goal_propositions[i]->id << endl;
		//			}
		//		}
	}

	if(DEBUG)
		cout << "Computing heuristic estimate Step 3: cost extraction :" << endl;
	//	for (size_t i = 0; i< prop_variables.size(); ++i) {
	//		for (size_t j = 0; j< prop_variables[i].size(); ++j)
	//			cout << prop_variables[i][j].cost << " - " << g_fact_names[i][j] << endl;
	//		cout << "---" << endl;
	//	}
	//	for(auto nvar: numeric_variables) {
	//		nvar.dump();
	//		if (nvar.max_val != nvar.val) {
	//			cout << nvar.str() << " reached " << nvar.val << " with cost " << nvar.cost << endl;
	//		}
	//		cout << "---" << endl;
	//	}

	ap_float total_cost = 0;
	for (const auto op: all_operators) {
		if(op->repetitions == 0) continue;
		total_cost += op->base_cost * op->repetitions;
		if(DEBUG && !op->is_axiom()) cout << op->repetitions << " repetitions of " << op->str() << " (" << op->base_cost<< ") increase total_cost to " << total_cost << endl;
		op->repetitions = 0; // cleanup for next heuristic computation
	}
	global_achiever_ordering_rank = 0; // cleanup for next round
	if(DEBUG)
		cout << "Computed heuristic estimate: " << total_cost << endl;
	return total_cost;
}

static Heuristic *_parse(OptionParser &parser) {
	parser.document_synopsis("Interval Numeric Add heuristic", "");
	parser.document_language_support("action costs", "supported");
	parser.document_language_support("conditional effects", "supported");
	parser.document_language_support("numeric", "supported");
	parser.document_language_support("axioms",
			"supported (in the sense that the planner won't complain -- "
			"handling of axioms might be very stupid "
			"and even render the heuristic unsafe)");
	parser.document_property("admissible", "no");
	parser.document_property("consistent", "no");
	parser.document_property("safe", "yes for tasks without axioms");
	parser.document_property("preferred operators", "yes");

	Heuristic::add_options_to_parser(parser);
	Options opts = parser.parse();

	if (parser.dry_run())
		return 0;
	else
		return new RepetitionFFHeuristic(opts);
}


RepetitionFFHeuristic::RepetitionFFHeuristic(const options::Options& options)
:  RepetitionRelaxationHeuristic(options), did_write_overflow_warning(false)
{}

void RepetitionFFHeuristic::mark_preferred_operators_and_determine_repetitions(const State& state,
		IProposition* goal) {

	if (!goal->marked) { // Only consider each subgoal once.
		goal->marked = true;
		UnaryOperator *unary_op = goal->reached_by;
		if (unary_op) { // We have not yet chained back to a start node.
			if(DEBUG) cout << "Prop " << linearized_fact_names[goal->id] << " is reached by " << unary_op->str() << " with cost " << goal->cost << endl;
			if (unary_op->repetitions == 0) {
				unary_op->repetitions++;
				if (DEBUG) cout << "Set repetitions of " << unary_op->str() << " to " << unary_op->repetitions << endl;
			}

			if(unary_op->op_type == operatorType::comp_axiom) {
				NumericStateVariable *left = unary_op->axiom_left_right[0];
				assert(left);
				NumericStateVariable *right = unary_op->axiom_left_right[1];
				assert(right);
				ap_float cur_left = state.nval(left->get_id());
				ap_float cur_right = state.nval(right->get_id());
				Interval left_feasible = left->val_at_id(goal->exploration_index);
				Interval right_feasible = right->val_at_id(goal->exploration_index);
				assert(relaxed_compare(left_feasible, unary_op->comp_ax_op, right_feasible));
				Targetvalues targetvals = determine_target_values(left_feasible, cur_left, unary_op->comp_ax_op, right_feasible, cur_right);
				// only have to achieve new target value if the current value does not already achieve the comparison axiom
				if (unary_op->comp_ax_op == ue || !compare(cur_left, unary_op->comp_ax_op, targetvals.left))
					mark_preferred_operators_and_determine_repetitions(state, left, targetvals.left, goal->cost, goal->exploration_index);
				else if (DEBUG) cout << "Current left value " << cur_left << " already satisfied for axiom" << endl;
				if (unary_op->comp_ax_op == ue || !compare(cur_right, unary_op->comp_ax_op, targetvals.right))
					mark_preferred_operators_and_determine_repetitions(state, right, targetvals.right, goal->cost, goal->exploration_index);
				else if (DEBUG) cout << "Current right value " << cur_left << " already satisfied for axiom" << endl;
			}
			for (size_t i = 0; i < unary_op->precondition.size(); ++i) {
				//            	if(DEBUG) cout << "Marking precondition " << linearized_fact_names[unary_op->precondition[i]->id] << " from operator " << unary_op->str() << endl;
				mark_preferred_operators_and_determine_repetitions(state, unary_op->precondition[i]);
			}
			int operator_no = unary_op->operator_no;
			if (unary_op->precondition_cost == 0 &&
					operator_no >= 0 && unary_op->op_type == operatorType::logic_op) {
				// Necessary condition for this being a preferred
				// operator, which we use as a quick test before the
				// more expensive applicability test.
				// If we had no 0-cost operators and axioms to worry
				// about, this would also be a sufficient condition.
				//            	cout << "fetching operator # " << operator_no << endl;
				//            	cout << unary_op->str() << endl;
				//            	cout << "number of ops = " << task_proxy.get_operators().size() << endl;
				//            	for (size_t i = 0; i < task_proxy.get_operators().size(); ++i) {
				//            		cout << i << ": " << task_proxy.get_operators()[i].get_name() << endl;
				//            	}
				assert(operator_no < (int) task_proxy.get_operators().size());
				OperatorProxy op = task_proxy.get_operators()[operator_no];
				if (is_applicable(op, state)) {
					set_preferred(op);
					if(DEBUG) cout << "Identified preferred operator : " << unary_op->str() << endl;
				}
			}
		}
	}
}

void RepetitionFFHeuristic::mark_preferred_operators_and_determine_repetitions(
		const State& state, NumericStateVariable* subgoal, ap_float target_value, ap_float parent_cost, int parent_ordinal_number) {
	if(DEBUG) {
		assert(subgoal);
		cout << "Subgoal " << subgoal->str() << "\n  has to reach target value " << target_value << " with cost " << parent_cost
				<< " and achiever ordering bound " << parent_ordinal_number << endl;
	}
	const ap_float current_val = state.nval(subgoal->get_id());
	//	cout << "Current value of state " << current_val << endl;
	if (target_value == current_val) {
		if(DEBUG) cout << "Target value " << target_value <<" already achieved in current state for " << subgoal->str() << endl;
		return;
	}
	//	else {
	//		printf("Target value %.16e differs from current value %.16e\n", target_value, current_val);
	//	}

	assert(subgoal);
	//	vector<NumericAchiever> &achievers = subgoal->get_achievers();
	//	assert(achievers.size() >= 1);

	/* We want to find an operator achieving the required target value.
	 * With get_achievers we get a list of monotonically growing list of intervals
	 * and we are interested in the smallest (= cheapest to achieve) of those
	 * which contains the target_value
	 */

	Interval achieved_interval = subgoal->val_at_id(parent_ordinal_number);
	assert(achieved_interval.contains(target_value)); // first assertion -> the variable CAN reach target_value
	//	size_t achiever_index = get_best_achiever_index(achievers, parent_ordinal_number, target_value);
	//	assert (achiever_index >= 1); // target_value would have been current_val and we would already have returned
	//
	//	assert(!achievers[achiever_index-1].reached_interval.contains(target_value)); // if the target value is already achieved in the current state, we should have returned at the beginning of this method
	//	Interval achieved_interval;
	//	if (achiever_index == achievers.size())
	//		achieved_interval=subgoal->val;
	//	else
	//		achieved_interval = achievers[achiever_index].reached_interval;
	//	assert(achieved_interval.contains(target_value)); // second assertion -> the variable can reach target_value satisfying the ordering constraint

	// the interval reached from contains the target_value and can be achieved from the operator one index lower
	//	int achiever_index = subgoal->index_at_id(parent_ordinal_number);
	//	assert(achiever_index >= 0); // If the index is -1, target_val has to be current_val and we already returned
	NumericAchiever &achiever = *subgoal->get_best_achiever(target_value);
	//	if (achiever.cost > parent_cost) {
	//		cout << "Assertion fails" <<endl;
	//		cout << "Achiever # " << achiever.order_pos << " from " << achiever.enque_pos << " op " << achiever.reached_by->str() << endl;
	//		cout << "Achiever cost " << achiever.cost << " while parent " << parent_cost << endl;
	//		cout << "reached interval " << achiever.reached_interval << endl;
	//		achiever.reached_by->axiom_left_right[0]->dump();
	//		achiever.reached_by->axiom_left_right[1]->dump();
	//		numeric_variables[achiever.reached_by->effect.aff_variable_index].dump();
	//	}
	assert(achiever.cost <= parent_cost);

	if(DEBUG) cout << "We can reach " << achieved_interval << " which contains " << target_value
			<< " by achiever# " << achiever.order_pos << endl;

	if (achiever.order_pos > parent_ordinal_number) {
		cout << "Assertion will fail!!\n---- " << endl;
		subgoal->dump();
		cout << "---- " << endl;
		cout << "achiever ordinal number = " << achiever.order_pos << " parent = " << parent_ordinal_number << endl;
	}
	assert(achiever.order_pos <= parent_ordinal_number);
	assert(achiever.enque_pos < parent_ordinal_number);

	UnaryOperator* achieving_op = achiever.reached_by;
	assert(achieving_op);

	Interval value_before = subgoal->val_at_id(achiever.enque_pos);
	if(DEBUG) {
		cout << "Starting from " << value_before << " we can reach " << target_value;
	}

	//	if (achiever_cost > parent_cost) {
	//		printf("target value %.16e\n", target_value);
	//		cout << "achiever cost " << achiever_cost << " includes base cost " << achiever->base_cost << endl;
	//		cout << "parent cost " << parent_cost << endl;
	//		cout << "subgoal: " << subgoal->str() << endl;
	//		subgoal->dump();
	//		cout << "achiever " << achiever->str() << endl;
	//		cout << "Assertion will fail" << endl;
	//	}
	assert(achiever.cost <= parent_cost);
	if(DEBUG) cout << "  using Operator " << achieving_op->str() << endl;

	if (achieving_op->op_type == operatorType::ass_axiom) {
		assert(achieving_op->base_cost == 0);
		assert(achieving_op->axiom_left_right.size() == 2);
		NumericStateVariable *left = achieving_op->axiom_left_right[0];
		NumericStateVariable *right = achieving_op->axiom_left_right[1];
		assert(left);
		assert(right);
		const ap_float current_left =  state.nval(left->get_id());
		const ap_float current_right =  state.nval(right->get_id());

		if (DEBUG) cout << "Assignment Axiom: searching target values to make " << left->str() << " " << achieving_op->ass_ax_op << " " << right->str() << " evaluate to " << target_value <<endl;
		if (DEBUG) cout << "Currently left = " << current_left << " and right = " << current_right << endl;

		Interval current_result = compute(current_left,achieving_op->ass_ax_op,current_right);
		if(current_result.contains(target_value)) {
			if (DEBUG) cout << "Axiom result " << current_result.precise_str() << " already contains target value" << target_value << endl;
			if (DEBUG) printf("Rounding error: target value %.16e differs from current value %.16e\n", target_value, current_val);
			return;
		}
		Interval left_before = left->val_at_id(achiever.enque_pos);
		Interval right_before = right->val_at_id(achiever.enque_pos);
		////
		////				feasible = left->val_at_id(parent_ordinal_number);
		//////		cout << "leftachieverssize = " << left->get_achievers().size() << endl;
		//////		cout << "leftfeasible starts with = " << left_feasible << endl;
		////		// while the last achiever found after this loop has an index which is too high,
		////		// the reach from interval IS reachable
		////		for (size_t i= left->get_achievers().size(); i > 0; --i) {
		//////			cout << "testing i = " << i - 1 << " cost = " << left->get_achievers()[i-1].cost << endl;
		////			if (left->get_achievers()[i-1].order_pos > achiever.order_pos) {
		////			//if (left->get_achievers()[i-1].cost > parent_cost) {
		////				left_feasible = left->get_achievers()[i-1].reached_interval;
		//////				cout << "diminishing leftfeasible to " << left_feasible << endl;
		////			}
		////		}
		////		if (DEBUG) cout << "cost checked left_feasible = " << left_feasible.precise_str() << endl;
		//
		//		Interval right_feasible = right->val;
		////		cout << "rightachieverssize = " << right->get_achievers().size() << endl;
		////		cout << "rightfeasible starts with = " << right_feasible << endl;
		//		for (size_t i= right->get_achievers().size(); i > 0; --i) {
		////			cout << "testing " << i << endl;
		//			if (right->get_achievers()[i-1].order_pos > achiever.order_pos) {
		////			if (right->get_achievers()[i-1].cost > parent_cost) {
		//				right_feasible = right->get_achievers()[i-1].reached_interval;
		////				cout << "diminishing rightfeasible to " << right_feasible << endl;
		//			}
		////			else cout << right->get_achievers()[i-1].order_pos << " in bounds (" << achiever.order_pos << ")" << endl;
		//		}
		////		right->dump();
		////		if (DEBUG) cout << "cost checked right_feasible = " << right_feasible.precise_str() << endl;
		if(DEBUG) cout << "Cost checked '" << achieving_op->ass_ax_op << "' axiom result = " << compute(left_before, achieving_op->ass_ax_op, right_before).precise_str() << " contains target value " << target_value << endl;

		Targetvalues targets = determine_target_values(left_before, current_left, achieving_op->ass_ax_op, right_before, current_right, target_value);

		if (DEBUG) printf("Determined left target value %.16e for %s\n", targets.left, left->str().c_str());
		if (DEBUG && abs(current_left - targets.left) < 0.001 && current_left != targets.left)
			printf("Warning! lefttarget %.16e is almost current %.16e\n", targets.left, current_left);
		mark_preferred_operators_and_determine_repetitions(state, left, targets.left, achiever.cost, achiever.enque_pos);
		if (DEBUG) printf("Determined right target value %.16e for %s\n", targets.right, right->str().c_str());
		if (DEBUG && abs(current_right - targets.right) < 0.001 && current_right != targets.right)
			printf("Warning! righttarget %.16e is almost current %.16e\n", targets.right, current_right);
		mark_preferred_operators_and_determine_repetitions(state, right, targets.right, achiever.cost, achiever.enque_pos);
	} else if (achieving_op->op_type == operatorType::numeric_op) {
		// We have to enable a certain target value for our affected variable.
		// We already know the interval of the affected variable before operator application,
		// but we do not know which interval we have to chose from the assignment.
		// In order to terminate asap we chose the least demanding interval for the assignment
		// (preconditions for more extended intervals might not be fulfilled, more repetitions is better than a deadlock)
		if (DEBUG) {
			cout << "Numeric Operator: " << achieving_op->str() << endl;
			cout << "  has to enable " << target_value << " for " << subgoal->str() << endl;
		}
		//		Interval aff_iv = achievers[achiever_index-1].reached_interval;
		//		if (DEBUG) cout << subgoal->str() << " is " << aff_iv << " before Operator application " << endl;
		//		if (DEBUG) cout << "Searching for the least demanding interval in the effect " << numeric_variables[achieving_op->effect.val_or_ass_var_index].str() << endl;
		assert((int) numeric_variables.size() > achieving_op->effect.val_or_ass_var_index);
		//		vector<NumericAchiever> &assignment_achievers = numeric_variables[achieving_op->effect.val_or_ass_var_index].get_achievers();
		//		size_t min_ass_target_index = 0;
		//		size_t max_ass_target_index = assignment_achievers.size();
		//		size_t ass_target_index = 0;
		NumericStateVariable *assignment_var = &numeric_variables[achieving_op->effect.val_or_ass_var_index];
		assert(assignment_var->get_id() == achieving_op->effect.val_or_ass_var_index);
		const ap_float current_ass = state.nval(achieving_op->effect.val_or_ass_var_index);
		Interval ass_iv = Interval(current_ass);
		//	 	if (DEBUG) {
		//	 		// Verify that the smallest achievable interval is equal to the current state value
		//	 		if(assignment_achievers.size() >= 1) {
		////	 			cout << "reached from 0" << endl;
		//	 			ass_iv = assignment_achievers[0].reached_interval;
		////	 			cout << ass_iv << " ass iv from 0 size: " << assignment_achievers.size() << endl;
		////	 			cout << "subgoal id " << subgoal->get_id() << endl;
		//	 		} else {
		////	 			cout << "lastval" << endl;
		//	 			ass_iv = numeric_variables[achieving_op->effect.val_or_ass_var_index].val;
		//	 		}
		////	 		printf("Current val %.16e", current_ass);
		////	 		cout << "= " << ass_iv.precise_str() << " verification " <<endl;
		////	 		cout << Interval(current_ass).precise_str() << endl;
		//	 		assert(ass_iv == Interval(current_ass));
		//	 	}
		Interval op_result  = repeat_apply(value_before, achieving_op->effect.assign_type, ass_iv);
		if (op_result.contains(target_value)) {
			if (DEBUG) cout << "Assignment "<< op_result << " already achievable in current state " << endl;
		} else {
			ass_iv = assignment_var->val_at_id(achiever.enque_pos);
			//			assert(assignment_achievers.size() >= 1);
			//			if (DEBUG) cout << "Assignment has to be enlarged in order to achieve target value " << target_value << " with operator " << achieving_op->str() << endl;
			//			while(min_ass_target_index < max_ass_target_index) {
			//				size_t middle_index = (min_ass_target_index + max_ass_target_index) / 2;
			////				if(middle_index >= assignment_achievers.size()) {
			////					cout << "Assertion will fail " << middle_index << " size = " << assignment_achievers.size() << endl;
			////				}
			//				assert(middle_index < assignment_achievers.size()); // while loop should already have terminated
			//				int ass_achiever_index = 0;
			//				if (middle_index > 0)
			//					ass_achiever_index = assignment_achievers[middle_index - 1].order_pos;
			//				if (ass_achiever_index > parent_ordinal_number) { // ass cost already includes operator cost
			////					cout << "Assignment #" << middle_index << " is too expensive " << ass_achiever_index << " > " << parent_ordinal_number << endl;
			//					max_ass_target_index = middle_index - 1;
			//					continue;
			//				}
			//				// determine the interval at the current index
			//				ass_iv = assignment_achievers[middle_index].reached_interval;
			//				Interval repeated_assignment = repeat_apply(aff_iv, achieving_op->effect.assign_type, ass_iv);
			////				if (DEBUG) cout << "Repeated application with " << ass_iv << " (index " << middle_index << ") yields "
			////						<< repeated_assignment << " which ";
			//				if(repeated_assignment.contains(target_value)) {
			////					if(DEBUG) cout << " contains " << target_value << endl;
			//					max_ass_target_index = middle_index;
			//				} else {
			////					if(DEBUG) cout << " does not contain " << target_value << endl;
			//					min_ass_target_index = middle_index + 1;
			//				}
			//			}
			//			assert(min_ass_target_index == max_ass_target_index);
			//			// determine the interval at the final index
			//			if (min_ass_target_index < assignment_achievers.size()) {
			//				ass_iv = assignment_achievers[min_ass_target_index].reached_interval;
			//			} else {
			//				ass_iv = numeric_variables[achieving_op->effect.val_or_ass_var_index].val;
			//			}
		}

		//		cout << "Operator " << achiever->str() << "\n  uses assignment " << ass_iv.precise_str() << " with index "<< max_ass_target_index << endl;
		//		if (!repeat_apply(value_before, achieving_op->effect.assign_type, ass_iv).contains(target_value)) {
		//			cout << "The ass_iv achieving_op index is " << min_ass_target_index << " = " << max_ass_target_index << " ass_iv = " << ass_iv << endl;
		//			cout << "aff_iv " << aff_iv.precise_str() << " op " << achieving_op->effect.assign_type << endl;
		//			cout << "rep result " << repeat_apply(aff_iv, achieving_op->effect.assign_type, ass_iv).precise_str() << endl;
		//			cout << "target val " << target_value << endl;
		//			cout << "subgoal id = " << subgoal->get_id() << endl;
		//		}
		if(!repeat_apply(value_before, achieving_op->effect.assign_type, ass_iv).contains(target_value)) {
			cout << "Assertion fails for op " << achieving_op->str() << endl;
			cout << "parent# " << parent_ordinal_number << endl;
			cout << "val before " << value_before << " assignment = " << ass_iv << endl;
			cout << "target " << target_value << " not in " << repeat_apply(value_before, achieving_op->effect.assign_type, ass_iv) << endl;
			numeric_variables[achieving_op->effect.aff_variable_index].dump();
			numeric_variables[achieving_op->effect.val_or_ass_var_index].dump();
		}
		assert(repeat_apply(value_before, achieving_op->effect.assign_type, ass_iv).contains(target_value));

		// We now have to determine target values in  aff_before o= assignment = targetvalue
		// For the target values we prefer values as close as possible to the current value of the
		// respective variables. However, we have to avoid values close to behavior class borders
		// e.g. x * y = 3 with x_0 = -5 and y_0 = 0 and x= (0,INF) y=(1,INF) should not lead to
		// x_t = EPSILON and y_t = 1+EPSILON (the closest values within the respective intervals)
		// because this leads to unnecessary hight repetitions.
		// idea for now: just add 1 to the border


		ap_float aff_target = closest_val_in(current_val,value_before); // for assign, increase or decrease this will not change
		//		printf("Currently afftarget is %.16e", aff_target);
		ap_float ass_target = closest_val_in(current_ass,ass_iv);
		int repetitions = -1;
		if(value_before.contains(target_value)) {
			cout << "Special case: subgoal can be reached with 0 repetitions - should have selected another operator before" << endl;
			assert(false); // should have selected another operator before
		}
		switch (achieving_op->effect.assign_type) {
		case assign:
			assert((value_before || ass_iv).contains(target_value)); // target value is either in the new interval OR in the space between the current value and the new target (which is added by convex union)
			ass_target = closest_val_in(target_value, ass_iv);
			repetitions = 1;
			break;
		case scale_up: {
			Interval best_aff_behavior_class;  // there can be multiple behavior classes e.g. [-2,2] * [-3,3] = 6 could be achieved by -2 * -3 or by 2 * 3
			Interval best_ass_behavior_class;
			Interval positive_half = value_before && Interval(0, INF, false, true);
			Interval positive_aff_classes_result = repeat_apply(positive_half, scale_up, ass_iv);
			Interval negative_half = value_before && Interval(-INF, 0, true, true);
			Interval negative_aff_classes_result = repeat_apply(negative_half, scale_up, ass_iv);
			if (current_val >= 0) {
				if (positive_aff_classes_result.contains(target_value)) {
					//					cout << "Target value can be achieved by scaling up from the positive half" << endl;
					best_aff_behavior_class = positive_half;
				} else {
					assert(negative_aff_classes_result.contains(target_value));
					//					cout << "Have to move target value to the negative half" << endl;
					best_aff_behavior_class = negative_half;
				}
			} else {
				if (negative_aff_classes_result.contains(target_value)) {
					//					cout << "Target value can be achieved by scaling up from the negative half" << endl;
					best_aff_behavior_class = negative_half;
				} else {
					assert(negative_aff_classes_result.contains(target_value));
					//					cout << "Have to move target value to the positive half" << endl;
					best_aff_behavior_class = positive_half;
				}
			}
			aff_target = closest_val_in(current_val,best_aff_behavior_class);
			//			printf("Currently afftarget is %.16e", aff_target);

			//			// set closest val from epsilon to 1.0 (-epsilon to -1.0) if necessary
			//			if (aff_target >= 0 && aff_target < 1) {
			//				if (best_aff_behavior_class.contains(1))
			//					aff_target = 1.0;
			//				else
			//					aff_target = aff_target + best_aff_behavior_class.right / 2;
			//			} else
			//			if (aff_target < 0 && aff_target > -1) {
			//				if (best_aff_behavior_class.contains(-1))
			//					aff_target = -1.0;
			//				else
			//					aff_target = aff_target + best_aff_behavior_class.left / 2;
			//			}
			if(!repeat_apply(Interval(aff_target),scale_up, ass_iv).contains(target_value)) {
				//				cout << "Special case: assignment has to flip " << best_aff_behavior_class.precise_str() << endl;
				Interval one_step_reverse_reachable = Interval(target_value) / ass_iv;
				//				cout << "reverse reachable" << one_step_reverse_reachable.precise_str() << endl;
				best_aff_behavior_class = best_aff_behavior_class && one_step_reverse_reachable;
				//				cout << "best aff class now " << best_aff_behavior_class.precise_str() << endl;
				aff_target = closest_val_in(aff_target, best_aff_behavior_class);
				if (aff_target - current_val > 0)
					aff_target += EPSILON;
				else if (aff_target - current_val < 0)
					aff_target -= EPSILON;
				aff_target = closest_val_in(aff_target, target_value);
			}

			//			cout << "Determined target value " << aff_target << " in " << aff_iv << " for the affected interval" << endl;
			// assert that we can still find a target value ass_target in ass_iv so that the target value target_value is reached
			//			cout << "intermediate = " << repeat_apply(Interval(aff_target),scale_up, ass_iv) << " target val = " << target_value << endl;
			assert(repeat_apply(Interval(aff_target),scale_up, ass_iv).contains(target_value));

			// now we have to find a suitable value ass_target in ass_iv
			ap_float trade_off = INF;

			vector<Interval> behaviour_classes;
			behaviour_classes.push_back(Interval(-INF, -1, false, true));
			behaviour_classes.push_back(Interval(-1));
			behaviour_classes.push_back(Interval(-1, 0, true, true));
			behaviour_classes.push_back(Interval(0));
			behaviour_classes.push_back(Interval(0, 1, true, true));
			behaviour_classes.push_back(Interval(1));
			behaviour_classes.push_back(Interval(1, INF, true, false));
			vector<ap_float> favourite_targets = {-2, -1, -0.5, 0, 0.5, 1, 2};
			assert (behaviour_classes.size() == favourite_targets.size());

			for (size_t i = 0; i < behaviour_classes.size(); ++i) {
				Interval ass_behaviour_class = ass_iv && behaviour_classes[i];
				if (repeat_apply(Interval(aff_target),scale_up, ass_behaviour_class).contains(target_value)) {
					//					cout << "potential behavior class : " << ass_behaviour_class << endl;
					ap_float ass_target_in_bc = closest_val_in(favourite_targets[i], ass_behaviour_class);
					//					cout << "ass target in this class " << ass_target_in_bc<< endl;
					int current_repetitions;
					if (aff_target == 0.0 || target_value == 0.0 || ass_target_in_bc == 0.0 || ass_target_in_bc == -1.0)
						current_repetitions = 1; // ass_target will be 0 as well
					else
						current_repetitions = ceil(log(abs(target_value / aff_target)) / log(abs(ass_target_in_bc)));
					//					cout << " requires " << current_repetitions << " repetitions" << endl;

					ap_float current_trade_off = abs(current_ass - ass_target_in_bc) + current_repetitions;
					//					cout << "tradeoff cost = " << current_trade_off << endl;
					if (current_trade_off < trade_off) {
						//						cout << "New best class and target value" << endl;
						ass_target = ass_target_in_bc;
						repetitions = current_repetitions;
					}
				}
			}
			assert(repetitions >= 0);
			assert(repeat_apply(Interval(aff_target),scale_up, Interval(ass_target)).contains(target_value));
			break;
		}
		case scale_down:
			// TODO: implement this
			ass_target = 0; // dummy init
			repetitions = 0; // dummy init
			assert(false);
			break;
		case increase: 	{
			if (target_value >= aff_target) {
				//				cout << "Increase effect has to increase " << subgoal->str() << endl;
				ass_iv = ass_iv && Interval(0, INF, true, false);
			} else {
				//				cout << "Increase effect has to decrease " << subgoal->str() << endl;
				ass_iv = ass_iv && Interval(-INF, 0, false, true);
			}
			ap_float ass_target_baseline = closest_val_in(current_ass, ass_iv);
			ass_target = ass_target_baseline;
			repetitions = ceil((target_value - aff_target)/ass_target);
			//			if (DEBUG) cout << "i = 0 ass_target = " << ass_target << " repetitions = " << repetitions << endl;
			for (int i = 1; i < FAIRNESS_REPETITIONS; ++i) {
				ass_target = closest_val_in(ass_target_baseline+((ass_target - ass_target_baseline + repetitions)/2), ass_iv);
				repetitions = ceil((target_value - aff_target)/ass_target);
				//				if (DEBUG) cout << "i = " << i << " ass_target = " << ass_target << " repetitions = " << repetitions << endl;
			}
			break; }
		case decrease:{
			if (target_value <= current_val) {
				//				cout << "Decrease effect has to decrease " << subgoal->str() << endl;
				ass_iv = ass_iv && Interval(0, INF, true, false);
			} else {
				//				cout << "Decrease effect has to increase " << subgoal->str() << endl;
				ass_iv = ass_iv && Interval(-INF, 0, false, true);
			}
			ap_float ass_target_baseline = closest_val_in(current_ass, ass_iv);
			ass_target = ass_target_baseline;
			repetitions = ceil((aff_target - target_value)/ass_target);
			//			cout << "i = 0 ass_target = " << ass_target << " repetitions = " << repetitions << endl;
			// iterate 5 times (arbitrarily chosen) to get a fairer target value (we try to balance work between operator repetitions k and values in the ass_iv variable)
			for (int i = 1; i < FAIRNESS_REPETITIONS; ++i) {
				ass_target = closest_val_in(ass_target_baseline+((ass_target - ass_target_baseline + repetitions)/2), ass_iv);
				repetitions = ceil((aff_target - target_value)/ass_target);
				//				cout << "i = " << i << " ass_target = " << ass_target << " repetitions = " << repetitions << endl;
			}
			break; }
		default:
			ass_target = 0; // dummy init
			repetitions = 0; // dummy init
			assert(false);
			break;
		}
		if (achiever.repetitions < repetitions) {
			achieving_op->repetitions = achieving_op->repetitions + repetitions - achiever.repetitions;
			achiever.repetitions = repetitions;
			if (DEBUG) cout << "Set repetitions of " << achieving_op->str() << " to " << achieving_op->repetitions << endl;
		}
		assert(repeat_apply(Interval(aff_target) || Interval(current_val), achieving_op->effect.assign_type, Interval(ass_target) || Interval(current_ass)).contains(target_value));

		if (DEBUG) cout << "Recursively setting " << subgoal->str() << " to " << aff_target << " and " << numeric_variables[achieving_op->effect.val_or_ass_var_index].str() << " to " << ass_target << endl;
		if (DEBUG && abs(aff_target - current_val) < 0.0001 && aff_target != current_val){
			printf("WARNING: setting aff_target to %.16e while current val is already %.16e\n", aff_target, current_val);
		}
		mark_preferred_operators_and_determine_repetitions(state, subgoal, aff_target, achiever.cost, achiever.enque_pos);
		if (DEBUG && abs(ass_target - current_ass) < 0.0001 && ass_target != current_ass){
			printf("WARNING: setting ass_target to %.16e while current val is already %.16e\n", ass_target, current_ass);
		}
		mark_preferred_operators_and_determine_repetitions(state, &numeric_variables[achieving_op->effect.val_or_ass_var_index], ass_target, achiever.cost, achiever.enque_pos);
		for (size_t i = 0; i < achieving_op->precondition.size(); ++i) {
			//        	if(DEBUG) cout << "Marking propositional precondition " << linearized_fact_names[achiever->precondition[i]->id] << " from operator " << achieving_op->str() << endl;
			mark_preferred_operators_and_determine_repetitions(state, achieving_op->precondition[i]);
		}
		if (achieving_op->precondition_cost == 0 &&
				achieving_op->op_type == operatorType::numeric_op) {
			// Necessary condition for this being a preferred
			// operator, which we use as a quick test before the
			// more expensive applicability test.
			// If we had no 0-cost operators and axioms to worry
			// about, this would also be a sufficient condition.
			//        	cout << "getting operator with # " << achieving_op->operator_no << endl;
			OperatorProxy op = task_proxy.get_operators()[achieving_op->operator_no];
			if (is_applicable(op, state)) {
				//TODO: we might want to ensure that the operator already does something usefull in the
				// initial state (and not after another operator extended the assignment
				//            	if (state.get_successor(op).num_values != state.num_values) {
				if(DEBUG) cout << "Identified preferred operator : " << achieving_op->str() << endl;
				set_preferred(op);
				//          	}
			}
		}
	} else {
		assert(achieving_op->op_type == operatorType::dummy);
		assert(achieving_op->effect.val_or_ass_var_index == -1);
		if (DEBUG) {
			cout << "Dummy Operator: " << achieving_op->str() << endl;
			cout << "  has to enable " << target_value << " for cycle breaker " << subgoal->str() << " with cost " << achiever.cost << endl;
		}
		// assert that subgoal points to a cycle breaker variable
		assert(subgoal == &numeric_cycle_breaker_vars[cycle_breaker_index[subgoal->get_id()]]);

		assert((int) numeric_variables.size() > subgoal->get_id());
		NumericStateVariable* ori_var = &numeric_variables[subgoal->get_id()];
		assert(ori_var);
		//		vector<NumericAchiever> &ori_var_achievers = ori_var->get_achievers();
		//		const ap_float current_val = state.nval(subgoal->get_id()); // current value of both variables

		if (DEBUG) {
			cout << " original variable (precondition of dummy operator) \n";
			ori_var->dump();
			cout << " dummy variable (effect of dummy operator) \n";
			subgoal->dump();
		}

		if (DEBUG) cout << "Searching for the corresponding interval in the original variable " << ori_var->str() << endl;
		assert(target_value != current_val);
		size_t ori_target_index = ori_var->index_at_id(achiever.enque_pos);
		Interval ori_iv = ori_var->val_at_id(achiever.enque_pos);
		//		while(ori_target_index < ori_var_achievers.size()) {
		//			ori_iv = ori_var_achievers[ori_target_index].reached_interval;
		//			if ((target_value < current_val && ori_iv.left < current_val)
		//					|| (target_value > current_val && ori_iv.right > current_val)) {
		//				if (DEBUG) cout << "yeah I can break at index " << ori_target_index << endl;
		//				break;
		//			} else {
		//				++ori_target_index;
		//			}
		//		}
		//		if(ori_target_index == ori_var_achievers.size()) {
		//			if(DEBUG) cout << "interval at max" << endl;
		//			ori_iv = ori_var->val;
		//		} else {
		//			assert (ori_iv == ori_var_achievers[ori_target_index].reached_interval);
		//		}
		//		assert(ori_target_index >= 1);
		ap_float cycle_cost = ori_var->get_achiever(ori_target_index)->cost;
		if (DEBUG) cout << "Corresponding original interval is " << ori_iv << endl;
		//		cout << "Cycle cost = " << cycle_cost << endl;

		//		Interval dummy_iv_before = achievers[achiever_index-1].reached_interval;
		//		if (DEBUG) cout << "Dummy interval before cycle " << dummy_iv_before << endl;

		//		Interval dummy_iv_after = subgoal->val_at_id(parent_ordinal_number);
		//		if(achiever_index == achievers.size()) {
		//			dummy_iv_after = subgoal->val;
		//		} else {
		//			dummy_iv_after = achievers[achiever_index].reached_interval;
		//		}
		//		if (DEBUG) cout << "Dummy interval after cycle " << dummy_iv_after << endl;
		assert(!value_before.contains(target_value));
		assert(achieved_interval.contains(target_value));

		ap_float new_target = target_value;
		if(ori_iv.contains(target_value)) {
			if (DEBUG) cout << "Target value can be reached with 0 additional cycles " << endl;
		} else {
			ap_float difference_per_cycle;
			ap_float required_distance;
			if(target_value <= value_before.left) {
				difference_per_cycle = value_before.left - ori_iv.left;
				required_distance = value_before.left - target_value;
			} else {
				assert(target_value >= value_before.right);
				difference_per_cycle = ori_iv.right - value_before.right;
				required_distance = target_value - value_before.right;
			}
			if (DEBUG) cout << "Difference achieved per cycle = " << difference_per_cycle << endl;
			if (DEBUG) cout << "Required = " << required_distance << endl;
			int cycles = ceil(required_distance / difference_per_cycle);
			if (DEBUG) cout << "Cycles = " << cycles << endl;
			assert(cycles >= 0);
			if(achieving_op->repetitions < cycles) {
				achieving_op->repetitions = cycles;
				if (DEBUG) cout << "Set repetitions of " << achieving_op->str() << " to " << achieving_op->repetitions << endl;
			}
			if(achieving_op->base_cost == 0) {
				if (DEBUG) cout << "Reset cost of " << achieving_op->str() << " to " << cycle_cost << endl;
				achieving_op->base_cost = cycle_cost;
			}
			new_target = closest_val_in(target_value, ori_iv);
		}
		if (DEBUG) cout << "Recursively setting original variable to " << new_target << endl;
		mark_preferred_operators_and_determine_repetitions(state, ori_var, new_target, achiever.cost, achiever.enque_pos);
	}
}

RepetitionFFHeuristic::~RepetitionFFHeuristic() {
}

static Plugin<Heuristic> _plugin("irhff", _parse);

}
