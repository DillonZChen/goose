#ifndef INTERVAL_H
#define INTERVAL_H

#include "../globals.h" // ap_float, INF

#include <algorithm>
#include <cassert>
#include <iosfwd>


struct Interval {
	ap_float left;
	ap_float right;
	bool left_open;
	bool right_open;

	Interval(ap_float _left, ap_float _right, bool _left_open, bool _right_open):
		left(_left),
		right(_right),
		left_open(_left_open),
		right_open(_right_open)
	{if (!(left <= right)) {
		this->~Interval();
		new (this) Interval();
		}
	}

	Interval(ap_float _left, ap_float _right):
		Interval(_left, _right, false, false) {};

	Interval(ap_float _number) : Interval(_number,_number) {assert(defined());};
	Interval(); // returns an undefined interval
	void dump() const;
	bool extends(Interval other);
	bool defined() const;
	bool contains(ap_float value) const;
	std::string precise_str() const;
};

Interval compute(const Interval &interval, const f_operator fop, const Interval &other);
Interval compute(const Interval &interval, const cal_operator calop, const Interval &other);

Interval operator+(const Interval &interval, const Interval &other);
Interval operator-(const Interval &interval, const Interval &other);
Interval operator*(const Interval &interval, const Interval &other);
Interval operator/(const Interval &interval, const Interval &other);
Interval operator||(const Interval &interval, const Interval &other); // convex union
Interval operator&&(const Interval &interval, const Interval &other); // intersection
bool operator==(const Interval &interval, const Interval &other);
bool operator!=(const Interval &interval, const Interval &other);
std::ostream& operator<<(std::ostream &os, const Interval &intervall);

#endif
