/* -*-C++-*- */
/*
 * Expressions.
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
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "refcount.h"
#include "functions.h"
#include "terms.h"
#include "rational.h"


namespace PPDDL {

struct Fluent;
/* ====================================================================== */
/* ValueMap */
using ValueMap = std::map<const Fluent*, Rational>;


/* ====================================================================== */
/* Expression. */
/*
 * An abstract expression.
 */
struct Expression : public RCObject {
  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const = 0;

  virtual Rational const& value() const {
    static Rational r(0,1);
    throw std::logic_error("called value() from Expression");
    return r;
  }

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
                                          const ValueMap& values) const = 0;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const Expression& e);
};

/* Output operator for expressions. */
std::ostream& operator<<(std::ostream& os, const Expression& e);

using VecExpression = std::vector<Expression const*>;

/* ====================================================================== */
/* Value */

/*
 * A constant value.
 */
struct Value : public Expression {
  /* Constructs a constant value. */
  explicit Value(const Rational& value) : value_(value) {}

  /* Returns the value of this expression. */
  const Rational& value() const override { return value_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Value& instantiation(const SubstitutionMap& subst,
                                     const ValueMap& values) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* The value. */
  Rational value_;
};


/* ====================================================================== */
/* Fluent */

/*
 * A fluent.
 */
struct Fluent : public Expression {
  /* Returns a fluent with the given function and terms. */
  static const Fluent& make(const Function& function, const TermList& terms);

  /* Deletes this fluent. */
  virtual ~Fluent();

  /* Returns the function of this fluent. */
  const Function& function() const { return function_; }

  /* Returns the terms of this fluent. */
  const TermList& terms() const { return terms_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this fluent subject to the given substitution. */
  const Fluent& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
                                          const ValueMap& values) const;

  std::string name() const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Less-than comparison function object for fluents. */
  struct FluentLess
    : public std::binary_function<const Fluent*, const Fluent*, bool> {
    /* Comparison function. */
    bool operator()(const Fluent* f1, const Fluent* f2) const;
  };

  /* A table of fluents. */
  using FluentTable = std::set<const Fluent*, FluentLess>;

  /* Table of fluents. */
  static FluentTable fluents;

  /* Function of this fluent. */
  Function function_;
  /* Terms of this fluent. */
  TermList terms_;

  /* Constructs a fluent with the given function. */
  explicit Fluent(const Function& function) : function_(function) {}

  /* Adds a term to this fluent. */
  void add_term(Term term) { terms_.push_back(term); }
};


/* ====================================================================== */
/* Computation */

/*
 * A computation expression.
 */
struct Computation : public Expression {
  /* Deletes this computation. */
  virtual ~Computation();

  /* Returns the first operand for this computation. */
  const Expression& operand1() const { return *operand1_; }

  /* Returns the second operand for this computation. */
  const Expression& operand2() const { return *operand2_; }

 protected:
  /* Constructs a computation. */
  Computation(const Expression& operand1, const Expression& operand2);

 private:
  /* The first operand for this computation. */
  const Expression* operand1_;
  /* The second operand for this computation. */
  const Expression* operand2_;
};


/* ====================================================================== */
/* Addition */

/*
 * An addition.
 */
struct Addition : public Computation {
  /* Returns an addition of the two expressions. */
  static const Expression& make(const Expression& term1,
                                const Expression& term2);

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
                                          const ValueMap& values) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs an addition. */
  Addition(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction.
 */
struct Subtraction : public Computation {
  /* Returns a subtraction of the two expressions. */
  static const Expression& make(const Expression& term1,
                                const Expression& term2);

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
                                          const ValueMap& values) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a subtraction. */
  Subtraction(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication.
 */
struct Multiplication : public Computation {
  /* Returns a multiplication of the two expressions. */
  static const Expression& make(const Expression& factor1,
                                const Expression& factor2);

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
                                          const ValueMap& values) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a multiplication. */
  Multiplication(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Division */

/*
 * A division.
 */
struct Division : public Computation {
  /* Returns a division of the two expressions. */
  static const Expression& make(const Expression& factor1,
                                const Expression& factor2);

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
                                          const ValueMap& values) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a division. */
  Division(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


}  // namespace PPDDL
#endif /* EXPRESSIONS_H */
