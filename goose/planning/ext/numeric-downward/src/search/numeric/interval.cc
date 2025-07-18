#include "interval.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON

using namespace std;


void Interval::dump() const {
	cout << "[" << left << "; " << right << "]" <<endl;
}

Interval operator ||(const Interval& interval, const Interval& other) {
	if (interval.defined() && other.defined()) {
		Interval ret = interval;
		if (interval.left > other.left) {
			ret.left = other.left;
			ret.left_open = other.left_open;
		} else if (interval.left == other.left) {
			ret.left_open = interval.left_open && other.left_open;
		}
		if (interval.right <  other.right) {
			ret.right = other.right;
			ret.right_open = other.right_open;
		} else if (interval.right == other.right) {
			ret.right_open = interval.right_open && other.right_open;
		}
		return ret;
	} else
		return Interval();
}

Interval operator &&(const Interval& interval, const Interval& other) {
	Interval ret = interval;
//	if (DEBUG) cout << "Intersecting " << ret << " with " << other << endl;
	if (interval.defined() && other.defined()) {
		if (interval.left < other.left) {
			ret.left = other.left;
			ret.left_open = other.left_open;
		} else if (interval.left == other.left) {
			ret.left_open = interval.left_open || other.left_open;
		}
//		if (DEBUG) cout << ret << " <2 (adjusted left bound)" << endl;
		if (interval.right > other.right) {
			ret.right = other.right;
			ret.right_open = other.right_open;
		} else if (interval.right == other.right) {
			ret.right_open = interval.right_open || other.right_open;
		}
//		if (DEBUG) cout << ret << " <3 (adjusted right bound)" << endl;
		if ((ret.left < ret.right) || (ret.left == ret.right && !ret.left_open && !ret.right_open))
			return ret;
	}
//	else cout << " one is undef" << endl;
	if (DEBUG) cout << "Warning: empty intersection of " << interval.precise_str() << " && " << other.precise_str() << endl;
	return Interval();
}


bool operator ==(const Interval& interval, const Interval& other) {
	return((interval.left == other.left)
			&& (interval.right == other.right)
			&& (interval.left_open == other.left_open)
			&& (interval.right_open == other.right_open));
}

bool operator !=(const Interval& interval, const Interval& other) {
	return !(interval == other);
}

ostream& operator<<(ostream &os, const Interval &interval) {
	if (interval.defined()) {
		if (interval.left_open)
			os << "(";
		else
			os << "[";
		os << std::to_string(interval.left) << ", " << std::to_string(interval.right);
		if (interval.right_open)
			os << ")";
		else
			os << "]";
	}
	else
		os << "[undef]";
    return os;
}

Interval operator+ (const Interval &interval, const Interval &other) {
	if (interval.defined() && other.defined()) {
		const int originalRounding = fegetround( );
		fesetround(FE_DOWNWARD);
		ap_float lower = interval.left + other.left;
		fesetround(FE_UPWARD);
		ap_float upper = interval.right + other.right;
		fesetround(originalRounding);
		return Interval(lower, upper, interval.left_open || other.left_open, interval.right_open || other.right_open);
	} else
		return Interval();

}

Interval operator- (const Interval &interval, const Interval &other) {
	if (interval.defined() && other.defined()) {
		const int originalRounding = fegetround( );
		fesetround(FE_DOWNWARD);
		ap_float lower = interval.left - other.right;
		fesetround(FE_UPWARD);
		ap_float upper = interval.right - other.left;
		fesetround(originalRounding);
		return Interval(lower, upper, interval.left_open || other.left_open, interval.right_open || other.right_open);
	} else
		return Interval();

}

Interval operator* (const Interval &interval, const Interval &other) {
	if (interval.defined() && other.defined()) {
		const int originalRounding = fegetround( );

		// determine lower bound and leftopen
		fesetround(FE_DOWNWARD);
		ap_float lower = interval.left * other.left;
		bool leftopen = interval.left_open || other.left_open;
		ap_float better = interval.left * other.right;
		if (better < lower) {
			lower = better;
			leftopen = interval.left_open || other.right_open;
		}
		if (better == lower && leftopen && !interval.left_open && !other.right_open)
			leftopen = false;
		better = interval.right * other.left;
		if (better < lower) {
			lower = better;
			leftopen = interval.right_open || other.left_open;
		}
		if (better == lower && leftopen && !interval.right_open && !other.left_open)
			leftopen = false;
		better = interval.right * other.right;
		if (better < lower) {
			lower = better;
			leftopen = interval.right_open || other.right_open;
		}
		if (better == lower && leftopen && !interval.right_open && !other.right_open)
			leftopen = false;

		// determine upper bound and rightopen
		fesetround(FE_UPWARD);
		ap_float upper = interval.left * other.left;
		bool rightopen = interval.left_open || other.left_open;
		better = interval.left * other.right;
		if (better > upper) {
			upper = better;
			rightopen = interval.left_open || other.right_open;
		}
		if (better == upper && rightopen && !interval.left_open && !other.right_open)
			rightopen = false;
		better = interval.right * other.left;
		if (better > upper) {
			upper = better;
			rightopen = interval.right_open || other.left_open;
		}
		if (better == upper && rightopen && !interval.right_open && !other.left_open)
			rightopen = false;
		better = interval.right * other.right;
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

Interval operator/ (const Interval &interval, const Interval &other) {
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
				lower = 1/other.right;
				leftopen = other.right_open;
			}
			if (other.left != 0) {
				fesetround(FE_UPWARD);
				upper = 1/other.left;
				rightopen = other.left_open;
			}
			fesetround(originalRounding);
			Interval inverseOther = Interval(lower, upper, leftopen, rightopen);
			return interval * inverseOther;
		} else // 0 is contained in interval
			return Interval(-INF, INF);
	} else
		return Interval();
}

bool Interval::extends(Interval other) {
	return ((left < other.left)
		||  (right > other.right)
		||  ((left == other.left) && !left_open && other.left_open)
		||  ((right == other.right) && !right_open && other.right_open));
}

Interval::Interval() : left(INF), right(-INF), left_open(false), right_open(false) {}

bool Interval::defined() const {
	if (left<=right || (left==INF && right == -INF)) {} else {
		printf(" left = %.16e right = %.16e ", left, right);
	}
	assert(left<=right || (left==INF && right == -INF));
	return (left<=right);
}

Interval compute(const Interval& interval, const f_operator fop,
		const Interval& other) {
	switch (fop) {
	case  assign:
		return other;
		break;
	case scale_up:
		return interval * other;
		break;
	case scale_down:
		return interval / other;
		break;
	case increase:
		return interval + other;
		break;
	case decrease:
		return interval - other;
		break;
	default: assert(false);
		return Interval();
	}
}

Interval compute(const Interval& interval, const cal_operator calop,
		const Interval& other) {
	switch (calop) {
	case sum:
		return interval + other;
		break;
	case diff:
		return interval - other;
		break;
	case mult:
		return interval * other;
		break;
	case divi:
		return interval / other;
		break;
	default: assert(false);
		return Interval();
	}
}

bool Interval::contains(ap_float value) const {
	return((left < value && right > value) || (!left_open && left == value) || (!right_open && right == value));
}

string Interval::precise_str() const {
	char buffer[60];
	sprintf(buffer, "%c%.16e, %.16e%c", (left_open?'(':'['), left, right, (right_open?')':']'));
//	cout << "test test test " << endl;
//	cout << buffer << endl;
	string ret_val = buffer;
//	cout << "zum zweiten " << ret_val << endl;
	return ret_val;
//	return "%c %.16e , %.16e %c", (left_open?'(':'['), left, right, (right_open?')':']');
}
