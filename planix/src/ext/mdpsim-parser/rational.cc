/* -*-C++-*- */
/*
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
#include "rational.h"
#include <cstdlib>
#include <stdexcept>


namespace PPDDL {

/* ====================================================================== */
/* Returns the greatest common divisor of the two integers. */
static int64_t gcd(int64_t n, int64_t m) {
  int64_t a = std::abs(n);
  int64_t b = std::abs(m);
  while (b > 0) {
    int64_t c = b;
    b = a % b;
    a = c;
  }
  assert(a > 0);
  return a;
}


bool Rational::isNormalized() const {
  return (denominator_ > 0)
      && (gcd(numerator_, denominator_) == 1)
      && (std::abs(numerator_) < MAX_INT32)
      && (denominator_ < MAX_INT32);
}


bool Rational::approximate(unsigned max_bits) {
  assert(denominator_ > 0);
  double log2_num = std::log2(std::abs(numerator_));
  double log2_den = std::log2(denominator_);
  double max_log2 = (log2_num > log2_den ? log2_num : log2_den);
  int64_t shift = std::ceil(max_log2 - max_bits);
  if (shift <= 0)
    return false;

  int sign = numerator_ < 0 ? -1 : 1;
  numerator_ = sign * ((sign * numerator_) >> shift);
  denominator_ = denominator_ >> shift;
  if (denominator_ == 0)
    denominator_ = 1;
  assert(denominator_ > 0);
  assert(sign * numerator_ >= 0);
  return true;
}

/* Rational */
void Rational::normalize() {
  if (denominator_ == 0) {
    throw std::runtime_error("zero denominator");
  }
  if (denominator_ < 0) {
    denominator_ *= -1;
    numerator_ *= -1;
  }

  int64_t d = gcd(numerator_, denominator_);
  numerator_ /= d;
  denominator_ /= d;

  if (std::abs(numerator_) >= MAX_INT32 || denominator_ >= MAX_INT32) {
#ifndef NDEBUG
    int64_t original_num = numerator_;
    int64_t original_dem = denominator_;
#endif
//    assert(std::ceil(std::log2(MAX_INT32)) == 31);
    bool loss_of_precision = approximate(31);
    // loss of precision must happen since numerator_ or denominator_ are greater
    // than or equal to MAX_INT32
    assert(loss_of_precision);

    // Due to the loss of precision the numerator and denominator might need to
    // be simplified again
    int64_t d = gcd(numerator_, denominator_);
    numerator_ /= d;
    denominator_ /= d;

#ifndef NDEBUG
    static bool warning_printed_ = false;
    if (!warning_printed_) {
      std::cout
        << "Overflow/Underflow detected for " << original_num << "/" << original_dem
        << ". Representing it as " << *this << ". Error = "
        << std::abs(double(*this) - double(original_num)/original_dem)
        << std::endl;
#ifndef RATIONAL_PRINT_ALL_OVERFLOW
      std::cout
        << "All other overflow while parsing messages will be omitted. To print "
        << "all messages, compile with option -DRATIONAL_PRINT_ALL_OVERFLOW\n";
      warning_printed_ = true;
#endif
    }
#endif
  }
  assert(denominator_ > 0);
  assert(std::abs(numerator_) < MAX_INT32);
  assert(denominator_ < MAX_INT32);
}


/* Returns the multipliers for the two integers. */
std::pair<int64_t, int64_t> Rational::multipliers(int64_t n, int64_t m) {
  assert(std::abs(n) < MAX_INT32);
  assert(std::abs(m) < MAX_INT32);

  int64_t gcd_n_m = gcd(n, m);
  assert(gcd_n_m < MAX_INT32);

  // More stable than using lcm. For instance, if n and m are large primes,
  // lcm is n * m which will overflow, while the multipliers are trivially
  // m and n. It doesn't solve the overflowing problem but it postpone it to
  // the operation using multipliers
  return std::make_pair(m/gcd_n_m, n/gcd_n_m);
}


/* Constructs a rational number. */
Rational::Rational(int64_t n, int64_t d) : numerator_(n), denominator_(d) {
  // Normalize will make sure that n and d are in the acceptable range
  normalize();
}

Rational::Rational(double f) : numerator_(0), denominator_(1) {
  doubleToRational(f);
}


void Rational::doubleToRational(double f) {
  int sign = f < 0 ? -1 : 1;
  f *= sign;

  assert(f >= 0);

  denominator_ = 1;
  numerator_ = int64_t(floor(f)); // 2.3 -> 2
  double f_prime = f - floor(f);  // 2.3 -> 0.3

  while (f_prime > 0) {
    assert(f_prime < 1);
    if (denominator_ >= MAX_INT64 / 10) {
      // OVERFLOW
      break;
    }
    denominator_ *= 10;

    f_prime *= 10;
    if (numerator_ >= (MAX_INT64 - floor(f_prime))/10) {
      // OVERFLOW
      break;
    }
    numerator_ = 10 * numerator_ + floor(f_prime);
    f_prime -= floor(f_prime);
  }

  numerator_ *= sign;
  normalize();
}


/* Constructs a rational number. */
Rational::Rational(const char* s) : numerator_(0), denominator_(1) {
  const char* si = s;
  for (; *si != '\0' && *si != '.' && *si != '/'; si++) {
    numerator_ = 10*numerator_ + (*si - '0');
  }
  if (*si == '/') {
    denominator_ = 0;
    for (si++; *si != '\0'; si++) {
      denominator_ = 10*denominator_ + (*si - '0');
    }
    if (denominator_ == 0) {
      throw std::runtime_error("division by zero");
    }
  } else if (*si == '.') {
    int sign = numerator_ < 0 ? -1 : 1;
    numerator_ *= sign;
    denominator_ = 1;
    for (si++; *si != '\0'; si++) {
      int next_digit = *si - '0';
      assert(numerator_ >= 0);
      assert(denominator_ > 0);
      if (numerator_ > (MAX_INT64 - next_digit)/10 || denominator_ > MAX_INT64/10) {
        // If we continue, the int64_t will overflow
        break;
      }
      numerator_ = 10 * numerator_ + next_digit;
      denominator_ *= 10;
    }
    numerator_ *= sign;
  }
  normalize();
}


/* Less-than comparison operator for rational numbers. */
bool operator<(const Rational& q, const Rational& p) {
  auto m = Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator() * m.first < p.numerator() * m.second;
}


/* Less-than-or-equal comparison operator for rational numbers. */
bool operator<=(const Rational& q, const Rational& p) {
  auto m = Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator() * m.first <= p.numerator() * m.second;
}


/* Equality comparison operator for rational numbers. */
bool operator==(const Rational& q, const Rational& p) {
  assert(q.isNormalized());
  assert(p.isNormalized());
  return q.numerator() == p.numerator() && q.denominator() == p.denominator();
}


/* Addition operator for rational numbers. */
Rational& Rational::operator+=(Rational const& p) {
  auto m = multipliers(denominator(), p.denominator());
  denominator_ = denominator() * m.first;
  numerator_ = numerator() * m.first + p.numerator() * m.second;
  normalize();
  return *this;
}


Rational& Rational::operator-=(Rational const& p) {
  auto m = multipliers(denominator(), p.denominator());
  denominator_ = denominator() * m.first;
  numerator_ = numerator() * m.first - p.numerator() * m.second;
  normalize();
  return *this;
}


Rational operator+(const Rational& q, const Rational& p) {
  Rational rv(q);
  rv += p;
  return rv;
}


/* Subtraction operator for rational numbers. */
Rational operator-(const Rational& q, const Rational& p) {
  Rational rv(q);
  rv -= p;
  return rv;
}


/* Multiplication operator for rational numbers. */
Rational operator*(const Rational& q, const Rational& p) {
  static_assert(int64_t(MAX_INT32) * int64_t(MAX_INT32) <= MAX_INT64,
                "overflow on Rationals will not be detectable");
  // The constructor will normalize the new number and the assert above guarantees
  // that overflow will not happen
  assert(q.isNormalized());
  assert(p.isNormalized());
  return Rational(int64_t(q.numerator()) * int64_t(p.numerator()),
                  int64_t(q.denominator()) * int64_t(p.denominator()));
}


/* Division operator for rational numbers. */
Rational operator/(const Rational& q, const Rational& p) {
  if (p == 0) {
    throw std::runtime_error("division by zero");
  }
  static_assert(int64_t(MAX_INT32) * int64_t(MAX_INT32) <= MAX_INT64,
                "overflow on Rationals will not be detectable");
  // The constructor will normalize the new number and the assert above guarantees
  // that overflow will not happen
  assert(q.isNormalized());
  assert(p.isNormalized());
  return Rational(int64_t(q.numerator()) * int64_t(p.denominator()),
                  int64_t(q.denominator()) * int64_t(p.numerator()));
}


/* Output operator for rational numbers. */
std::ostream& operator<<(std::ostream& os, const Rational& q) {
  os << q.numerator();
  if (q.denominator() != 1) {
    os << '/' << q.denominator();
  }
  return os;
}
}  // namespace PPDDL
