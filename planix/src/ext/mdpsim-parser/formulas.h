/* -*-C++-*- */
/*
 * Formulas.
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
#ifndef FORMULAS_H
#define FORMULAS_H

#include <iostream>
#include <set>
#include <vector>

#include "expressions.h"
#include "refcount.h"
#include "predicates.h"
#include "terms.h"

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif



namespace PPDDL {

struct Atom;
/* ====================================================================== */
/* AtomSet */
/*
 * A set of atoms.
 */
using AtomSet = std::set<Atom const*>;


/* ====================================================================== */
/* StateFormula */
/*
 * A state formula.
 */
struct StateFormula : public RCObject {
  /* A formula true in every state. */
  static const StateFormula& TRUE;
  /* A formula false in every state. */
  static const StateFormula& FALSE;

  /* Tests if this state formula is a tautology. */
  bool tautology() const { return this == &TRUE; }

  /* Tests if this state formula is a contradiction. */
  bool contradiction() const { return this == &FALSE; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const = 0;

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const = 0;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const = 0;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const StateFormula& f);
};

/* Conjunction operator for state formulas. */
const StateFormula& operator&&(const StateFormula& f1, const StateFormula& f2);

/* Disjunction operator for state formulas. */
const StateFormula& operator||(const StateFormula& f1, const StateFormula& f2);

/* Output operator for state formulas. */
std::ostream& operator<<(std::ostream& os, const StateFormula& f);


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
using FormulaList = std::vector<const StateFormula*>;


/* ====================================================================== */
/* Atom */

struct Atom;

/* Less-than comparison function object for atoms. */
struct AtomLess
  : public std::binary_function<const Atom*, const Atom*, bool> {
  /* Comparison function. */
  bool operator()(const Atom* a1, const Atom* a2) const;
};

/* A table of atoms. */
using AtomTable = std::set<const Atom*, AtomLess>;

/*
 * An atom.
 */
struct Atom : public StateFormula {
  /* Returns an atom with the given predicate and terms. */
  // LEGACY(FWT): When the second item of the pair is true, then this atom is a
  // new atom and it should be deleted if not used. This is not a perfect
  // solution and a better ownership should be implemented. However, this fix
  // the current leaks that I found.
  static std::pair<Atom const&, bool> make(Predicate predicate, const TermList& terms);

  /* Deletes this atom. */
  virtual ~Atom();

  /* Returns the predicate of this atom. */
  Predicate predicate() const { return predicate_; }

  /* Returns the terms of this atom. */
  const TermList& terms() const { return terms_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return holds(atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns this atom subject to the given substitution. */
  const Atom& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

  // This method is used by Domain to check for memory leaks
  static size_t sizeOfAtomTable() {
    if (atoms_.size() == 0) {
      std::cout << "No atoms\n";
      return 0;
    }
    std::cout << "Number of atoms: " << atoms_.size() << std::endl;
    for (Atom const* const& atm : atoms_) {
      std::cout << "  " << *atm << std::endl;
    }
    return atoms_.size();
  }

  static AtomTable const& getAtomTable() { return atoms_; }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Table of atoms. */
  static AtomTable atoms_;

  /* Predicate of this atom. */
  Predicate predicate_;
  /* Terms of this atom. */
  TermList terms_;

  /* Constructs an atom with the given predicate. */
  explicit Atom(Predicate predicate) : predicate_(predicate) {}

  /* Adds a term to this atom. */
  void add_term(const Term& term) { terms_.push_back(term); }
};


/* ====================================================================== */
/* Equality */

/*
 * Equality formula.
 */
struct Equality : public StateFormula {
  /* Returns an equality of the two terms. */
  static const StateFormula& make(const Term& term1, const Term& term2);

  /* Returns the first term of this equality formula. */
  const Term& term1() const { return term1_; }

  /* Returns the second term of this equality formula. */
  const Term& term2() const { return term2_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const;

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* First term of equality formula. */
  Term term1_;
  /* Second term of equality formula. */
  Term term2_;

  /* Constructs an equality formula. */
  Equality(const Term& term1, const Term& term2)
    : term1_(term1), term2_(term2) {}
};


/* ====================================================================== */
/* Comparison */

/*
 * A comparison formula.
 */
struct Comparison : public StateFormula {
  /* Deletes this comparison. */
  virtual ~Comparison();

  /* Returns the first expression of this comparison. */
  const Expression& expr1() const { return *expr1_; }

  /* Returns the second expression of this comparison. */
  const Expression& expr2() const { return *expr2_; }

 protected:
  /* Constructs a comparison. */
  Comparison(const Expression& expr1, const Expression& expr2);

 private:
  /* The first expression. */
  const Expression* expr1_;
  /* The second expression. */
  const Expression* expr2_;
};


/* ====================================================================== */
/* LessThan */

/*
 * A less-than comparison.
 */
struct LessThan : public Comparison {
  /* Returns a less-than comparison of the two expressions. */
  static const StateFormula& make(const Expression& expr1,
                                  const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return holds(atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a less-than comparision. */
  LessThan(const Expression& expr1, const Expression& expr2)
    : Comparison(expr1, expr2) {}
};


/* ====================================================================== */
/* LessThanOrEqualTo */

/*
 * A less-than-or-equal-to comparison.
 */
struct LessThanOrEqualTo : public Comparison {
  /* Returns a less-than-or-equal-to comparison of the two expressions. */
  static const StateFormula& make(const Expression& expr1,
                                  const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return holds(atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a less-than-or-equal-to comparision. */
  LessThanOrEqualTo(const Expression& expr1, const Expression& expr2)
    : Comparison(expr1, expr2) {}
};


/* ====================================================================== */
/* EqualTo */

/*
 * A equal-to comparison.
 */
struct EqualTo : public Comparison {
  /* Returns a equal-to comparison of the two expressions. */
  static const StateFormula& make(const Expression& expr1,
                                  const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return holds(atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a equal-to comparision. */
  EqualTo(const Expression& expr1, const Expression& expr2)
    : Comparison(expr1, expr2) {}
};


/* ====================================================================== */
/* GreaterThanOrEqualTo */

/*
 * A greater-than-or-equal-to comparison.
 */
struct GreaterThanOrEqualTo : public Comparison {
  /* Returns a greater-than-or-equal-to comparison of the two expressions. */
  static const StateFormula& make(const Expression& expr1,
                                  const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return holds(atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a greater-than-or-equal-to comparision. */
  GreaterThanOrEqualTo(const Expression& expr1, const Expression& expr2)
    : Comparison(expr1, expr2) {}
};


/* ====================================================================== */
/* GreaterThan */

/*
 * A greater-than comparison.
 */
struct GreaterThan : public Comparison {
  /* Returns a greater-than comparison of the two expressions. */
  static const StateFormula& make(const Expression& expr1,
                                  const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return holds(atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a greater-than comparision. */
  GreaterThan(const Expression& expr1, const Expression& expr2)
    : Comparison(expr1, expr2) {}
};


/* ====================================================================== */
/* Negation */

/*
 * A negated state formula.
 */
struct Negation : public StateFormula {
  /* Returns the negation of the given state formula. */
  static const StateFormula& make(const StateFormula& formula);

  /* Deletes this negation. */
  virtual ~Negation();

  /* Returns the negand of this negation. */
  const StateFormula& negand() const { return *negand_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return !negand().holds(terms, atoms, values);
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const {
    return !negand().holds(atoms, values);
  }

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* The negand of this negation. */
  const StateFormula* negand_;

  /* Constructs a negated state formula. */
  explicit Negation(const StateFormula& negand);
};


/* ====================================================================== */
/* Conjunction */

/*
 * A conjunction of state formulas.
 */
struct Conjunction : public StateFormula {
  /* Deletes this conjunction. */
  virtual ~Conjunction();

  /* Returns the conjuncts of this conjunction. */
  const FormulaList& conjuncts() const { return conjuncts_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const;

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* The conjuncts. */
  FormulaList conjuncts_;

  /* Constructs a conjunction. */
  Conjunction() {}

  /* Adds a conjunct to this conjunction. */
  void add_conjunct(const StateFormula& conjunct);

  friend const StateFormula& operator&&(const StateFormula& f1,
                                        const StateFormula& f2);
};


/* ====================================================================== */
/* Disjunction */

/*
 * A disjunction of state formulas.
 */
struct Disjunction : public StateFormula {
  /* Deletes this disjunction. */
  virtual ~Disjunction();

  /* Returns the disjuncts of this disjunction. */
  const FormulaList& disjuncts() const { return disjuncts_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const;

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* The disjuncts. */
  FormulaList disjuncts_;

  /* Constructs a disjunction. */
  Disjunction() {}

  /* Adds a disjunct to this disjunction. */
  void add_disjunct(const StateFormula& disjunct);

  friend const StateFormula& operator||(const StateFormula& f1,
                                        const StateFormula& f2);
};


/* ====================================================================== */
/* Quantification */

/*
 * Abstract quantified formula.
 */
struct Quantification : public StateFormula {
  /* Deletes this quantification. */
  virtual ~Quantification();

  /* Returns the quanitfied variables. */
  const VariableList& parameters() const { return parameters_; }

  /* Returns the quantified formula. */
  const StateFormula& body() const { return *body_; }

 protected:
  /* Constructs a quantification. */
  Quantification(const VariableList& parameters, const StateFormula& body);

 private:
  /* Quanitfied variables. */
  VariableList parameters_;
  /* The quantified formula. */
  const StateFormula* body_;
};


/* ====================================================================== */
/* Exists */

/*
 * Existentially quantified state formula.
 */
struct Exists : public Quantification {
  /* Returns an existentially quantified formula. */
  static const StateFormula& make(const VariableList& parameters,
                                  const StateFormula& body);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const;

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const {
    throw std::logic_error("Expecting grounded state formula, i.e., no quantification");
    return false;
  }

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs an existentially quantified formula. */
  Exists(const VariableList& parameters, const StateFormula& body)
    : Quantification(parameters, body) {}
};


/* ====================================================================== */
/* Forall */

/*
 * Universally quantified state formula.
 */
struct Forall : public Quantification {
  /* Returns a universally quantified formula. */
  static const StateFormula& make(const VariableList& parameters,
                                  const StateFormula& body);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const;

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const {
    throw std::logic_error("Expecting grounded state formula, i.e., no quantification");
    return false;
  }

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

 private:
  /* Constructs a universally quantified formula. */
  Forall(const VariableList& parameters, const StateFormula& body)
    : Quantification(parameters, body) {}
};


/* ====================================================================== */
/* AtomList */

/*
 * List of atoms.
 */
using AtomList = std::vector<const Atom*>;


}  // namespace PPDDL
#endif /* FORMULAS_H */
