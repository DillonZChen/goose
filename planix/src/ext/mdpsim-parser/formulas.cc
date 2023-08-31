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
#include "formulas.h"
#include <stack>
#include <stdexcept>
#include <memory>


namespace PPDDL {

/* ====================================================================== */
/* Constant */

/*
 * A state formula with a constant truth value.
 */
struct Constant : public StateFormula {
  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
    return value_;
  }

  // Tests if this grounded state formula holds in a state.
  virtual bool holds(AtomSet const& atoms, ValueMap const& values) const {
    return value_;
  }

  /* Returns an instantiation of this state formula. */
  virtual const Constant& instantiation(const SubstitutionMap& subst,
                                        const TermTable& terms,
                                        const AtomSet& atoms,
                                        const ValueMap& values,
                                        bool state) const {
    return *this;
  }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const {
    os << (value_ ? "(and)" : "(or)");
  }

 private:
  /* Constant representing true. */
  static const Constant TRUE_;
  /* Constant representing false. */
  static const Constant FALSE_;

  /* Value of this constant. */
  bool value_;

  /* Constructs a constant state formula. */
  explicit Constant(bool value) : value_(value) { ref(this); }

  friend struct StateFormula;
};


/* Constant representing true. */
const Constant Constant::TRUE_(true);
/* Constant representing false. */
const Constant Constant::FALSE_(false);


/* ====================================================================== */
/* StateFormula */

/* A formula true in every state. */
const StateFormula& StateFormula::TRUE = Constant::TRUE_;
/* A formula false in every state. */
const StateFormula& StateFormula::FALSE = Constant::FALSE_;


/* Conjunction operator for state formulas. */
const StateFormula& operator&&(const StateFormula& f1,
                               const StateFormula& f2) {
  if (&f1 == &f2) {
    return f1;
  } else if (f1.contradiction()) {
    RCObject::ref(&f2);
    RCObject::destructive_deref(&f2);
    return f1;
  } else if (f2.contradiction()) {
    RCObject::ref(&f1);
    RCObject::destructive_deref(&f1);
    return f2;
  } else if (f1.tautology()) {
    return f2;
  } else if (f2.tautology()) {
    return f1;
  } else {
    Conjunction& conjunction = *new Conjunction();
    const Conjunction* c1 = dynamic_cast<const Conjunction*>(&f1);
    if (c1 != 0) {
      for (FormulaList::const_iterator fi = c1->conjuncts().begin();
           fi != c1->conjuncts().end(); fi++) {
        conjunction.add_conjunct(**fi);
      }
      RCObject::ref(c1);
      RCObject::destructive_deref(c1);
    } else {
      conjunction.add_conjunct(f1);
    }
    const Conjunction* c2 = dynamic_cast<const Conjunction*>(&f2);
    if (c2 != 0) {
      for (FormulaList::const_iterator fi = c2->conjuncts().begin();
           fi != c2->conjuncts().end(); fi++) {
        conjunction.add_conjunct(**fi);
      }
      RCObject::ref(c2);
      RCObject::destructive_deref(c2);
    } else {
      conjunction.add_conjunct(f2);
    }
    return conjunction;
  }
}


/* Disjunction operator for state formulas. */
const StateFormula& operator||(const StateFormula& f1,
                               const StateFormula& f2) {
  if (&f1 == &f2) {
    return f1;
  } else if (f1.tautology()) {
    RCObject::ref(&f2);
    RCObject::destructive_deref(&f2);
    return f1;
  } else if (f2.tautology()) {
    RCObject::ref(&f1);
    RCObject::destructive_deref(&f1);
    return f2;
  } else if (f1.contradiction()) {
    return f2;
  } else if (f2.contradiction()) {
    return f1;
  } else {
    Disjunction& disjunction = *new Disjunction();
    const Disjunction* d1 = dynamic_cast<const Disjunction*>(&f1);
    if (d1 != 0) {
      for (FormulaList::const_iterator fi = d1->disjuncts().begin();
           fi != d1->disjuncts().end(); fi++) {
        disjunction.add_disjunct(**fi);
      }
      RCObject::ref(d1);
      RCObject::destructive_deref(d1);
    } else {
      disjunction.add_disjunct(f1);
    }
    const Disjunction* d2 = dynamic_cast<const Disjunction*>(&f2);
    if (d2 != 0) {
      for (FormulaList::const_iterator fi = d2->disjuncts().begin();
           fi != d2->disjuncts().end(); fi++) {
        disjunction.add_disjunct(**fi);
      }
      RCObject::ref(d2);
      RCObject::destructive_deref(d2);
    } else {
      disjunction.add_disjunct(f2);
    }
    return disjunction;
  }
}


/* Output operator for state formulas. */
std::ostream& operator<<(std::ostream& os, const StateFormula& f) {
  f.print(os);
  return os;
}


/* ====================================================================== */
/* Constant */

/* ====================================================================== */
/* Atom */

/* Table of atoms. */
AtomTable Atom::atoms_;


/* Comparison function. */
bool AtomLess::operator()(const Atom* a1, const Atom* a2) const {
  if (a1->predicate() < a2->predicate()) {
    return true;
  } else if (a2->predicate() < a1->predicate()) {
    return false;
  } else {
    return a1->terms() < a2->terms();
  }
}


/* Returns an atom with the given predicate and terms. */
std::pair<Atom const&, bool> Atom::make(Predicate predicate, const TermList& terms) {
  Atom* atom = new Atom(predicate);
  bool ground = true;
  for (TermList::const_iterator ti = terms.begin(); ti != terms.end(); ti++) {
    atom->add_term(*ti);
    if (ground && (*ti).variable()) {
      ground = false;
    }
  }

  if (!ground) {
    return {*atom, true};
  }

  std::pair<AtomTable::const_iterator, bool> result = atoms_.insert(atom);
  if (!result.second) {
    delete atom;
    // LEGACY(FWT): I'm not sure why the ref_count of *result.first is not increased here
    return {**result.first, false};
  }
  return {*atom, true};
}


/* Deletes this atom. */
Atom::~Atom() {
  AtomTable::const_iterator ai = atoms_.find(this);
  if (*ai == this) {
    atoms_.erase(ai);
  }
}


/* Tests if this state formula holds in the given state. */
bool Atom::holds(AtomSet const& atoms, ValueMap const& values) const {
  return atoms.find(this) != atoms.end();
}


/* Returns this atom subject to the given substitution. */
const Atom& Atom::substitution(const SubstitutionMap& subst) const {
  TermList inst_terms;
  bool substituted = false;
  for (TermList::const_iterator ti = terms().begin();
       ti != terms().end(); ti++) {
    SubstitutionMap::const_iterator si =
      (*ti).variable() ? subst.find((*ti).as_variable()) : subst.end();
    if (si != subst.end()) {
      inst_terms.push_back((*si).second);
      substituted = true;
    } else {
      inst_terms.push_back(*ti);
    }
  }
  if (substituted) {
    // FWT: Ignoring the flag saying if the atom is new (as in the original code)
    // since it seems to not matter
    return make(predicate(), inst_terms).first;
  } else {
    return *this;
  }
}


/* Returns an instantiation of this state formula. */
const StateFormula& Atom::instantiation(const SubstitutionMap& subst,
                                        const TermTable& terms,
                                        const AtomSet& atoms,
                                        const ValueMap& values,
                                        bool state) const {
  if (this->terms().empty()) {
    if (state || PredicateTable::static_predicate(predicate())) {
      if (atoms.find(this) != atoms.end()) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return *this;
    }
  } else {
    TermList inst_terms;
    bool substituted = false;
    size_t objects = 0;
    for (TermList::const_iterator ti = this->terms().begin();
         ti != this->terms().end(); ti++) {
      SubstitutionMap::const_iterator si =
        (*ti).variable() ? subst.find((*ti).as_variable()) : subst.end();
      if (si != subst.end()) {
        inst_terms.push_back((*si).second);
        substituted = true;
        objects++;
      } else {
        inst_terms.push_back(*ti);
        if ((*ti).object()) {
          objects++;
        }
      }
    }
    if (substituted) {
      auto pair_atm_is_new = make(predicate(), inst_terms);
      Atom const& inst_atom = pair_atm_is_new.first;

      if ((state || PredicateTable::static_predicate(predicate()))
          && objects == inst_terms.size())
      {
        bool found_inst_atom = (atoms.find(&inst_atom) != atoms.end());

        // FWT: Either the StateFormula representing TRUE or FALSE is returned,
        // therefore, if inst_atom was newly created, it won't be used anymore
        // and its reference will be lost, thus deleting it.
        if (pair_atm_is_new.second) {
          delete &inst_atom;
        }

        if (found_inst_atom) {
          return TRUE;
        } else {
          return FALSE;
        }
      } else {
        return inst_atom;
      }
    } else {
      return *this;
    }
  }
}


/* Prints this object on the given stream. */
void Atom::print(std::ostream& os) const {
  os << '(' << predicate();
  for (TermList::const_iterator ti = terms().begin();
       ti != terms().end(); ti++) {
    os << ' ' << *ti;
  }
  os << ')';
}


/* ====================================================================== */
/* Equality */

/* Returns an equality of the two terms. */
const StateFormula& Equality::make(const Term& term1, const Term& term2) {
  if (term1 == term2) {
    return TRUE;
  } else if (term1.object() && term2.object() && term1 != term2) {
    return FALSE;
  } else {
    return *new Equality(term1, term2);
  }
}


/* Tests if this state formula holds in the given state. */
bool Equality::holds(const TermTable& terms,
                     const AtomSet& atoms, const ValueMap& values) const {
  throw std::logic_error("Equality::holds not implemented");
}

bool Equality::holds(AtomSet const& atoms, ValueMap const& values) const {
  throw std::logic_error("LEGACY: Equality::holds not implemented");
}

/* Returns an instantiation of this state formula. */
const StateFormula& Equality::instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const {
  Term inst_term1 = term1();
  if (inst_term1.variable()) {
    const SubstitutionMap::const_iterator si =
      subst.find(inst_term1.as_variable());
    if (si != subst.end()) {
      inst_term1 = (*si).second;
    }
  }
  Term inst_term2 = term2();
  if (inst_term2.variable()) {
    const SubstitutionMap::const_iterator si =
      subst.find(inst_term2.as_variable());
    if (si != subst.end()) {
      inst_term2 = (*si).second;
    }
  }
  if (inst_term1 == term1() && inst_term2 == term2()) {
    return *this;
  } else {
    return make(inst_term1, inst_term2);
  }
}


/* Prints this object on the given stream. */
void Equality::print(std::ostream& os) const {
  os << "(= " << term1() << ' ' << term2() << ")";
}


/* ====================================================================== */
/* Comparison */

/* Constructs a comparison. */
Comparison::Comparison(const Expression& expr1, const Expression& expr2)
  : expr1_(&expr1), expr2_(&expr2) {
  ref(expr1_);
  ref(expr2_);
}


/* Deletes this comparison. */
Comparison::~Comparison() {
  destructive_deref(expr1_);
  destructive_deref(expr2_);
}


/* ====================================================================== */
/* LessThan */

/* Returns a less-than comparison of the two expressions. */
const StateFormula& LessThan::make(const Expression& expr1,
                                   const Expression& expr2) {
  const Value* v1 = dynamic_cast<const Value*>(&expr1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&expr2);
    if (v2 != 0) {
      const StateFormula& value = (v1->value() < v2->value()) ? TRUE : FALSE;
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new LessThan(expr1, expr2);
}


/* Tests if this state formula holds in the given state. */
bool LessThan::holds(AtomSet const& atoms, ValueMap const& values) const {
  return expr1().value(values) < expr2().value(values);
}


/* Returns an instantiation of this state formula. */
const StateFormula& LessThan::instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const {
  const Expression& inst_expr1 = expr1().instantiation(subst, values);
  const Expression& inst_expr2 = expr2().instantiation(subst, values);
  if (&inst_expr1 == &expr1() && &inst_expr2 == &expr2()) {
    return *this;
  } else {
    return make(inst_expr1, inst_expr2);
  }
}


/* Prints this object on the given stream. */
void LessThan::print(std::ostream& os) const {
  os << "(< " << expr1() << ' ' << expr2() << ")";
}


/* ====================================================================== */
/* LessThanOrEqualTo */

/* Returns a less-than-or-equal-to comparison of the two expressions. */
const StateFormula& LessThanOrEqualTo::make(const Expression& expr1,
                                            const Expression& expr2) {
  const Value* v1 = dynamic_cast<const Value*>(&expr1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&expr2);
    if (v2 != 0) {
      const StateFormula& value = (v1->value() <= v2->value()) ? TRUE : FALSE;
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new LessThanOrEqualTo(expr1, expr2);
}


/* Tests if this state formula holds in the given state. */
bool LessThanOrEqualTo::holds(AtomSet const& atoms, ValueMap const& values) const {
  return expr1().value(values) <= expr2().value(values);
}


/* Returns an instantiation of this state formula. */
const StateFormula&
LessThanOrEqualTo::instantiation(const SubstitutionMap& subst,
                                 const TermTable& terms,
                                 const AtomSet& atoms,
                                 const ValueMap& values,
                                 bool state) const {
  const Expression& inst_expr1 = expr1().instantiation(subst, values);
  const Expression& inst_expr2 = expr2().instantiation(subst, values);
  if (&inst_expr1 == &expr1() && &inst_expr2 == &expr2()) {
    return *this;
  } else {
    return make(inst_expr1, inst_expr2);
  }
}


/* Prints this object on the given stream. */
void LessThanOrEqualTo::print(std::ostream& os) const {
  os << "(<= " << expr1() << ' ' << expr2() << ")";
}


/* ====================================================================== */
/* EqualTo */

/* Returns a equal-to comparison of the two expressions. */
const StateFormula& EqualTo::make(const Expression& expr1,
                                  const Expression& expr2) {
  const Value* v1 = dynamic_cast<const Value*>(&expr1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&expr2);
    if (v2 != 0) {
      const StateFormula& value = (v1->value() == v2->value()) ? TRUE : FALSE;
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new EqualTo(expr1, expr2);
}


/* Tests if this state formula holds in the given state. */
bool EqualTo::holds(AtomSet const& atoms, ValueMap const& values) const {
  return expr1().value(values) == expr2().value(values);
}


/* Returns an instantiation of this state formula. */
const StateFormula& EqualTo::instantiation(const SubstitutionMap& subst,
                                           const TermTable& terms,
                                           const AtomSet& atoms,
                                           const ValueMap& values,
                                           bool state) const {
  const Expression& inst_expr1 = expr1().instantiation(subst, values);
  const Expression& inst_expr2 = expr2().instantiation(subst, values);
  if (&inst_expr1 == &expr1() && &inst_expr2 == &expr2()) {
    return *this;
  } else {
    return make(inst_expr1, inst_expr2);
  }
}


/* Prints this object on the given stream. */
void EqualTo::print(std::ostream& os) const {
  os << "(= " << expr1() << ' ' << expr2() << ")";
}


/* ====================================================================== */
/* GreaterThanOrEqualTo */

/* Returns a greater-than-or-equal-to comparison of the two expressions. */
const StateFormula& GreaterThanOrEqualTo::make(const Expression& expr1,
                                               const Expression& expr2) {
  const Value* v1 = dynamic_cast<const Value*>(&expr1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&expr2);
    if (v2 != 0) {
      const StateFormula& value = (v1->value() >= v2->value()) ? TRUE : FALSE;
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new GreaterThanOrEqualTo(expr1, expr2);
}


/* Tests if this state formula holds in the given state. */
bool GreaterThanOrEqualTo::holds(AtomSet const& atoms, ValueMap const& values) const {
  return expr1().value(values) >= expr2().value(values);
}


/* Returns an instantiation of this state formula. */
const StateFormula&
GreaterThanOrEqualTo::instantiation(const SubstitutionMap& subst,
                                    const TermTable& terms,
                                    const AtomSet& atoms,
                                    const ValueMap& values,
                                    bool state) const {
  const Expression& inst_expr1 = expr1().instantiation(subst, values);
  const Expression& inst_expr2 = expr2().instantiation(subst, values);
  if (&inst_expr1 == &expr1() && &inst_expr2 == &expr2()) {
    return *this;
  } else {
    return make(inst_expr1, inst_expr2);
  }
}


/* Prints this object on the given stream. */
void GreaterThanOrEqualTo::print(std::ostream& os) const {
  os << "(>= " << expr1() << ' ' << expr2() << ")";
}


/* ====================================================================== */
/* GreaterThan */

/* Returns a greater-than comparison of the two expressions. */
const StateFormula& GreaterThan::make(const Expression& expr1,
                                      const Expression& expr2) {
  const Value* v1 = dynamic_cast<const Value*>(&expr1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&expr2);
    if (v2 != 0) {
      const StateFormula& value = (v1->value() > v2->value()) ? TRUE : FALSE;
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new GreaterThan(expr1, expr2);
}


/* Tests if this state formula holds in the given state. */
bool GreaterThan::holds(AtomSet const& atoms, ValueMap const& values) const {
  return expr1().value(values) > expr2().value(values);
}


/* Returns an instantiation of this state formula. */
const StateFormula& GreaterThan::instantiation(const SubstitutionMap& subst,
                                               const TermTable& terms,
                                               const AtomSet& atoms,
                                               const ValueMap& values,
                                               bool state) const {
  const Expression& inst_expr1 = expr1().instantiation(subst, values);
  const Expression& inst_expr2 = expr2().instantiation(subst, values);
  if (&inst_expr1 == &expr1() && &inst_expr2 == &expr2()) {
    return *this;
  } else {
    return make(inst_expr1, inst_expr2);
  }
}


/* Prints this object on the given stream. */
void GreaterThan::print(std::ostream& os) const {
  os << "(> " << expr1() << ' ' << expr2() << ")";
}


/* ====================================================================== */
/* Negation */

/* Returns the negation of the given state formula. */
const StateFormula& Negation::make(const StateFormula& formula) {
  if (formula.tautology()) {
    return FALSE;
  } else if (formula.contradiction()) {
    return TRUE;
  } else {
    return *new Negation(formula);
  }
}


/* Constructs a negated state formula. */
Negation::Negation(const StateFormula& negand)
  : negand_(&negand) {
  ref(negand_);
}


/* Deletes this negation. */
Negation::~Negation() {
  destructive_deref(negand_);
}


/* Returns an instantiation of this state formula. */
const StateFormula& Negation::instantiation(const SubstitutionMap& subst,
                                            const TermTable& terms,
                                            const AtomSet& atoms,
                                            const ValueMap& values,
                                            bool state) const {
  const StateFormula& inst_negand =
    negand().instantiation(subst, terms, atoms, values, state);
  if (&inst_negand == &negand()) {
    return *this;
  } else {
    return make(inst_negand);
  }
}


/* Prints this object on the given stream. */
void Negation::print(std::ostream& os) const {
  os << "(not " << negand() << ")";
}


/* ====================================================================== */
/* Conjunction. */

/* Deletes this conjunction. */
Conjunction::~Conjunction() {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    destructive_deref(*fi);
  }
}


/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula& conjunct) {
  conjuncts_.push_back(&conjunct);
  ref(&conjunct);
}


/* Tests if this state formula holds in the given state. */
bool Conjunction::holds(const TermTable& terms,
                        const AtomSet& atoms, const ValueMap& values) const {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    if (!(*fi)->holds(terms, atoms, values)) {
      return false;
    }
  }
  return true;
}


bool Conjunction::holds(AtomSet const& atoms, ValueMap const& values) const {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    if (!(*fi)->holds(atoms, values)) {
      return false;
    }
  }
  return true;
}


/* Returns an instantiation of this state formula. */
const StateFormula& Conjunction::instantiation(const SubstitutionMap& subst,
                                               const TermTable& terms,
                                               const AtomSet& atoms,
                                               const ValueMap& values,
                                               bool state) const {
  const StateFormula* inst_conj = &TRUE;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end() && !inst_conj->contradiction(); fi++) {
    inst_conj = &(*inst_conj
                  && (*fi)->instantiation(subst, terms, atoms, values, state));
  }
  return *inst_conj;
}


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os) const {
  os << "(and";
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    os << ' ' << **fi;
  }
  os << ")";
}


/* ====================================================================== */
/* Disjunction. */

/* Deletes this disjunction. */
Disjunction::~Disjunction() {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    destructive_deref(*fi);
  }
}


/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula& disjunct) {
  disjuncts_.push_back(&disjunct);
  ref(&disjunct);
}


/* Tests if this state formula holds in the given state. */
bool Disjunction::holds(const TermTable& terms,
                        const AtomSet& atoms, const ValueMap& values) const {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    if ((*fi)->holds(terms, atoms, values)) {
      return true;
    }
  }
  return false;
}


bool Disjunction::holds(AtomSet const& atoms, ValueMap const& values) const {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    if ((*fi)->holds(atoms, values)) {
      return true;
    }
  }
  return false;
}


/* Returns an instantiation of this formula. */
const StateFormula& Disjunction::instantiation(const SubstitutionMap& subst,
                                               const TermTable& terms,
                                               const AtomSet& atoms,
                                               const ValueMap& values,
                                               bool state) const {
  const StateFormula* inst_disj = &FALSE;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end() && !inst_disj->tautology(); fi++) {
    inst_disj = &(*inst_disj
                  || (*fi)->instantiation(subst, terms, atoms, values, state));
  }
  return *inst_disj;
}


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os) const {
  os << "(or";
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    os << ' ' << **fi;
  }
  os << ")";
}


/* ====================================================================== */
/* Quantification */

/* Constructs a quantification. */
Quantification::Quantification(const VariableList& parameters,
                               const StateFormula& body)
  : parameters_(parameters), body_(&body) {
  ref(body_);
}


/* Deletes this quantification. */
Quantification::~Quantification() {
  destructive_deref(body_);
}


/* ====================================================================== */
/* Exists */

/* Returns an existentially quantified formula. */
const StateFormula& Exists::make(const VariableList& parameters,
                                 const StateFormula& body) {
  if (parameters.empty() || body.tautology() || body.contradiction()) {
    return body;
  } else {
    return *new Exists(parameters, body);
  }
}


/* Tests if this state formula holds in the given state. */
bool Exists::holds(const TermTable& terms,
                   const AtomSet& atoms, const ValueMap& values) const {
  const StateFormula& inst_exists =
    instantiation(SubstitutionMap(), terms, atoms, values, true);
  return inst_exists.tautology();
}


/* Returns an instantiation of this formula. */
const StateFormula& Exists::instantiation(const SubstitutionMap& subst,
                                          const TermTable& terms,
                                          const AtomSet& atoms,
                                          const ValueMap& values,
                                          bool state) const {
  int n = parameters().size();
  std::vector<const ObjectList*> arguments(n);
  std::vector<ObjectList::const_iterator> next_arg;
  for (int i = 0; i < n; i++) {
    Type t = TermTable::type(parameters()[i]);
    arguments[i] = &terms.compatible_objects(t);
    if (arguments[i]->empty()) {
      return FALSE;
    }
    next_arg.push_back(arguments[i]->begin());
  }
  const StateFormula* inst_exists = &FALSE;
  std::stack<const StateFormula*> disjuncts;
  disjuncts.push(&body().instantiation(subst, terms, atoms, values, state));
  ref(disjuncts.top());
  for (int i = 0; i < n; ) {
    SubstitutionMap pargs;
    pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
    const StateFormula& disjunct =
      disjuncts.top()->instantiation(pargs, terms, atoms, values, state);
    disjuncts.push(&disjunct);
    if (i + 1 == n) {
      inst_exists = &(*inst_exists || disjunct);
      if (inst_exists->tautology()) {
        disjuncts.pop();
        while (!disjuncts.empty()) {
          destructive_deref(disjuncts.top());
          disjuncts.pop();
        }
        return *inst_exists;
      }
      for (int j = i; j >= 0; j--) {
        if (j < i) {
          destructive_deref(disjuncts.top());
        }
        disjuncts.pop();
        next_arg[j]++;
        if (next_arg[j] == arguments[j]->end()) {
          if (j == 0) {
            i = n;
            break;
          } else {
            next_arg[j] = arguments[j]->begin();
          }
        } else {
          i = j;
          break;
        }
      }
    } else {
      ref(disjuncts.top());
      i++;
    }
  }
  while (!disjuncts.empty()) {
    destructive_deref(disjuncts.top());
    disjuncts.pop();
  }
  return *inst_exists;
}


/* Prints this object on the given stream. */
void Exists::print(std::ostream& os) const {
  VariableList::const_iterator vi = parameters().begin();
  os << "(exists (" << *vi;
  for (vi++; vi != parameters().end(); vi++) {
    os << ' ' << *vi;
  }
  os << ") " << body() << ")";
}


/* ====================================================================== */
/* Forall */

/* Returns a universally quantified formula. */
const StateFormula& Forall::make(const VariableList& parameters,
                                 const StateFormula& body) {
  if (parameters.empty() || body.tautology() || body.contradiction()) {
    return body;
  } else {
    return *new Forall(parameters, body);
  }
}


/* Tests if this state formula holds in the given state. */
bool Forall::holds(const TermTable& terms,
                   const AtomSet& atoms, const ValueMap& values) const {
  const StateFormula& inst_forall =
    instantiation(SubstitutionMap(), terms, atoms, values, true);
  return inst_forall.tautology();
}


/* Returns an instantiation of this formula. */
const StateFormula& Forall::instantiation(const SubstitutionMap& subst,
                                          const TermTable& terms,
                                          const AtomSet& atoms,
                                          const ValueMap& values,
                                          bool state) const {
  int n = parameters().size();
  std::vector<const ObjectList*> arguments(n);
  std::vector<ObjectList::const_iterator> next_arg;
  for (int i = 0; i < n; i++) {
    Type t = TermTable::type(parameters()[i]);
    arguments[i] = &terms.compatible_objects(t);
    if (arguments[i]->empty()) {
      return TRUE;
    }
    next_arg.push_back(arguments[i]->begin());
  }
  const StateFormula* inst_forall = &TRUE;
  std::stack<const StateFormula*> conjuncts;
  conjuncts.push(&body().instantiation(subst, terms, atoms, values, state));
  ref(conjuncts.top());
  for (int i = 0; i < n; ) {
    SubstitutionMap pargs;
    pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
    const StateFormula& conjunct =
      conjuncts.top()->instantiation(pargs, terms, atoms, values, state);
    conjuncts.push(&conjunct);
    if (i + 1 == n) {
      inst_forall = &(*inst_forall && conjunct);
      if (inst_forall->contradiction()) {
        conjuncts.pop();
        while (!conjuncts.empty()) {
          destructive_deref(conjuncts.top());
          conjuncts.pop();
        }
        return *inst_forall;
      }
      for (int j = i; j >= 0; j--) {
        if (j < i) {
          destructive_deref(conjuncts.top());
        }
        conjuncts.pop();
        next_arg[j]++;
        if (next_arg[j] == arguments[j]->end()) {
          if (j == 0) {
            i = n;
            break;
          } else {
            next_arg[j] = arguments[j]->begin();
          }
        } else {
          i = j;
          break;
        }
      }
    } else {
      ref(conjuncts.top());
      i++;
    }
  }
  while (!conjuncts.empty()) {
    destructive_deref(conjuncts.top());
    conjuncts.pop();
  }
  return *inst_forall;
}


/* Prints this object on the given stream. */
void Forall::print(std::ostream& os) const {
  VariableList::const_iterator vi = parameters().begin();
  os << "(forall (" << *vi;
  for (vi++; vi != parameters().end(); vi++) {
    os << ' ' << *vi;
  }
  os << ") " << body() << ")";
}
}  // namespace PPDDL
