/* -*-C++-*- */
/*
 * Effects.
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
#ifndef EFFECTS_H
#define EFFECTS_H

#include <iostream>
#include <utility>
#include <vector>
#include <map>

#include "formulas.h"
#include "expressions.h"
#include "refcount.h"
#include "terms.h"
#include "rational.h"
#include "states.h"


namespace PPDDL {


using CostMap = std::map<std::string, double>;

/* ====================================================================== */
/* Update */

/*
 * An update.
 */
struct Update {
  /* Deletes this update. */
  virtual ~Update();

  /* Returns the fluent of this update. */
  const Fluent& fluent() const { return *fluent_; }

  /* Returns the expression of this update. */
  const Expression& expression() const { return *expr_; }

  /* Changes the given state according to this update. */
  virtual void affect(ValueMap& values) const = 0;

  /* Returns an instantiaion of this update. */
  virtual const Update& instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const = 0;

  virtual CostMap cost() const = 0;

 protected:
  /* Constructs an update. */
  Update(const Fluent& fluent, const Expression& expr);

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

 private:
  /* Fluent affected by this update. */
  const Fluent* fluent_;
  /* Expression. */
  const Expression* expr_;

  friend std::ostream& operator<<(std::ostream& os, const Update& u);
};

/* Output operator for updates. */
std::ostream& operator<<(std::ostream& os, const Update& u);


/* ====================================================================== */
/* UpdateList */

/*
 * List of updates.
 */
using UpdateList = std::vector<const Update*>;


/* ====================================================================== */
/* Assign */

/*
 * An assign update.
 */
struct Assign : public Update {
  /* Constructs an assign update. */
  Assign(const Fluent& fluent, const Expression& expr)
    : Update(fluent, expr) {}

  /* Changes the given state according to this update. */
  virtual void affect(ValueMap& values) const;

  /* Returns an instantiaion of this update. */
  virtual const Update& instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const;

  CostMap cost() const override {
    throw std::logic_error("Called cost() for Assign");
    return {};
  }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* ScaleUp */

/*
 * An scale-up update.
 */
struct ScaleUp : public Update {
  /* Constructs an scale-up update. */
  ScaleUp(const Fluent& fluent, const Expression& expr)
    : Update(fluent, expr) {}

  /* Changes the given state according to this update. */
  virtual void affect(ValueMap& values) const;

  /* Returns an instantiaion of this update. */
  virtual const Update& instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const;

  CostMap cost() const override {
    throw std::logic_error("Called cost() for ScaleUp");
    return {};
  }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* ScaleDown */

/*
 * An scale-down update.
 */
struct ScaleDown : public Update {
  /* Constructs an scale-down update. */
  ScaleDown(const Fluent& fluent, const Expression& expr)
    : Update(fluent, expr) {}

  /* Changes the given state according to this update. */
  virtual void affect(ValueMap& values) const;

  /* Returns an instantiaion of this update. */
  virtual const Update& instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const;

  CostMap cost() const override {
    throw std::logic_error("Called cost() for ScaleDown");
    return {};
  }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Increase */

/*
 * An increase update.
 */
struct Increase : public Update {
  /* Constructs an increase update. */
  Increase(const Fluent& fluent, const Expression& expr)
    : Update(fluent, expr) {}

  /* Changes the given state according to this update. */
  virtual void affect(ValueMap& values) const;

  /* Returns an instantiaion of this update. */
  virtual const Update& instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const;

  CostMap cost() const override;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Decrease */

/*
 * An decrease update.
 */
struct Decrease : public Update {
  /* Constructs an decrease update. */
  Decrease(const Fluent& fluent, const Expression& expr)
    : Update(fluent, expr) {}

  /* Changes the given state according to this update. */
  virtual void affect(ValueMap& values) const;

  /* Returns an instantiaion of this update. */
  virtual const Update& instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const;

  CostMap cost() const override;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Effect */

/*
 * An effect.
 */
struct Effect : public RCObject {
  /* The empty effect. */
  static const Effect& EMPTY;

  /* Tests if this is the empty effect. */
  bool empty() const { return this == &EMPTY; }

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const = 0;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const = 0;

  virtual PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const = 0;
  virtual PrState<double> probTransitionTable(PrState<double> const& dist) const = 0;

  virtual CostMap cost() const = 0;

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const Effect& e);
};

/* Conjunction operator for effects. */
const Effect& operator&&(const Effect& e1, const Effect& e2);

/* Output operator for effects. */
std::ostream& operator<<(std::ostream& os, const Effect& e);


/* ====================================================================== */
/* EffectList */

/*
 * List of effects.
 */
using EffectList = std::vector<const Effect*>;


/* ====================================================================== */
/* SimpleEffect */

/*
 * A simple effect.
 */
struct SimpleEffect : public Effect {
  /* Deletes this simple effect. */
  virtual ~SimpleEffect();

  /* Returns the atom associated with this simple effect. */
  const Atom& atom() const { return *atom_; }

 protected:
  /* Constructs a simple effect. */
  explicit SimpleEffect(const Atom& atom);

 private:
  /* Atom added by this effect. */
  const Atom* atom_;
};


/* ====================================================================== */
/* AddEffect */

/*
 * An add effect.
 */
struct AddEffect : public SimpleEffect {
  /* Constructs an add effect. */
  explicit AddEffect(const Atom& atom) : SimpleEffect(atom) {}

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return probTransitionTable<>(dist);
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return probTransitionTable<>(dist);
  }

  CostMap cost() const override {
    return {};
  }

 protected:
  template<typename T>
  PrState<T> probTransitionTable(PrState<T> const& dist) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;
};


/* ====================================================================== */
/* DeleteEffect */

/*
 * A delete effect.
 */
struct DeleteEffect : public SimpleEffect {
  /* Constructs a delete effect. */
  explicit DeleteEffect(const Atom& atom) : SimpleEffect(atom) {}

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return probTransitionTable<>(dist);
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return probTransitionTable<>(dist);
  }

  CostMap cost() const override {
    return {};
  }

 protected:
  template<typename T>
  PrState<T> probTransitionTable(PrState<T> const& dist) const;
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;
};


/* ====================================================================== */
/* UpdateEffect */

/*
 * An update effect.
 */
struct UpdateEffect : public Effect {
  /* Returns an effect for the given update. */
  static const Effect& make(const Update& update);

  /* Deletes this update effect. */
  virtual ~UpdateEffect();

  /* Returns the update performed by this effect. */
  const Update& update() const { return *update_; }

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;


  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return probTransitionTable<>(dist);
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return probTransitionTable<>(dist);
  }

  CostMap cost() const override {
    return update().cost();
  }


 protected:
  template<typename T>
  PrState<T> probTransitionTable(PrState<T> const& dist) const;
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;

 private:
  /* Update performed by this effect. */
  const Update* update_;

  /* Constructs an update effect. */
  explicit UpdateEffect(const Update& update) : update_(&update) {}
};


/* ====================================================================== */
/* ConjunctiveEffect */

/*
 * A conjunctive effect.
 */
struct ConjunctiveEffect : public Effect {
  /* Deletes this conjunctive effect. */
  virtual ~ConjunctiveEffect();

  /* Returns the conjuncts of this conjunctive effect. */
  const EffectList& conjuncts() const { return conjuncts_; }

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return probTransitionTable<>(dist);
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return probTransitionTable<>(dist);
  }

  CostMap cost() const override {
    CostMap c;
    for (auto const& eff : conjuncts()) {
      for (auto const& fname_val : eff->cost()) {
        c[fname_val.first] += fname_val.second;
      }
    }
    return c;
  }

 protected:
  template<typename T>
  PrState<T> probTransitionTable(PrState<T> const& dist) const;
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;

 private:
  /* The conjuncts. */
  EffectList conjuncts_;

  /* Constructs a conjunctive effect. */
  ConjunctiveEffect() {}

  /* Adds a conjunct to this conjunctive effect. */
  void add_conjunct(const Effect& conjunct);

  friend const Effect& operator&&(const Effect& e1, const Effect& e2);
};


/* ====================================================================== */
/* ConditionalEffect */

/*
 * A conditional effect.
 */
struct ConditionalEffect : public Effect {
  /* Returns a conditional effect. */
  static const Effect& make(const StateFormula& condition,
                            const Effect& effect);

  /* Deletes this conditional effect. */
  virtual ~ConditionalEffect();

  /* Returns the condition of this effect. */
  const StateFormula& condition() const { return *condition_; }

  /* Returns the conditional effect of this effect. */
  const Effect& effect() const { return *effect_; }

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return probTransitionTable<>(dist);
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return probTransitionTable<>(dist);
  }

  CostMap cost() const override {
    throw std::logic_error("Called cost() for conditional effect");
    return {};
  }

 protected:
  template<typename T>
  PrState<T> probTransitionTable(PrState<T> const& dist) const;
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;

 private:
  /* Effect condition. */
  const StateFormula* condition_;
  /* Effect. */
  const Effect* effect_;

  /* Constructs a conditional effect. */
  ConditionalEffect(const StateFormula& condition, const Effect& effect);
};


/* ====================================================================== */
/* ProbabilisticEffect */

/*
 * A probabilistic effect.
 */
struct ProbabilisticEffect : public Effect {
  /* Returns a probabilistic effect. */
  static const Effect&
  make(const std::vector<std::pair<Rational, const Effect*> >& os);

  /* Deletes this probabilistic effect. */
  virtual ~ProbabilisticEffect();

  /* Returns the number of outcomes of this probabilistic effect. */
  size_t size() const { return prob_.size(); }

  /* Returns the ith outcome's probability. */
  Rational probability(size_t i) const {
    assert(hasNormalizedPrDist());
    return prob_[i];
  }

  /* Returns the ith outcome's effect. */
  const Effect& effect(size_t i) const { return *effects_[i]; }

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override;

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return probTransitionTable<>(dist);
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return probTransitionTable<>(dist);
  }

  CostMap cost() const override {
    CostMap c;
    for (size_t i = 0; i < size(); ++i) {
      for (auto const& fname_val : effect(i).cost()) {
        c[fname_val.first] += probability(i) * fname_val.second;
      }
    }
    return c;
  }

 protected:
  template<typename T>
  PrState<T> probTransitionTable(PrState<T> const& dist) const;
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;

 private:
  std::vector<Rational> prob_;
  /* Outcome effects. */
  EffectList effects_;

  ProbabilisticEffect() { }

  /* Adds an outcome to this probabilistic effect. */
  void add_outcome(const Rational& p, const Effect& effect);

  // Adds an empty effect (NO-OP) with probability 1 - sum_i(prob(i)). This is
  // ignored if the NO-OP probability is too small since it is likely an artefact
  // of the numerical instability.
  void normalizePrDist();
  // Returns true if the sum of probabilistic effects is within tolerances
  bool hasNormalizedPrDist() const;
};


/* ====================================================================== */
/* QuantifiedEffect */

/*
 * A universally quantified effect.
 */
struct QuantifiedEffect : public Effect {
  /* Returns a universally quantified effect. */
  static const Effect& make(const VariableList& parameters,
                            const Effect& body);

  /* Deletes this universally quantifed effect. */
  virtual ~QuantifiedEffect();

  /* Returns the parameters of this universally quantified effect. */
  const VariableList& parameters() const { return parameters_; }

  /* Returns the quantified effect. */
  const Effect& effect() const { return *effect_; }

  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override
  {
    // LEGACY(FWT): This should never happen when the problem is instantiated
    // since all QuantifiedEffect are compiled into conjunctions or disjunctions
    throw std::logic_error("Quantified::state_change not implemented");
  }

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override;

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    throw std::logic_error("QuantifiedEffect not expected in grounded problem");
    return dist;
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    throw std::logic_error("QuantifiedEffect not expected in grounded problem");
    return dist;
  }

  CostMap cost() const override {
    throw std::logic_error("Called cost() for QuantifiedEffect");
    return {};
  }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override;

 private:
  /* Quantified variables. */
  VariableList parameters_;
  /* The quantified effect. */
  const Effect* effect_;

  /* Constructs a universally quantified effect. */
  explicit QuantifiedEffect(const VariableList& parameters,
                            const Effect& effect);
};


}  // namespace PPDDL
#endif /* EFFECTS_H */
