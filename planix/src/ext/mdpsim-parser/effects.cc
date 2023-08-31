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
#include "effects.h"
#include <cstdlib>
#include <stack>
#include <stdexcept>
#include <typeinfo>

namespace PPDDL {

/* ====================================================================== */
/* Update */

/* Constructs an update. */
Update::Update(const Fluent& fluent, const Expression& expr)
  : fluent_(&fluent), expr_(&expr) {
  RCObject::ref(fluent_);
  RCObject::ref(expr_);
}


/* Deletes this update. */
Update::~Update() {
  RCObject::destructive_deref(fluent_);
  RCObject::destructive_deref(expr_);
}


/* Output operator for updates. */
std::ostream& operator<<(std::ostream& os, const Update& u) {
  u.print(os);
  return os;
}


/* ====================================================================== */
/* Assign */

/* Changes the given state according to this update. */
void Assign::affect(ValueMap& values) const {
  values[&fluent()] = expression().value(values);
}


/* Returns an instantiaion of this update. */
const Update& Assign::instantiation(const SubstitutionMap& subst,
                                    const ValueMap& values) const {
  return *new Assign(fluent().substitution(subst),
                     expression().instantiation(subst, values));
}


/* Prints this object on the given stream. */
void Assign::print(std::ostream& os) const {
  os << "(assign " << fluent() << ' ' << expression() << ")";
}


/* ====================================================================== */
/* ScaleUp */

/* Changes the given state according to this update. */
void ScaleUp::affect(ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(&fluent());
  if (vi == values.end()) {
    throw std::logic_error("changing undefined value");
  } else {
    values[&fluent()] = (*vi).second * expression().value(values);
  }
}


/* Returns an instantiaion of this update. */
const Update& ScaleUp::instantiation(const SubstitutionMap& subst,
                                     const ValueMap& values) const {
  return *new ScaleUp(fluent().substitution(subst),
                      expression().instantiation(subst, values));
}

/* Prints this object on the given stream. */
void ScaleUp::print(std::ostream& os) const {
  os << "(scale-up " << fluent() << ' ' << expression() << ")";
}


/* ====================================================================== */
/* ScaleDown */

/* Changes the given state according to this update. */
void ScaleDown::affect(ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(&fluent());
  if (vi == values.end()) {
    throw std::logic_error("changing undefined value");
  } else {
    values[&fluent()] = (*vi).second / expression().value(values);
  }
}


/* Returns an instantiaion of this update. */
const Update& ScaleDown::instantiation(const SubstitutionMap& subst,
                                       const ValueMap& values) const {
  return *new ScaleDown(fluent().substitution(subst),
                        expression().instantiation(subst, values));
}


/* Prints this object on the given stream. */
void ScaleDown::print(std::ostream& os) const {
  os << "(scale-down " << fluent() << ' ' << expression() << ")";
}


/* ====================================================================== */
/* Increase */

/* Changes the given state according to this update. */
void Increase::affect(ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(&fluent());
  if (vi == values.end()) {
    throw std::logic_error("changing undefined value");
  } else {
    values[&fluent()] = (*vi).second + expression().value(values);
  }
}


/* Returns an instantiaion of this update. */
const Update& Increase::instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const {
  return *new Increase(fluent().substitution(subst),
                       expression().instantiation(subst, values));
}


/* Prints this object on the given stream. */
void Increase::print(std::ostream& os) const {
  os << "(increase " << fluent() << ' ' << expression() << ")";
}


CostMap Increase::cost() const {
  return {{fluent().name(), expression().value()}};
}


/* ====================================================================== */
/* Decrease */

/* Changes the given state according to this update. */
void Decrease::affect(ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(&fluent());
  if (vi == values.end()) {
    throw std::logic_error("changing undefined value");
  } else {
    values[&fluent()] = (*vi).second - expression().value(values);
  }
}


/* Returns an instantiaion of this update. */
const Update& Decrease::instantiation(const SubstitutionMap& subst,
                                      const ValueMap& values) const {
  return *new Decrease(fluent().substitution(subst),
                       expression().instantiation(subst, values));
}


/* Prints this object on the given stream. */
void Decrease::print(std::ostream& os) const {
  os << "(decrease " << fluent() << ' ' << expression() << ")";
}

CostMap Decrease::cost() const {
  return {{fluent().name(),
           -1.0 * static_cast<double>(expression().value())}};
}

/* ====================================================================== */
/* EmptyEffect */

/*
 * An empty effect.
 */
struct EmptyEffect : public Effect {
  /* Fills the provided lists with a sampled state change for this
     effect in the given state. */
  virtual void state_change(AtomList& adds, AtomList& deletes,
                            UpdateList& updates,
                            const AtomSet& atoms,
                            const ValueMap& values) const override {}

  /* Returns an instantiation of this effect. */
  virtual const Effect& instantiation(const SubstitutionMap& subst,
                                      const TermTable& terms,
                                      const AtomSet& atoms,
                                      const ValueMap& values) const override {
    return *this;
  }

  PrState<Rational> probTransitionTable(PrState<Rational> const& dist) const override {
    return dist;
  }
  PrState<double> probTransitionTable(PrState<double> const& dist) const override {
    return dist;
  }

  CostMap cost() const override {
    return {};
  }

 protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const override { os << "(and)"; }

 private:
  /* Constant representing the empty effect. */
  static const EmptyEffect EMPTY_;

  /* Constructs an empty effect. */
  EmptyEffect() { ref(this); }

  friend struct Effect;
};

/* Constant representing the empty effect. */
const EmptyEffect EmptyEffect::EMPTY_;


/* ====================================================================== */
/* Effect */

/* The empty effect. */
const Effect& Effect::EMPTY = EmptyEffect::EMPTY_;


/* Conjunction operator for effects. */
const Effect& operator&&(const Effect& e1, const Effect& e2) {
  if (e1.empty()) {
    return e2;
  } else if (e2.empty()) {
    return e1;
  } else {
    ConjunctiveEffect& conjunction = *new ConjunctiveEffect();
    const ConjunctiveEffect* c1 = dynamic_cast<const ConjunctiveEffect*>(&e1);
    if (c1 != 0) {
      for (EffectList::const_iterator ei = c1->conjuncts().begin();
           ei != c1->conjuncts().end(); ei++) {
        conjunction.add_conjunct(**ei);
      }
      RCObject::ref(c1);
      RCObject::destructive_deref(c1);
    } else {
      conjunction.add_conjunct(e1);
    }
    const ConjunctiveEffect* c2 = dynamic_cast<const ConjunctiveEffect*>(&e2);
    if (c2 != 0) {
      for (EffectList::const_iterator ei = c2->conjuncts().begin();
           ei != c2->conjuncts().end(); ei++) {
        conjunction.add_conjunct(**ei);
      }
      RCObject::ref(c2);
      RCObject::destructive_deref(c2);
    } else {
      conjunction.add_conjunct(e2);
    }
    return conjunction;
  }
}


/* Output operator for effects. */
std::ostream& operator<<(std::ostream& os, const Effect& e) {
  e.print(os);
  return os;
}


/* ====================================================================== */
/* SimpleEffect */

/* Constructs a simple effect. */
SimpleEffect::SimpleEffect(const Atom& atom)
  : atom_(&atom) {
  ref(atom_);
}


/* Deletes this simple effect. */
SimpleEffect::~SimpleEffect() {
  destructive_deref(atom_);
}


/* ====================================================================== */
/* AddEffect */

/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void AddEffect::state_change(AtomList& adds, AtomList& deletes,
                             UpdateList& updates,
                             const AtomSet& atoms,
                             const ValueMap& values) const {
  adds.push_back(&atom());
}


template<typename T>
PrState<T> AddEffect::probTransitionTable(PrState<T> const& dist) const {
  PrState<T> rv;
  for (auto const& it : dist) {
    State const& s = it.first;
    AtomSet sp{s.atoms};
    sp.insert(&atom());
    assert(it.second > 0 || it.second == 0);
    rv[{sp, s.values}] += it.second;
  }
  return rv;
}


/* Returns an instantiation of this effect. */
const Effect& AddEffect::instantiation(const SubstitutionMap& subst,
                                       const TermTable& terms,
                                       const AtomSet& atoms,
                                       const ValueMap& values) const {
  const Atom* inst_atom = &atom().substitution(subst);
  if (inst_atom == &atom()) {
    return *this;
  } else {
    return *new AddEffect(*inst_atom);
  }
}


/* Prints this object on the given stream. */
void AddEffect::print(std::ostream& os) const {
  os << atom();
}


/* ====================================================================== */
/* DeleteEffect */

/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void DeleteEffect::state_change(AtomList& adds, AtomList& deletes,
                                UpdateList& updates,
                                const AtomSet& atoms,
                                const ValueMap& values) const {
  deletes.push_back(&atom());
}

template<typename T>
PrState<T> DeleteEffect::probTransitionTable(PrState<T> const& dist) const {
  PrState<T> rv;
  for (auto const& it : dist) {
    State const& s = it.first;
    AtomSet sp{s.atoms};
    sp.erase(&atom());
    assert(it.second > 0 || it.second == 0);
    rv[{sp, s.values}] += it.second;
  }
  return rv;
}


/* Returns an instantiation of this effect. */
const Effect& DeleteEffect::instantiation(const SubstitutionMap& subst,
                                          const TermTable& terms,
                                          const AtomSet& atoms,
                                          const ValueMap& values) const {
  const Atom* inst_atom = &atom().substitution(subst);
  if (inst_atom == &atom()) {
    return *this;
  } else {
    return *new DeleteEffect(*inst_atom);
  }
}


/* Prints this object on the given stream. */
void DeleteEffect::print(std::ostream& os) const {
  os << "(not " << atom() << ")";
}


/* ====================================================================== */
/* UpdateEffect */

/* Returns an effect for the given update. */
const Effect& UpdateEffect::make(const Update& update) {
  if (typeid(update) == typeid(ScaleUp)
      || typeid(update) == typeid(ScaleDown)) {
    const Value* v = dynamic_cast<const Value*>(&update.expression());
    if (v != 0 && v->value() == 1) {
      return EMPTY;
    }
  } else if (typeid(update) == typeid(Increase)
             || typeid(update) == typeid(Decrease)) {
    const Value* v = dynamic_cast<const Value*>(&update.expression());
    if (v != 0 && v->value() == 0) {
      return EMPTY;
    }
  }
  return *new UpdateEffect(update);
}


/* Deletes this update effect. */
UpdateEffect::~UpdateEffect() {
  delete update_;
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void UpdateEffect::state_change(AtomList& adds, AtomList& deletes,
                                UpdateList& updates,
                                const AtomSet& atoms,
                                const ValueMap& values) const {
  updates.push_back(update_);
}


template<typename T>
PrState<T> UpdateEffect::probTransitionTable(PrState<T> const& dist) const {
  PrState<T> rv;
  for (auto const& it : dist) {
    State const& s = it.first;
    ValueMap new_values = s.values;
    update().affect(new_values);
    rv[{s.atoms, new_values}] += it.second;
  }
  return rv;
}


/* Returns an instantiation of this effect. */
const Effect& UpdateEffect::instantiation(const SubstitutionMap& subst,
                                          const TermTable& terms,
                                          const AtomSet& atoms,
                                          const ValueMap& values) const {
  return *new UpdateEffect(update().instantiation(subst, values));
}


/* Prints this object on the given stream. */
void UpdateEffect::print(std::ostream& os) const {
  os << update();
}


/* ====================================================================== */
/* ConjunctiveEffect */

/* Deletes this conjunctive effect. */
ConjunctiveEffect::~ConjunctiveEffect() {
  for (EffectList::const_iterator ei = conjuncts().begin();
       ei != conjuncts().end(); ei++) {
    destructive_deref(*ei);
  }
}


/* Adds a conjunct to this conjunctive effect. */
void ConjunctiveEffect::add_conjunct(const Effect& conjunct) {
  const ConjunctiveEffect* conj_effect =
    dynamic_cast<const ConjunctiveEffect*>(&conjunct);
  if (conj_effect != 0) {
    for (EffectList::const_iterator ei = conj_effect->conjuncts().begin();
         ei != conj_effect->conjuncts().end(); ei++) {
      conjuncts_.push_back(*ei);
      ref(*ei);
    }
    ref(&conjunct);
    destructive_deref(&conjunct);
  } else {
    conjuncts_.push_back(&conjunct);
    ref(&conjunct);
  }
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void ConjunctiveEffect::state_change(AtomList& adds, AtomList& deletes,
                                     UpdateList& updates,
                                     const AtomSet& atoms,
                                     const ValueMap& values) const {
  for (EffectList::const_iterator ei = conjuncts().begin();
       ei != conjuncts().end(); ei++) {
    (*ei)->state_change(adds, deletes, updates, atoms, values);
  }
}


template<typename T>
PrState<T> ConjunctiveEffect::probTransitionTable(PrState<T> const& dist) const {
  PrState<T> rv{dist};
  for (Effect const* eff : conjuncts()) {
    rv = eff->probTransitionTable(rv);
  }
  return rv;
}


/* Returns an instantiation of this effect. */
const Effect& ConjunctiveEffect::instantiation(const SubstitutionMap& subst,
                                               const TermTable& terms,
                                               const AtomSet& atoms,
                                               const ValueMap& values) const {
  ConjunctiveEffect& inst_effect = *new ConjunctiveEffect();
  for (EffectList::const_iterator ei = conjuncts().begin();
       ei != conjuncts().end(); ei++) {
    inst_effect.add_conjunct((*ei)->instantiation(subst, terms,
                                                  atoms, values));
  }
  return inst_effect;
}


/* Prints this object on the given stream. */
void ConjunctiveEffect::print(std::ostream& os) const {
  os << "(and";
  for (EffectList::const_iterator ei = conjuncts().begin();
       ei != conjuncts().end(); ei++) {
    os << ' ' << **ei;
  }
  os << ")";
}


/* ====================================================================== */
/* ConditionalEffect */

/* Returns a conditional effect. */
const Effect& ConditionalEffect::make(const StateFormula& condition,
                                      const Effect& effect) {
  if (condition.tautology()) {
    return effect;
  } else if (condition.contradiction() || effect.empty()) {
    return EMPTY;
  } else {
    return *new ConditionalEffect(condition, effect);
  }
}


/* Constructs a conditional effect. */
ConditionalEffect::ConditionalEffect(const StateFormula& condition,
                                     const Effect& effect)
  : condition_(&condition), effect_(&effect) {
  ref(condition_);
  ref(effect_);
}


/* Deletes this conditional effect. */
ConditionalEffect::~ConditionalEffect() {
  destructive_deref(condition_);
  destructive_deref(effect_);
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void ConditionalEffect::state_change(AtomList& adds, AtomList& deletes,
                                     UpdateList& updates,
                                     const AtomSet& atoms,
                                     const ValueMap& values) const {
  if (condition().holds(atoms, values)) {
    /* Effect condition holds. */
    effect().state_change(adds, deletes, updates, atoms, values);
  }
}


template<typename T>
PrState<T> ConditionalEffect::probTransitionTable(PrState<T> const& dist) const {
  PrState<T> rv;
  PrState<T> subset_holding_cond;

  for (auto const& it : dist) {
    State const& s = it.first;
    if (condition().holds(s.atoms, s.values)) {
      subset_holding_cond.insert(it);
    }
    else {
      rv.insert(it);
    }
  }
  // Applying the effect on the states in which the condition holds
  if (subset_holding_cond.size() > 0) {
    for (auto const& it : effect().probTransitionTable(subset_holding_cond)) {
      assert(it.second > 0 || it.second == 0);
      rv[it.first] += it.second;
    }
  }
  return rv;
}


/* Returns an instantiation of this effect. */
const Effect& ConditionalEffect::instantiation(const SubstitutionMap& subst,
                                               const TermTable& terms,
                                               const AtomSet& atoms,
                                               const ValueMap& values) const
{
  StateFormula const& inst_cond = condition().instantiation(subst, terms, atoms, values, false);
  Effect const& inst_eff = effect().instantiation(subst, terms, atoms, values);

  // FWT: Apparently the make functions were design to be used with the parser
  // thus the memory management works there but not for this case. Therefore, we
  // have to make sure that we delete what is created here but not used.
  Effect const& rv_eff = make(inst_cond, inst_eff);

  if (&rv_eff == &EMPTY) {
    // FWT: inst_eff was created here and was not used. inst_eff might be used
    // somewhere else (see IPPC'08 a-schedule-problem55.pddl where
    //
    // assert(refCount(inst_eff) == 0);
    // 
    // fails) and it is not clear when and why this happens therefore we can
    // only delete if there is no one has a reference to it.
    deleteIfZeroCount(inst_eff);
  }

  return rv_eff;
}


/* Prints this object on the given stream. */
void ConditionalEffect::print(std::ostream& os) const {
  os << "(when " << condition() << ' ' << effect() << ")";
}


/* ====================================================================== */
/* ProbabilisticEffect */

void ProbabilisticEffect::normalizePrDist() {
  Rational pr_no_op(1);
  for (size_t i = 0; i < size(); ++i) {
    // Not using probability(i) because it is guarded with a call to hasNormalizedPrDist
    pr_no_op -= prob_[i];
  }
  assert(pr_no_op >= -1e-6);
  if (pr_no_op >= 1e-6) {
    add_outcome(pr_no_op, EMPTY);
  }
}


bool ProbabilisticEffect::hasNormalizedPrDist() const {
  Rational sum(0);
  for (size_t i = 0; i < size(); ++i) {
    // Not using probability(i) because it is guarded with a call to hasNormalizedPrDist
    sum += prob_[i];
  }
  return std::abs(1 - double(sum)) <= 1e-6;
}


/* Returns a probabilistic effect. */
const Effect&
ProbabilisticEffect::make(std::vector<std::pair<Rational, Effect const*>> const& os)
{
  if (os.size() == 0)
    return EMPTY;

  ProbabilisticEffect& peff = *new ProbabilisticEffect();
  size_t n_zero_prob_effs = 0;
  for (size_t i = 0; i < os.size(); i++) {
    assert(os[i].first >= 0);
    if (os[i].first == 0) {
      n_zero_prob_effs++;
      continue;
    }
    peff.add_outcome(os[i].first, *os[i].second);
  }
  assert(peff.size() + n_zero_prob_effs == os.size());
  peff.normalizePrDist();
  return peff;
}


/* Deletes this probabilistic effect. */
ProbabilisticEffect::~ProbabilisticEffect() {
  for (EffectList::const_iterator ei = effects_.begin();
       ei != effects_.end(); ei++) {
    destructive_deref(*ei);
  }
}


/* Adds an outcome to this probabilistic effect. */
void ProbabilisticEffect::add_outcome(Rational const& p, Effect const& effect) {
  if (p == 0)
    return;
  assert(p > 0);

  auto prob_effect = dynamic_cast<ProbabilisticEffect const*>(&effect);
  if (prob_effect != nullptr) {
    for (size_t i = 0; i < prob_effect->size(); ++i) {
      add_outcome(p*prob_effect->probability(i), prob_effect->effect(i));
    }
    ref(&effect);
    destructive_deref(&effect);
  }
  else {
    prob_.push_back(p);
    effects_.push_back(&effect);
    ref(&effect);
  }
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void ProbabilisticEffect::state_change(AtomList& adds, AtomList& deletes,
                                       UpdateList& updates,
                                       const AtomSet& atoms,
                                       const ValueMap& values) const {
  assert(hasNormalizedPrDist());
  assert(size() > 0);
  double w = rand()/(RAND_MAX + 1.0);
  assert(w < 1);
  size_t i = 0;
  for (i = 0; i < size(); ++i) {
    w -= prob_[i];
    if (w < 0) {
      effect(i).state_change(adds, deletes, updates, atoms, values);
      return;
    }
  }
  effect(size()-1).state_change(adds, deletes, updates, atoms, values);
}


template<typename T>
PrState<T> ProbabilisticEffect::probTransitionTable(PrState<T> const& dist) const {
  PrState<T> rv;
  assert(hasNormalizedPrDist());
  for (size_t i = 0; i < size(); ++i) {
    for (auto const& it : effect(i).probTransitionTable(dist)) {
      T joint_pr = probability(i) * it.second;
      assert(joint_pr >= 0);
      // joint_pr can be zero due to underflow. The if avoids adding states
      // to rv that have zero probability, keeping the resulting probability
      // distributions clean.
      if (joint_pr > 0)
        rv[it.first] += joint_pr;
    }
  }
  return rv;
}


/* Returns an instantiation of this effect. */
const Effect&
ProbabilisticEffect::instantiation(const SubstitutionMap& subst,
                                   const TermTable& terms,
                                   const AtomSet& atoms,
                                   const ValueMap& values) const {
  assert(hasNormalizedPrDist());
  ProbabilisticEffect& inst_effect = *new ProbabilisticEffect();
  for (size_t i = 0; i < size(); i++) {
    inst_effect.add_outcome(probability(i),
                            effect(i).instantiation(subst, terms,
                                                    atoms, values));
  }
  assert(inst_effect.size() >= size());
  assert(inst_effect.hasNormalizedPrDist());
  return inst_effect;
}


/* Prints this object on the given stream. */
void ProbabilisticEffect::print(std::ostream& os) const {
  switch (size()) {
    case 0: os << "(and)"; break;
    case 1: os << effect(0); break;
    default:
      os << "(probabilistic";
      for (size_t i = 0; i < size(); i++) {
        os << ' ' << probability(i) << ' ' << effect(i);
      }
      os << ")";
  }
}


/* ====================================================================== */
/* QuantifiedEffect */

/* Returns a universally quantified effect. */
const Effect& QuantifiedEffect::make(const VariableList& parameters,
                                     const Effect& effect) {
  if (parameters.empty() || effect.empty()) {
    return effect;
  } else {
    return *new QuantifiedEffect(parameters, effect);
  }
}


/* Constructs a universally quantified effect. */
QuantifiedEffect::QuantifiedEffect(const VariableList& parameters,
                                   const Effect& effect)
  : parameters_(parameters), effect_(&effect) {
  ref(effect_);
}


/* Deletes this universally quantifed effect. */
QuantifiedEffect::~QuantifiedEffect() {
  destructive_deref(effect_);
}


/* Returns an instantiation of this effect. */
const Effect& QuantifiedEffect::instantiation(const SubstitutionMap& subst,
                                              const TermTable& terms,
                                              const AtomSet& atoms,
                                              const ValueMap& values) const {
  int n = parameters().size();
  if (n == 0) {
    return effect().instantiation(subst, terms, atoms, values);
  } else {
    SubstitutionMap args(subst);
    std::vector<const ObjectList*> arguments(n);
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      Type t = TermTable::type(parameters()[i]);
      arguments[i] = &terms.compatible_objects(t);
      if (arguments[i]->empty()) {
        return EMPTY;
      }
      next_arg.push_back(arguments[i]->begin());
    }
    const Effect* conj = &EMPTY;
    std::stack<const Effect*> conjuncts;
    conjuncts.push(&effect().instantiation(args, terms, atoms, values));
    ref(conjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Effect& conjunct =
        conjuncts.top()->instantiation(pargs, terms, atoms, values);
      conjuncts.push(&conjunct);
      if (i + 1 == n) {
        conj = &(*conj && conjunct);
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
    return *conj;
  }
}


/* Prints this object on the given stream. */
void QuantifiedEffect::print(std::ostream& os) const {
  if (parameters().empty()) {
    os << effect();
  } else {
    VariableList::const_iterator vi = parameters().begin();
    os << "(forall (" << *vi;
    for (vi++; vi != parameters().end(); vi++) {
      os << ' ' << *vi;
    }
    os << ") " << effect() << ")";
  }
}
}  // namespace PPDDL
