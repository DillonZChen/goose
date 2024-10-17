#include "additive_transformation_helper.h"

#include <fenv.h>

namespace additive_transformation_helper {


Interval compute_additive_asymptotic_behavior(const Interval& interval, const f_operator fop,
  const Interval& other) {
    Interval precondition(0);
	  switch (fop) {
	  case assign:
      if ((interval.left == -INF && other.right > -INF)
          || (interval.left < INF && other.right == INF))
        precondition = precondition || Interval(INF);
      if ((interval.right == INF && other.left < INF)
          || (interval.right > -INF && other.left == -INF))
        precondition = precondition || Interval(-INF);
      if (interval.left > -INF && interval.right < INF
          && other.left > -INF && other.right < INF)
        precondition = other - interval;
	  	break;
	  case scale_up:
      if (interval.left < 0) {
        if (other.left < 1)
          precondition = precondition || Interval(INF);
        if (other.right > 1)
          precondition = precondition || Interval(-INF);
      }
      if (interval.right > 0) {
        if (other.left < 1)
          precondition = precondition || Interval(-INF);
        if (other.right > 1)
          precondition = precondition || Interval(INF);
      }
	  	break;
	  case scale_down:
      if (interval.left < 0) {
        if (other.left < 0)
          precondition = precondition || Interval(INF);
        if (other.left < 1 && other.right > 0)
          precondition = precondition || Interval(-INF);
        if (other.right > 1)
          precondition = precondition || Interval(INF);
      }
      if (interval.right > 0) {
        if (other.left < 0)
          precondition = precondition || Interval(-INF);
        if (other.left < 1 && other.right > 0)
          precondition = precondition || Interval(INF);
        if (other.right > 1)
          precondition = precondition || Interval(-INF);
      }
	  	break;
	  case increase:
      precondition = other;
	  	break;
	  case decrease:
      precondition = -1.0 * other;
	  	break;
	  default: assert(false);
    }
    Interval new_interval(interval.left, interval.right, interval.left_open, interval.right_open);
    if (precondition.right > 0) {
      new_interval = new_interval || Interval(INF);
      new_interval.right_open = true;
    }
    if (precondition.left < 0) {
      new_interval = new_interval || Interval(-INF);
      new_interval.left_open = true;
    }
    return new_interval;
}

Interval compute_asymptotic_behavior(const Interval& interval, const cal_operator calop,
		const Interval& other) {
	  switch (calop) {
	  case sum:
      return asymptotic_sum(interval, other);
      break;
	  case diff:
      return asymptotic_difference(interval, other);
      break;
	  case mult:
      return asymptotic_multiplication(interval, other);
      break;
	  case divi:
      return asymptotic_division(interval, other);
      break;
	  default: assert(false);
      return Interval();
    }
}

Interval asymptotic_sum(const Interval &interval, const Interval &other) {
	if (interval.defined() && other.defined()) {
		const int originalRounding = fegetround( );
		fesetround(FE_DOWNWARD);
		ap_float lower = (interval.left > -INF && other.left > -INF) ? interval.left + other.left : -INF;
		fesetround(FE_UPWARD);
		ap_float upper = (interval.right < INF && other.right < INF) ? interval.right + other.right : INF;
		fesetround(originalRounding);
		return Interval(lower, upper, interval.left_open || other.left_open, interval.right_open || other.right_open);
	} else
		return Interval();
}

Interval asymptotic_difference(const Interval &interval, const Interval &other) {
	if (interval.defined() && other.defined()) {
		const int originalRounding = fegetround( );
		fesetround(FE_DOWNWARD);
    ap_float lower = -INF;
    if (interval.left > -INF && other.right < INF) {
      if (interval.left < INF && other.right > -INF)
        lower = interval.left - other.right;
      else
        lower = INF;
    }
		fesetround(FE_UPWARD);
    ap_float upper = INF;
    if (interval.right < INF && other.left > -INF) {
      if (interval.right > -INF && other.left < INF)
        upper = interval.right - other.left;
      else
        upper = -INF;
    }
		fesetround(originalRounding);
		return Interval(lower, upper, interval.left_open || other.left_open, interval.right_open || other.right_open);
	} else
		return Interval();
}

ap_float asymptotic_multiplication(ap_float a, ap_float b) {
  if (a == -INF || a == INF) {
    if (b > 0) return a;
    if (b < 0) return -a;
    return 0;
  } else if (b == -INF || b == INF) {
    if (a > 0) return b;
    if (a < 0) return -b;
    return 0;
  } 
  return a * b;
}

Interval asymptotic_multiplication(const Interval &interval, const Interval &other) {
	if (interval.defined() && other.defined()) {
		const int originalRounding = fegetround( );

		// determine lower bound and leftopen
		fesetround(FE_DOWNWARD);
    ap_float lower = asymptotic_multiplication(interval.left, other.left);
		bool leftopen = interval.left_open || other.left_open;
    ap_float better = asymptotic_multiplication(interval.left, other.right);
		if (better < lower) {
			lower = better;
			leftopen = interval.left_open || other.right_open;
		}
		if (better == lower && leftopen && !interval.left_open && !other.right_open)
			leftopen = false;
    better = asymptotic_multiplication(interval.right, other.left);
		if (better < lower) {
			lower = better;
			leftopen = interval.right_open || other.left_open;
		}
		if (better == lower && leftopen && !interval.right_open && !other.left_open)
			leftopen = false;
    better = asymptotic_multiplication(interval.right, other.right);
		if (better < lower) {
			lower = better;
			leftopen = interval.right_open || other.right_open;
		}
		if (better == lower && leftopen && !interval.right_open && !other.right_open)
			leftopen = false;

		// determine upper bound and rightopen
		fesetround(FE_UPWARD);
    ap_float upper = asymptotic_multiplication(interval.left, other.left);
		bool rightopen = interval.left_open || other.left_open;
		better = interval.left * other.right;
		if (better > upper) {
			upper = better;
			rightopen = interval.left_open || other.right_open;
		}
		if (better == upper && rightopen && !interval.left_open && !other.right_open)
			rightopen = false;
    better = asymptotic_multiplication(interval.right, other.left);
		if (better > upper) {
			upper = better;
			rightopen = interval.right_open || other.left_open;
		}
		if (better == upper && rightopen && !interval.right_open && !other.left_open)
			rightopen = false;
    better = asymptotic_multiplication(interval.right, other.right);
		if (better > upper) {
			upper = better;
			rightopen = interval.right_open || other.right_open;
		}
		if (better == upper && rightopen && !interval.right_open && !other.right_open)
			rightopen = false;
		fesetround(originalRounding);
		return Interval(lower, upper, leftopen, rightopen);
	} else
		return Interval();
}

ap_float asymptotic_division(ap_float a, ap_float b) {
  if (a == -INF || a == INF) {
    if (b > 0) return a;
    if (b < 0) return -a;
    return 0;
  } else if (b == -INF || b == INF) {
    if (a > 0) return b;
    if (a < 0) return -b;
    return 0;
  } 
  return a * b;
}

Interval asymptotic_division(const Interval &interval, const Interval &other) {
	// currently division by [0,0] is [-inf, inf] should we change it to [undef]?
	if (interval.defined() && other.defined()) {
		ap_float lower = -INF;
		ap_float upper = INF;
		bool leftopen = false;
		bool rightopen = false;
		if (other.right <= 0 || other.left >= 0) {
			const int originalRounding = fegetround( );
			if (other.right != 0) {
				fesetround(FE_DOWNWARD);
        lower = (other.right == INF || other.right == -INF) ? 0 : 1/other.right;
				leftopen = other.right_open;
			}
			if (other.left != 0) {
				fesetround(FE_UPWARD);
        upper = (other.left == INF || other.left == -INF) ? 0 : 1/other.left;
				rightopen = other.left_open;
			}
			fesetround(originalRounding);
      if (interval.left == INF && lower <= 0) {

      }
			Interval inverseOther = Interval(lower, upper, leftopen, rightopen);
			Interval new_interval = asymptotic_multiplication(interval, inverseOther);
      if (interval.right == INF) {
        if (other.right >= 0) {
          new_interval = new_interval || Interval(INF);
          new_interval.right_open = true;
        }
        if (other.left < 0) {
          new_interval = new_interval || Interval(-INF);
          new_interval.left_open = true;
        }
      }
      if (interval.left == -INF) {
        if (other.right >= 0) {
          new_interval = new_interval || Interval(-INF);
          new_interval.left_open = true;
        }
        if (other.left < 0) {
          new_interval = new_interval || Interval(INF);
          new_interval.right_open = true;
        }
      }
      return new_interval;
		} else // 0 is contained in interval
			return Interval(-INF, INF);
	} else
		return Interval();
}

}