#include "relaxed_interval_helper.h"
#include "interval.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace std;

bool relaxed_compare(const Interval& interval, const comp_operator cop,
		const Interval& other) {
	switch(cop) {
	case lt:
		return interval.left < other.right;
	case le:
		return (interval.left < other.right) || (interval.left == other.right && !interval.left_open && !other.right_open);
	case eq:
		return relaxed_compare(interval, le, other) && relaxed_compare(interval, ge, other);
	case ge:
		return (interval.right > other.left) || (interval.left == other.right && !interval.left_open && !other.right_open);
	case gt:
		return interval.right > other.left;
	case ue:
		return (interval.right != interval.left) || (interval.right != other.right) || (interval.right != other.left);
	default:
		assert(false);
		return false;
	}
}

bool compare(ap_float left, const comp_operator cop, ap_float right) {
	switch (cop) {
	case lt:
		return left < right;
	case le:
		return left <= right;
	case eq:
		return left == right;
	case ge:
		return left >= right;
	case gt:
		return left > right;
	case ue:
		return left != right;
	default:
		assert(false);
		return false;
	}
}

Interval comparison_feasible(const Interval& interval, const comp_operator cop,
		const Interval& other) {
	if (!relaxed_compare(interval, cop, other)) return Interval(); // return [undef] interval
	Interval result = interval;
	switch(cop) {
	case lt:
		if (result.right >= other.right) {
			result.right = other.right;
			result.right_open = true;
		}
		break;
	case le:
		if (result.right > other.right) {
			result.right = other.right;
			result.right_open = other.right_open;
		}
		break;
	case eq:
		if (result.left < other.left) {
			result.left = other.left;
			result.left_open = other.left_open;
		}
		if (result.right > other.right) {
			result.right = other.right;
			result.right_open = other.right_open;
		}
		break;
	case ge:
		if (result.left < other.left) {
			result.left = other.left;
			result.left_open = other.left_open;
		}
		break;
	case gt:
		if (result.left <= other.left) {
			result.left = other.left;
			result.left_open = true;
		}
		break;
	case ue:
		break;
	default:
		result = Interval(); // will make the following assertion false
	}
	assert(result.left < result.right || (result.left == result.right && !result.left_open && !result.right_open));
	return result;
}

ap_float closest_val_in(ap_float target, const Interval& interval, ap_float bound_distance_epsilon) {
//	if (DEBUG) { cout << "checking closest val next to " << target;
//		cout << " in " << interval << endl;
//	}
	assert(interval.defined());
	if (target <= interval.left) {
		if (interval.left_open) {
			if(interval.contains(interval.left + bound_distance_epsilon))
				return interval.left + bound_distance_epsilon;
			else
				return (interval.left + interval.right) / 2;
		} else
			return interval.left;
	}
	if (target >= interval.right) {
		if (interval.right_open) {
			if(interval.contains(interval.left - bound_distance_epsilon))
				return interval.right - bound_distance_epsilon;
			else
				return (interval.left + interval.right) / 2;
		} else
			return interval.right;
	}
	return target;
}

Targetvalues determine_target_values(Interval left_interval,
		ap_float cur_left, const comp_operator cop,
		Interval right_interval, ap_float cur_right) {
	assert(relaxed_compare(left_interval, cop, right_interval));
	left_interval = comparison_feasible(left_interval, cop, right_interval);
//        		if(DEBUG) cout << "  interval fragment of " << left->str() << " = " << left->val << " is " << left_interval << " starting at " << cur_left << endl;
	ap_float middle = (cur_left + cur_right) / 2;
	ap_float left_target_val = middle;  // left should be fair ...
	if (compare(cur_left, cop, middle)) // .. unless beeing fair would result in extra work
		left_target_val = cur_left;
//        		if(DEBUG) cout << "  left target value closest to " << left_target_val;
	left_target_val = closest_val_in(left_target_val, left_interval);
//        		if(DEBUG) cout << " is  " << left_target_val << endl;
	right_interval = comparison_feasible(right_interval, get_mirror_op(cop), Interval(left_target_val));
//        		if(DEBUG) cout << "  interval fragment of " << right->str() << " = " << right->val << " is " << right_interval << " starting at " << cur_right << endl;
	ap_float right_target_val = cur_right;
	right_target_val = closest_val_in(right_target_val, right_interval);
//        		if(DEBUG) cout << " is  " << right_target_val << endl;
	assert(compare(left_target_val, cop, right_target_val));
	return Targetvalues(left_target_val, right_target_val);
}

ap_float solve_right(const ap_float left, const cal_operator aop, const ap_float target) {
	// left o right = target
	switch (aop) {
	case sum:  // l + r = t -> r = t - l
		return target - left;
	case diff: // l - r = t -> r = l - t
		return left - target;
	case mult: // l * r = t -> r = t / l
		if (left != 0)
			return target / left;
		else // avoid division by 0 -> assume, that l is an EPSILON > 0
			return (target < 0)?-INF:INF;
	case divi: // l / r = t -> r = l / t
		if (target != 0)
			return left / target;
		else
			return (left < 0)?-INF:INF;
	default:
		return 0; // dummy init
		exit(1);
	}
}

ap_float solve_left(const ap_float right, const cal_operator aop, const ap_float target) {
	switch (aop) {
	case sum:  // l + r = t -> l = t - r
		return target - right;
	case diff: // l - r = t -> l = t + r
		return target + right;
	case mult: // l * r = t -> l = t / r
		if (right != 0)
			return target / right;
		else
			return (target < 0)?-INF:INF;
	case divi: // l / r = t -> l = t * r
		return target * right;
	default:
		return 0; // dummy init
		exit(1);
	}
}

ap_float epsilonfix (ap_float target, ap_float current) {
	if (target > current) return target += EPSILON;
	else if (target < current) return target -= EPSILON;
	return target;
}

Intervalpair solve_right_interval(const Interval left, const cal_operator aop, const ap_float target) {
	switch (aop) {
	case sum:  // L + R = t -> R = t-L
		return Intervalpair(compute(Interval(target), diff, left));
	case diff: // L - R = t -> R = L - t
		return Intervalpair(compute(left, diff, Interval(target)));
	case mult: // L * R = t -> R = t / L
		return Intervalpair(
				compute(Interval(target), divi, left && Interval(-INF, 0, true, true)),
				compute(Interval(target), divi, left && Interval(0, INF, true, true)));
	case divi: // L / R = t -> R = L / t
		return Intervalpair(
				compute(left, divi, Interval(target) && Interval(-INF, 0, true, true)),
				compute(left, divi, Interval(target) && Interval(0, INF, true, true)));
	default:
		return Interval(); // dummy init
		exit(1);
	}
}

Intervalpair solve_left_interval(const Interval right, const cal_operator aop, const ap_float target) {
	switch (aop) {
	// the only case where a division occurs is the inverse of multiplication.
	case mult: // L * R = t -> L = t / R
		return Intervalpair(
				compute(Interval(target), divi, right && Interval(-INF, 0, true, true)),
				compute(Interval(target), divi, right && Interval(0, INF, true, true)));
	default:
		return Intervalpair(compute(Interval(target), get_inverse_op(aop), right));
	}
}



Interval interval_cut_float_fixed_non_empty(Interval iv, Intervalpair cut_ivs) {
	Interval bottom = iv && cut_ivs.bottom;
	Interval top = iv && cut_ivs.top;
	if(bottom.defined()) {
		if (top.defined())
			return bottom || top;
		else
			return bottom;
	}
	if (top.defined()) {
		return top;
	} else {
		// It can happen that by rounding up and down (or down and up) we do not reach the original value
		if(DEBUG) cout << "WARNING, rounding up and down yields empty interval, have to fix this... " <<endl;
		if(DEBUG) cout << "Neither bottom " << bottom.precise_str() << " nor top " << top.precise_str() << " is defined." << endl;
		if ((iv.left > cut_ivs.bottom.left) && (iv.left > cut_ivs.bottom.right)) {
			if (iv.left_open)
				bottom = iv && Interval(iv.left, iv.left + EPSILON, true, true);
			else
				bottom = Interval(iv.left, iv.left);
		} else if ((iv.right < cut_ivs.bottom.left) && (iv.right < cut_ivs.bottom.right)){
			if (iv.right_open)
				bottom = iv && Interval(iv.right - EPSILON, iv.right, true, true);
			else
				bottom = Interval(iv.right, iv.right);
		}
		if ((iv.left > cut_ivs.top.left) && (iv.left > cut_ivs.top.right)) {
			if (iv.left_open)
				top = iv && Interval(iv.left, iv.left + EPSILON, true, true);
			else
				top = Interval(iv.left, iv.left);
		} else if ((iv.right < cut_ivs.top.left) && (iv.right < cut_ivs.top.right)){
			if (iv.right_open)
				top = iv && Interval(iv.right - EPSILON, iv.right, true, true);
			else
				top = Interval(iv.right, iv.right);
		}
		if(bottom.defined()) {
			if (top.defined())
				return bottom || top;
			else
				return bottom;
		}
		assert(top.defined());
		return top;
	}
}


Targetvalues determine_target_values(Interval left_interval,
		ap_float cur_left, const cal_operator aop,
		Interval right_interval, ap_float cur_right,
		ap_float target_value) {
	assert(compute(left_interval, aop, right_interval).contains(target_value));
	if (DEBUG) cout << "Have to find targets so that " << left_interval << " " << aop << " " << right_interval << " = " << target_value << endl;
	left_interval = interval_cut_float_fixed_non_empty(left_interval, solve_left_interval(right_interval, aop, target_value));
	assert(compute(left_interval, aop, right_interval).contains(target_value));

	if (DEBUG) cout << "reachable left_interval = " << left_interval.precise_str() << endl;

	ap_float left_lazy_target_val = closest_val_in(cur_left, left_interval);

	Intervalpair right_inverses = solve_right_interval(left_interval, aop, target_value);
	// corresponding target value if left is lazy
	ap_float right_eager_target_val = solve_right(left_lazy_target_val, aop, target_value);
	right_interval = interval_cut_float_fixed_non_empty(right_interval, right_inverses);
	assert(right_interval.defined());

	if (DEBUG) cout << "reachable right_interval = " << right_interval.precise_str() << endl;
//		cout << " right eager target = " << right_eager_target_val << endl;
//	if(!compute(left_interval, aop, right_interval).contains(target_value)) {
//		cout << "Assertion about to fail: " << left_interval.precise_str() << aop << right_interval.precise_str() << endl;
//		cout << " = " << compute(left_interval, aop, right_interval).precise_str() << " does not contain " << target_value << endl;
//	}
	assert(compute(left_interval, aop, right_interval).contains(target_value));

	// The values in left_interval and right_interval can now be
	// (1.) reached with the current cost
	// (2.) all have a partner in the other interval so that [left] o [right] = target_value
	//      CAUTION! Because of float representation, the values directly on the interval bounds
	// 		might be reachable only because of rounding errors and do NOT have a direct partner

	// Instead of picking arbitrary values from left and right, we want to balance the "work"
	// that is required to reach the target values as fair as possible
	// e.g. [0,2] + [0,2] = 2 starting from left=0 and right=0 should set left and right to 1
	// and not one to 0 and the other to 2

	ap_float right_lazy_target_val = closest_val_in(cur_right, right_interval);
	ap_float left_eager_target_val = solve_left(right_lazy_target_val, aop, target_value);

	if(DEBUG) cout << "  if left is lazy, the target values are " << left_lazy_target_val << " and " << right_eager_target_val << endl;
	if(DEBUG) cout << "  if right is lazy, the target values are " << left_eager_target_val << " and " << right_lazy_target_val << endl;

	ap_float left_lazy_work = abs(left_lazy_target_val - cur_left) + abs(right_eager_target_val - cur_right);
	ap_float right_lazy_work = abs(left_eager_target_val - cur_left) + abs(right_lazy_target_val - cur_right);
	if(DEBUG) cout << "  the total work is " << left_lazy_work << " if left is lazy and " << right_lazy_work << " if right is lazy" << endl;
	right_eager_target_val = closest_val_in(right_eager_target_val, right_interval);

	ap_float fair_work;
	ap_float left_target;
	ap_float right_target;
	bool leftadapted; // if true, left could be out of bounds, if false right could be out of bounds
	if (left_lazy_work <= right_lazy_work) {// we tend towards left being lazy
		if (DEBUG) cout << "left lazy is cheaper " << endl;
		fair_work = (left_lazy_work / 2);
		left_target = left_eager_target_val;
		if(left_eager_target_val > cur_left)
			left_target = closest_val_in(cur_left + fair_work, left_interval);
		if(left_eager_target_val < cur_left)
			left_target = closest_val_in(cur_left - fair_work, left_interval);
		right_target = solve_right(left_target, aop, target_value);
		leftadapted=false;
	} else { // we tend towards right being lazy
		//			cout << "right lazy is cheaper " << endl;
		fair_work = right_lazy_work / 2;
		right_target = right_eager_target_val;
		if(right_eager_target_val > cur_right)
			right_target = closest_val_in(cur_right + fair_work, right_interval);
		if(right_eager_target_val < cur_left)
			right_target = closest_val_in(cur_right - fair_work, right_interval);
		if(DEBUG) cout << "Right target determined to be " << right_target << endl;

		assert(right_interval.contains(right_target));
		left_target = solve_left(right_target, aop, target_value);
		leftadapted=true;
	}
	//	printf ("Fixed target values to %.16e for %s and %.16e for %s \n",  left_target, "left->str().c_str()", right_target, "right->str().c_str()");
	if (DEBUG) printf ("Assignment Axiom determined fair target values %.16e for %s and %.16e for %s \n",  left_target, "left", right_target, "right->str().c_str()");
	// The target value can be just outside the axiom result interval because of float rounding errors

	right_target = closest_val_in(epsilonfix(right_target, cur_right), right_interval);
	assert(right_interval.contains(right_target));
	left_target = closest_val_in(epsilonfix(left_target, cur_left), left_interval);
	assert(left_interval.contains(left_target));

	int loop = 0;
	while (!compute(Interval(left_target) || Interval(cur_left), aop, Interval(right_target) || Interval(cur_right)).contains(target_value)) {
		// rare float rounding error: left_target had to be adjusted to fit inside the interval, but is no parter of right_target any more
		if (leftadapted) {
			right_target = solve_right(left_target, aop, target_value);
			right_target = closest_val_in(epsilonfix(right_target, cur_right), right_interval);
			leftadapted = false;
		} else {
			left_target = solve_left(right_target, aop, target_value);
			left_target = closest_val_in(epsilonfix(left_target, cur_left), left_interval);
			leftadapted = false;
		}
		loop++;
		if (loop >= 3) {
			cout << "Loop assertion fails, cannot find target values" << endl;
			printf("target val %.16e\n" , target_value);
			cout << "Left: " << left_interval.precise_str() << " target ";
			printf("left_target %.16e\n", left_target);
			printf("left_current %.16e\n", cur_left);
			cout << "Right: " << right_interval.precise_str() << " target ";
			printf("right_target %.16e\n", right_target);
			printf("right_current %.16e\n", cur_right);
			cout << left_interval << aop << right_interval << " = " <<  compute(left_interval,aop, right_interval).precise_str() << endl;
			cout << "targetval interval " << compute(Interval(left_target) || Interval(cur_left), aop, Interval(right_target) || Interval(cur_right)).precise_str() << endl;
			assert(false);
			break;
		}
	}

	//	cout << "assertion will fail" << endl;
	//		cout << "left " << (Interval(left_target) || Interval(cur_left)).precise_str() << endl;
	//		printf("left_target %.16e\n", left_target);
//		cout << "leftfeasible = " << left_interval.precise_str() << endl;
//		cout << "right " << (Interval(right_target) || Interval(cur_right)).precise_str() << endl;
//		printf("right_target %.16e\n", right_target);
//		printf("current_right %.16e\n", cur_right);
//		cout << "rightfeasible = " << right_interval.precise_str() << endl;
//		cout << "ass ax result " <<  compute(Interval(left_target) || Interval(cur_left), aop, Interval(right_target) || Interval(cur_right)).precise_str() << endl;
//		printf("target val %.16e\n" , target_value);
//		cout << "intervals are " << left_interval.precise_str() << aop << right_interval.precise_str() << (compute(left_interval, aop, right_interval)).precise_str() << endl;

	if (DEBUG) cout << "The chosen target values are " << left_target << " and " << right_target << endl;
	assert(compute(Interval(left_target) || Interval(cur_left), aop, Interval(right_target) || Interval(cur_right)).contains(target_value));
	return Targetvalues(left_target,right_target);
}

std::string precise_str(ap_float number) {
	char buffer[30];
	sprintf(buffer, "%.16e", number);
	string ret_val = buffer;
	return ret_val;
}

Targetvalues determine_target_values(Interval left_interval, ap_float cur_left,
		const f_operator fop, Interval right_interval, ap_float cur_right,
		ap_float target_value) {
	assert(left_interval.contains(cur_left));
	assert(right_interval.contains(cur_right));
	if ((Interval(cur_left) || compute(Interval(cur_left), fop, Interval(cur_right))).contains(target_value)) {
		// target_value is not within the new interval but obtained by convex union.
		// this is "good" for us, as we can just use the current values
		return Targetvalues(cur_left, cur_right);
	}
	if (left_interval.contains(target_value)) {
		// the target value is obtained by the old "left" interval
		return Targetvalues(target_value, cur_right);
	}
	switch (fop) {
	case assign:
//		if(!right_interval.contains(target_value)) {
//			cout << "Assertion will fail: " << left_interval << fop << right_interval << " does not contain " << target_value << endl;
//			Interval effect = apply_effect(left_interval, fop, right_interval);
//			cout << left_interval << " || " << effect << " = " << (left_interval || effect) << endl;
//			cout << "Current values add up to " << apply_effect(Interval(cur_left), fop, Interval(cur_right)) <<endl;
//			cout << " they are " << cur_left << " and " << cur_right << endl;
//		}
		assert(right_interval.contains(target_value));
		return Targetvalues(cur_left, target_value);
	case increase:
		return determine_target_values(left_interval,cur_left,sum,right_interval,cur_right,target_value);
	case decrease:
		return determine_target_values(left_interval,cur_left,diff,right_interval,cur_right,target_value);
	case scale_up:
		return determine_target_values(left_interval,cur_left,mult,right_interval,cur_right,target_value);
	case scale_down:
		return determine_target_values(left_interval,cur_left, divi, right_interval,cur_right,target_value);
	default:
		exit(1);
	}
}

Interval repeat_apply(const Interval& interval, const f_operator fop,
		const Interval& other) {
	Interval result = interval;
	Interval intermediate = Interval();
	while(intermediate != result) {
		intermediate = result;
		switch (fop) {
		case  assign:
			return interval || other;
			break;
		case scale_up:
			if(result.left < 0) {
				if(other.left < -1)
					result = result || Interval(-INF, INF);
				if(other.contains(-1))
					result = result || Interval(result.left, -result.left, result.left_open, result.left_open);
				if(other.left < 0 && other.right > -1)
					result = result || Interval(result.left, result.left * other.left, result.left_open, result.left_open || other.left_open);
				if(other.contains(0))
					result = result || Interval(0);
				if(other.left < 1 && other.right > 0)
					result = result || Interval(result.left, 0, result.left_open, true);
				if(other.right > 1)
					result = result || Interval(-INF, result.right, false, result.right_open);
			}
			if(result.right > 0){
				if(other.left < -1)
					result = result || Interval(-INF, INF);
				if(other.contains(-1))
					result = result || Interval(-result.right, result.right, result.right_open, result.right_open);
				if(other.left < 0 && other.right > -1)
					result = result || Interval(result.right * other.left, result.right, result.right_open || other.left_open, result.right_open);
				if(other.contains(0))
					result = result || Interval(0);
				if(other.left < 1 && other.right > 0)
					result = result || Interval(0, result.right, true, result.right_open);
				if(other.right > 1)
					result = result || Interval(result.left, INF, result.left_open, false);
			}
			break;
		case scale_down:
			if(result.left < 0) {
				if(other.left < -1 && other.right < -1) // only has an effect if other.right is also < -1 and avoids division by 0
					result = result || Interval(result.left, result.left / other.right, result.left_open, result.left_open || other.right_open);
				if(other.contains(-1))
					result = result || Interval(result.left, -result.left, result.left_open, result.left_open);
				if(other.left < 0 && other.right > -1)
					result = result || Interval(-INF, INF);
				if(other.left < 1 && other.right > 0)
					result = result || Interval(-INF, result.right, false, result.right_open);
				if(other.right > 1)
					result = result || Interval(result.left, 0, result.left_open, true);
			}
			if(result.right > 0){
				if(other.left < -1 && other.right < -1)
					result = result || Interval(result.right / other.right, result.right, result.right_open || other.right_open, result.right_open);
				if(other.contains(-1))
					result = result || Interval(-result.right, result.right, result.right_open, result.right_open);
				if(other.left < 0 && other.right > -1)
					result = result || Interval(-INF, INF);
				if(other.left < 1 && other.right > 0)
					result = result || Interval(result.left, INF, result.left_open, false);
				if(other.right > 1)
					result = result || Interval(0, result.right, true, result.right_open);
			}
			break;
		case increase:
			if(other.left < 0)
				result = result || Interval(-INF, result.right, false, result.right_open);
			if(other.right > 0)
				result = result || Interval(result.left, INF, result.left_open, false);
			break;
		case decrease:
			if(other.left < 0)
				result = result || Interval(result.left, INF, result.left_open, false);
			if(other.right > 0)
				result = result || Interval(-INF, result.right, false, result.right_open);
			break;
		default: assert(false);
		return Interval();
		}
	}
	return result;
}
