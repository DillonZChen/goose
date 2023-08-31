/* -*-C++-*- */
/*
 * Problem descriptions.
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
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <iostream>
#include <map>
#include <string>

#include "states.h"

#include "domains.h"
#include "actions.h"
#include "effects.h"
#include "formulas.h"
#include "expressions.h"
#include "terms.h"
#include "types.h"


namespace PPDDL {

/* ====================================================================== */
/* Problem */

/*
 * Problem definition.
 */
struct Problem {
  /* Table of problem definitions. */
  using ProblemMap = std::map<std::string, const Problem*>;

  /* Returns a const_iterator pointing to the first problem. */
  static ProblemMap::const_iterator begin();

  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMap::const_iterator end();

  /* Returns a pointer to the problem with the given name, or 0 if it
     is undefined. */
  static const Problem* find(const std::string& name);

  /* Removes all defined problems. */
  static void clear();

  /* Constructs a problem. */
  Problem(const std::string& name, const Domain& domain);

  /* Deletes a problem. */
  ~Problem();

  /* Returns the name of this problem. */
  const std::string& name() const { return name_; }

  /* Returns the domain of this problem. */
  const Domain& domain() const { return *domain_; }

  /* Returns the term table of this problem. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this problem. */
  const TermTable& terms() const { return terms_; }

  /* Adds an atomic state formula to the initial conditions of this
     problem. */
  void add_init_atom(const Atom& atom);

  /* Adds a fluent value to the initial conditions of this problem. */
  void add_init_value(const Fluent& fluent, const Rational& value);

  /* Adds an initial effect for this problem. */
  void add_init_effect(const Effect& effect);

  /* Sets the goal for this problem. */
  void set_goal(const StateFormula& goal);

  /* Sets the goal reward for this problem. */
  void set_goal_reward(const Update& goal_reward);

  /* Sets the metric to maximize for this problem. */
  void set_metrics(VecExpression& metrics, bool minimize);

  /* Grounds/Instantiates this problem. */
  void ground();

  State initialState() const;

  bool isGoal(State const& s) const { return goal().holds(s.atoms, s.values); }

  /* Returns the goal of this problem. */
  const StateFormula& goal() const { return *goal_; }

  /* Returns a pointer to the goal reward for this problem, or 0 if no
     explicit reward is associated with goal states. */
  const Update* goal_reward() const { return goal_reward_; }

  /* Returns the metric to maximize for this problem. */
  Expression const& metric() const {
    Expression const* m = *(metrics().begin());
    if (metrics().size() > 1) {
      throw std::logic_error("Metric called for a multi-objective problem.");
    }
    return *m;
  }

  VecExpression const& metrics() const { return metrics_; }

  /* Tests if the metric is constant. */
  bool constant_metric() const;

  /* Returns a list of instantiated actions. */
  VecActionPtr const& actions() const { return actions_; }

  /* Fills the given vector with actions enabled in the given state. */
  void enabledActions(ActionList& actions, State const& s) const;

  bool isApplicable(Action const& action, State const& state) {
    return action.enabled(state);
  }

 private:
  friend std::ostream& operator<<(std::ostream& os, const Problem& p);

  // For a public interface to the initial conditions, use initialState()
  /* Returns the initial atoms of this problem. */
  AtomSet const& init_atoms() const { return init_atoms_; }

  /* Returns the initial values of this problem. */
  ValueMap const& init_values() const { return init_values_; }

  /* Returns the initial effects of this problem. */
  EffectList const& init_effects() const { return init_effects_; }


 private:
  /* Table of defined problems. */
  static ProblemMap problems;

  /* Name of problem. */
  std::string name_;
  /* Problem domain. */
  const Domain* domain_;
  /* Problem terms. */
  TermTable terms_;
  /* Initial atoms. */
  AtomSet init_atoms_;
  /* Initial fluent values. */
  ValueMap init_values_;
  /* Initial effects. */
  EffectList init_effects_;
  /* Goal; FALSE if not a goal-directed planning problem. */
  const StateFormula* goal_;
  /* Goal reward expression. */
  const Update* goal_reward_;
  /* FWT: Multi-Objectic metrics to be MINIMIZED */
  VecExpression metrics_;
  /* Instantiated actions. */
  VecActionPtr actions_;
};


// Parses and return a problem
Problem const* parsePPDDL(std::string const& domain_problem_fname);
Problem const* parsePPDDL(std::string const& domain_fname, std::string const& problem_fname);


/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p);

/* Constructs an atom from predicate and term names (predicate and terms must all exist). */
const Atom* getAtom(const Problem &problem, const std::string &pred_name,
                    const std::vector<std::string> &term_names);

}  // namespace PPDDL
#endif /* PROBLEMS_H */
