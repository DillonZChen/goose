#ifndef NUMERIC_RELAXED_INTERVAL_HELPER_H
#define NUMERIC_RELAXED_INTERVAL_HELPER_H

#include "../globals.h"
#include "interval.h"

struct Targetvalues {
	ap_float left;
	ap_float right;
	Targetvalues(ap_float _left, ap_float _right) : left(_left), right(_right) {}
};

struct Intervalpair {
	Interval bottom;
	Interval top;
	Intervalpair(Interval _b, Interval _t) : bottom(_b), top(_t) {}
	Intervalpair(Interval i) : bottom(i), top(i) {}
};

Interval repeat_apply(const Interval& interval, const f_operator fop,
		const Interval& other);

bool relaxed_compare(const Interval& interval, const comp_operator cop,
		const Interval& other);

bool compare(ap_float left, const comp_operator cop, ap_float right);

Interval comparison_feasible(const Interval& interval, const comp_operator cop,
		const Interval& other); // returns the feasible subinterval from interval which satisfies the constraint

/**
 * Returns a value inside the interval, which is closest to target.
 * So if target lies inside the interval, target is returned.
 * If target lies outside the interval, and the close interval bound is open,
 * the return value is not the interval bound but a value in the interval with a margin of
 * EPSILON which is 1 by default.
 * In case the interval is not broad enough to move EPSILON away from the open bound,
 * instead the middle of the interval is returned.
 */
ap_float closest_val_in(ap_float target, const Interval& interval, ap_float bound_distance_epsilon = 1);

std::string precise_str(ap_float number);

/**
 * Determines target values that satisfy the constraint.
 * The total deviation from the initial values is minimized
 * while also ensuring that both sides have to do a similar
 * amount of "work" to move the target value in the right direction
 */
Targetvalues determine_target_values(Interval left_interval, ap_float cur_left,
		const comp_operator cop, Interval right_interval, ap_float cur_right);

Targetvalues determine_target_values(Interval left_interval, ap_float cur_left,
		const cal_operator aop, Interval right_interval, ap_float cur_right, ap_float target_value);

Targetvalues determine_target_values(Interval left_interval, ap_float cur_left,
		const f_operator fop, Interval right_interval, ap_float cur_right, ap_float target_value);


#endif /* NUMERIC_RELAXED_INTERVAL_HELPER_H_ */
