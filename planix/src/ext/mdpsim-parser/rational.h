/* -*-C++-*- */
/*
 * Rational numbers.
 *
 * Copyright 2003-2005 Carnegie Mellon University and Rutgers University
 * Copyright 2007 Håkan Younes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RATIONAL_H
#define RATIONAL_H

#include <iostream>
#include <utility>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <limits>


#define MAX_INT32 std::numeric_limits<int32_t>::max()
#define MAX_INT64 std::numeric_limits<int64_t>::max()



namespace PPDDL {

/*****************************************************************************
 
 Implementation of Rational Numbers using limited precision. For an unlimited
 precision implementation, use cpp_rational from boost::multiprecision

 boost/rational.hpp could be used and certainly would improve most of the method,
 however, boost/rational does not check for under and overflow. Therefore, it
 cannot be used as a drop-in to this implementation, instead, it needs to be
 wrapped with under and overflow checks and simplifications.

 The idea of this class is to represent rationals with int32_t numerators and
 denominator (i.e., 32 bits for each) but store and operate them using int64_t
 (i.e., 64 bits integers). This way under and overflow does not happen during
 the operations and only need to be checked once converted back to the range
 of int32_t (this is done by the method approximate).

 *****************************************************************************/
struct Rational {
  /* Returns the multipliers for the two integers. */
  // return is smaller than n and m
  static std::pair<int64_t, int64_t> multipliers(int64_t n, int64_t m);

  /* Constructs a rational number. */
  Rational(int32_t n = 0) : numerator_(n), denominator_(1) {}

  /* Constructs a rational number. */
  Rational(int64_t n, int64_t m);

  /* Constructs a rational number. */
  Rational(const char* s);

  Rational(double f);

  /* Returns the numerator of this rational number. */
  int32_t numerator() const {
    assert(std::abs(numerator_) < MAX_INT32);
    return numerator_;
  }

  /* Returns the denominator of this rational number. */
  int32_t denominator() const {
    assert(denominator_ > 0);
    assert(denominator_ < MAX_INT32);
    return denominator_;
  }


  operator double() const { return numerator()/double(denominator()); }

  bool operator==(int i) const {
    return denominator() == 1 && numerator() == i;
  }

  Rational& operator+=(Rational const& r);
  Rational& operator-=(Rational const& r);

  bool isNormalized() const;
  void normalize();

  // Approximate the current Rational to one in which the absolute value of the
  // numerator and denominator are at most 2^{max_bits}. Returns true if the
  // number was changed; therefore an approximation error is incurred whenever
  // true is returned
  bool approximate(unsigned max_bits);

 private:
  void doubleToRational(double f);


  // See comment on no top of rational.h
  int64_t numerator_;
  int64_t denominator_;
};


// Hash function for Rationals using boost standard
inline std::size_t hash_value(Rational const& r) {
  return std::hash<double>{}(static_cast<double>(r));
}

/* Less-than comparison operator for rational numbers. */
bool operator<(const Rational& q, const Rational& p);
inline bool operator<(const Rational& q, int p) { return q < Rational(p); }
inline bool operator<(const Rational& q, double p) { return double(q) < p; }


/* Less-than-or-equal comparison operator for rational numbers. */
bool operator<=(const Rational& q, const Rational& p);
inline bool operator<=(const Rational& q, int p) { return q <= Rational(p); }
inline bool operator<=(const Rational& q, double p) { return double(q) <= p; }


/* Equality comparison operator for rational numbers. */
bool operator==(const Rational& q, const Rational& p);


/* Inequality comparison operator for rational numbers. */
inline bool operator!=(const Rational& q, const Rational& p) { return !(q == p); }
inline bool operator!=(const Rational& q, int p) { return q != Rational(p); }
inline bool operator!=(const Rational& q, double p) { return double(q) != p; }


/* Greater-than-or-equal comparison operator for rational numbers. */
inline bool operator>=(const Rational& q, const Rational& p) { return p <= q; }
inline bool operator>=(const Rational& q, int p) { return Rational(p) <= q; }
inline bool operator>=(const Rational& q, double p) { return p <= double(q); }


/* Greater-than comparison operator for rational numbers. */
inline bool operator>(const Rational& q, const Rational& p) { return p < q; }
inline bool operator>(const Rational& q, int p) { return Rational(p) < q; }
inline bool operator>(const Rational& q, double p) { return p < double(q); }


/* Addition operator for rational numbers. */
Rational operator+(const Rational& q, const Rational& p);
inline Rational operator+(const Rational& q, int p) { return q + Rational(p); }
inline double operator+(const Rational& q, double f) { return double(q) + f; }


/* Subtraction operator for rational numbers. */
Rational operator-(const Rational& q, const Rational& p);
inline Rational operator-(const Rational& q, int p) { return q - Rational(p); }
inline double operator-(const Rational& q, double f) { return double(q) - f; }


/* Multiplication operator for rational numbers. */
Rational operator*(const Rational& q, const Rational& p);
inline Rational operator*(const Rational& q, int p) { return q * Rational(p); }
inline double operator*(const Rational& q, double f) { return double(q) * f; }


/* Division operator for rational numbers. */
Rational operator/(const Rational& q, const Rational& p);
inline Rational operator/(const Rational& q, int p) { return q / Rational(p); }
inline double operator/(const Rational& q, double f) { return double(q) / f; }

/* Output operator for rational numbers. */
std::ostream& operator<<(std::ostream& os, const Rational& q);


}  // namespace PPDDL
#endif /* RATIONAL_H */
