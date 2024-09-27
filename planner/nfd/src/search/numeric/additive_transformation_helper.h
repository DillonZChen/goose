#ifndef ADDITIVE_TRANSFORMATION_HELPER_H
#define ADDITIVE_TRANSFORMATION_HELPER_H

#include "../globals.h"
#include "interval.h"

namespace additive_transformation_helper {

Interval compute_additive_asymptotic_behavior(const Interval& interval, const f_operator fop,
		const Interval& other);

Interval compute_asymptotic_behavior(const Interval& interval, const cal_operator calop,
		const Interval& other);

Interval asymptotic_sum(const Interval &interval, const Interval &other);

Interval asymptotic_difference(const Interval &interval, const Interval &other);

Interval asymptotic_multiplication(const Interval &interval, const Interval &other);

Interval asymptotic_division(const Interval &interval, const Interval &other);

}

#endif // ADDITIVE_TRANSFORMATION_HELPER_H