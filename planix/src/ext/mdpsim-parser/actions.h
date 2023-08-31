/* -*-C++-*- */
/*
 * Actions.
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
#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <cmath>

#include "states.h"
#include "effects.h"
#include "formulas.h"
#include "expressions.h"
#include "terms.h"



namespace PPDDL {

/* ====================================================================== */
/* ActionSchema */

struct Action;

using ActionSet = std::set<const Action*>;
using ActionList = std::vector<const Action*>;
using VecActionPtr = std::vector<const Action*>;

/*
 * Action schema.
 */
struct ActionSchema {
  /* Constructs an action schema with the given name. */
  explicit ActionSchema(const std::string& name);

  /* Deletes this action schema. */
  ~ActionSchema();

  /* Adds a parameter to this action schema. */
  void add_parameter(const Variable& parameter) {
    parameters_.push_back(parameter);
  }

  /* Sets the precondition of this action schema. */
  void set_precondition(const StateFormula& precondition);

  /* Sets the effect of this action schema. */
  void set_effect(const Effect& effect);

  /* Returns the name of this action schema. */
  const std::string& name() const { return name_; }

  /* Returns the parameters of this action schema. */
  const VariableList& parameters() const { return parameters_; }

  /* Returns the preconditions of this action schema. */
  const StateFormula& precondition() const { return *precondition_; }

  /* Returns the effect of this action schema. */
  const Effect& effect() const { return *effect_; }

  /* Fills the provided list with instantiations of this action
     schema. */
  void instantiations(VecActionPtr& actions, const TermTable& terms,
                      const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this action schema. */
  const Action& instantiation(const SubstitutionMap& subst,
                              const TermTable& terms,
                              const AtomSet& atoms,
                              const ValueMap& values,
                              const StateFormula& precond) const;

 private:
  /* Action name. */
  std::string name_;
  /* Action parameters. */
  VariableList parameters_;
  /* Action precondition. */
  const StateFormula* precondition_;
  /* Action effect. */
  const Effect* effect_;
};

/* Output operator for action schemas. */
std::ostream& operator<<(std::ostream& os, const ActionSchema& a);


/* ====================================================================== */
/* ActionSchemaMap */

/*
 * Table of action schemas.
 */
using ActionSchemaMap = std::map<std::string, const ActionSchema*>;


/* ====================================================================== */
/* Action */

/*
 * A fully instantiated action.
 */
struct Action {
  /* Constructs an action with the given name. */
  explicit Action(const std::string& name);

  /* Deletes this action. */
  ~Action();

  /* Adds an argument to this action. */
  void add_argument(const Object& argument) { arguments_.push_back(argument); }

  /* Sets the precondition of this action. */
  void set_precondition(const StateFormula& precondition);

  /* Sets the effect of this action. */
  void set_effect(const Effect& effect);

  /* Returns the name of this action. */
  const std::string& name() const { return name_; }

  /* Returns the arguments of this action. */
  const ObjectList& arguments() const { return arguments_; }

  /* Returns the precondition of this action. */
  const StateFormula& precondition() const { return *precondition_; }

  /* Returns the effect of this action. */
  const Effect& effect() const { return *effect_; }

  /* Tests if this action is enabled in the given state. */
  bool enabled(State const& s) const { return enabled(s.atoms, s.values); }

  /* Returns the result of applying this action on state s. If a is a probabilistic action, one of
   * the results will be sampled. Use probTransitionTable(s) to get the probability distribution of
   * the possible resulting states
   */
  State apply(State const& s) const;

  // Returns P(.|s,a), that is, the probability distribution over states
  // representing the outcome of applying a in state s. This distribution is
  // computed on-the-fly, so it is not suitable for algorithms that query the
  // same P(.|s,a) several times (e.g., any algorithm based on Bellman updates).
  // Such algorithms such use a cached version of this method to save time.
  // The main user of this method are algorithms such as i-dual that, for any
  // pair (s,a), it queries P(.|s,a) only once for (s,a).
  //
  // ATTENTION(FWT): ValueMap& values ON PURPOSE because it keeps track of
  // functions that are used by preconditions and effects and I do not plan to
  // support them.
  template<typename T>
  PrState<T> probTransitionTable(State const& s) const {
    PrState<T> rv = effect().probTransitionTable(PrState<T>{{s, 1}});
#if not defined NDEBUG
    double sum = 0;
    for (auto const& it : rv) {
      assert(it.second > 0);
      sum += it.second;
    }
    assert(std::abs(sum - 1) <= 1e-3);
#endif
    return rv;
  }

  CostMap cost() const {
    return effect().cost();
  }


 private:
  /* Tests if this action is enabled in the given state. */
  bool enabled(AtomSet const& atoms, ValueMap const& values) const;

  /* Changes the given state according to the effects of this action. */
  void affect(State& s) const;

 private:
  /* Action name. */
  std::string name_;
  /* Action arguments. */
  ObjectList arguments_;
  /* Action precondition. */
  const StateFormula* precondition_;
  /* Action effect. */
  const Effect* effect_;
};

/* Output operator for actions. */
std::ostream& operator<<(std::ostream& os, const Action& a);

}  // namespace PPDDL


/*
 * Less than function object for action pointers.
 */
namespace std {
template<>
struct less<const PPDDL::Action*>
  : public std::binary_function<const PPDDL::Action*, const PPDDL::Action*, bool> {
  /* Comparison function operator. */
  bool operator()(const PPDDL::Action* a1, const PPDDL::Action* a2) const {
    if (a1->name() < a2->name()) {
      return true;
    } else if (a2->name() < a1->name()) {
      return false;
    } else {
      return a1->arguments() < a2->arguments();
    }
  }
};
}


#endif /* ACTIONS_H */
