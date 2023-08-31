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
#include <cstring>
#include <typeinfo>

#include "problems.h"
#include "domains.h"

/*******************************************************************************
              Wrapper for the parser that returns a single problem
*/

/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;

namespace PPDDL {

extern std::string current_file;

Problem const* parsePPDDL(std::string const& domain_problem_fname) {
  yyin = fopen(domain_problem_fname.c_str(), "r");

  Problem const* problem = nullptr;

  if (yyin == 0) {
    std::cerr << "mdpsim: " << domain_problem_fname << ": " << strerror(errno)
              << std::endl;
  } else {
    current_file = domain_problem_fname;
    bool success = (yyparse() == 0);
    if (success) {
      size_t p = 0;
      for (auto pi = Problem::begin(); pi != Problem::end(); ++pi, ++p) {
        problem = pi->second;
      }
      if (p > 1) {
        std::cout << "Found " << p << " problems in " << domain_problem_fname
                  << ". Expected just 1. Quitting\n";
        assert(false);
        exit(-1);
      }
    }
  }
  fclose(yyin);
  return problem;
}

Problem const* parsePPDDL(std::string const& domain_fname, std::string const& problem_fname) {
  // Parsing the domain first. There should be no problem in that file
  Problem const* no_problem = parsePPDDL(domain_fname);
  assert(no_problem == nullptr);
  // Parsing the problem
  return parsePPDDL(problem_fname);
}
}  // namespace PPDDL
/*******************************************************************************/



namespace PPDDL {
/* ====================================================================== */
/* Problem */

/* Table of defined problems. */
Problem::ProblemMap Problem::problems = Problem::ProblemMap();


/* Returns a const_iterator pointing to the first problem. */
Problem::ProblemMap::const_iterator Problem::begin() {
  return problems.begin();
}


/* Returns a const_iterator pointing beyond the last problem. */
Problem::ProblemMap::const_iterator Problem::end() {
  return problems.end();
}


/* Returns a pointer to the problem with the given name, or 0 if it
   is undefined. */
const Problem* Problem::find(const std::string& name) {
  ProblemMap::const_iterator pi = problems.find(name);
  return (pi != problems.end()) ? (*pi).second : 0;
}


/* Removes all defined problems. */
void Problem::clear() {
  ProblemMap::const_iterator pi = begin();
  while (pi != end()) {
    delete (*pi).second;
    pi = begin();
  }
  problems.clear();
}


#ifndef INITIAL_ACTION_CAP
#define INITIAL_ACTION_CAP 1024
#endif

/* Constructs a problem. */
Problem::Problem(const std::string& name, const Domain& domain)
  : name_(name), domain_(&domain), terms_(TermTable(domain.terms())),
    goal_(&StateFormula::FALSE), goal_reward_(0),
    actions_(INITIAL_ACTION_CAP, nullptr)
{
  RCObject::ref(goal_);
  const Problem* p = find(name);
  if (p != 0) {
    delete p;
  }
  problems[name] = this;
}


/* Deletes a problem. */
Problem::~Problem() {
  problems.erase(name());
  for (AtomSet::const_iterator ai = init_atoms_.begin();
       ai != init_atoms_.end(); ai++) {
    RCObject::destructive_deref(*ai);
  }
  for (ValueMap::const_iterator vi = init_values_.begin();
       vi != init_values_.end(); vi++) {
    RCObject::destructive_deref((*vi).first);
  }
  for (EffectList::const_iterator ei = init_effects_.begin();
       ei != init_effects_.end(); ei++) {
    RCObject::destructive_deref(*ei);
  }
  RCObject::destructive_deref(goal_);
  if (goal_reward_ != 0) {
    delete goal_reward_;
  }
  for (auto ai = actions_.begin(); ai != actions_.end(); ++ai) {
    delete *ai;
  }
  for (auto const& expr_ptr : metrics_) {
    RCObject::destructive_deref(expr_ptr);
  }
}


/* Adds an atomic state formula to the initial conditions of this
   problem. */
void Problem::add_init_atom(const Atom& atom) {
  if (init_atoms_.find(&atom) == init_atoms_.end()) {
    init_atoms_.insert(&atom);
    RCObject::ref(&atom);
  }
}


/* Adds a fluent value to the initial conditions of this problem. */
void Problem::add_init_value(const Fluent& fluent, const Rational& value) {
  if (init_values_.find(&fluent) == init_values_.end()) {
    init_values_.insert(std::make_pair(&fluent, value));
    RCObject::ref(&fluent);
  } else {
    init_values_[&fluent] = value;
  }
}


/* Adds an initial effect for this problem. */
void Problem::add_init_effect(const Effect& effect) {
  init_effects_.push_back(&effect);
  RCObject::ref(&effect);
}


/* Sets the goal for this problem. */
void Problem::set_goal(const StateFormula& goal) {
  if (&goal != goal_) {
    RCObject::ref(&goal);
    RCObject::destructive_deref(goal_);
    goal_ = &goal;
  }
}


/* Sets the goal reward for this problem. */
void Problem::set_goal_reward(const Update& goal_reward) {
  if (&goal_reward != goal_reward_) {
    delete goal_reward_;
    goal_reward_ = &goal_reward;
  }
}


void Problem::set_metrics(VecExpression& metrics, bool minimize) {
  assert(metrics_.size() == 0);
  for (Expression const*& expr : metrics) {
    assert(expr != nullptr);
    if (minimize) {
      // Adding to the ad-hoc ref counter and copying it
      RCObject::ref(expr);
      metrics_.push_back(expr);
    }
    else {
      // Changing sign of expression and adding to the internal list
      metrics_.push_back(&Subtraction::make(*new Value(0), *expr));
    }
    // Dereferencing the expr because it will not be used in the parser anymore
    RCObject::destructive_deref(expr);
  }
}


State Problem::initialState() const {
  State s{init_atoms(), init_values()};
  for (auto const& init_eff : init_effects()) {
    AtomList adds;
    AtomList deletes;
    UpdateList updates;
    init_eff->state_change(adds, deletes, updates, s.atoms, s.values);
    s.atoms.insert(adds.begin(), adds.end());
    for (auto const& upd : updates) {
      upd->affect(s.values);
    }
  }
  return s;
}


/* Grounds/Instantiates this problem. */
void Problem::ground() {
  set_goal(goal().instantiation(SubstitutionMap(), terms(),
                                init_atoms(), init_values(),
                                // false because this is a formula being grounded
                                // instead of being simplified for a state
                                false));
  if (goal_reward() != 0) {
    set_goal_reward(goal_reward()->instantiation(SubstitutionMap(),
                                                 init_values()));
  }
  VecExpression lifted_metrics = metrics_;
  metrics_.clear();
  for (auto const& expr_ptr : lifted_metrics) {
    metrics_.push_back(&expr_ptr->instantiation(SubstitutionMap(), init_values()));
  }
  for (ActionSchemaMap::const_iterator ai = domain().actions().begin();
       ai != domain().actions().end(); ai++) {
    (*ai).second->instantiations(actions_, terms(),
                                 init_atoms(), init_values());
  }
}


/* Tests if the metric is constant. */
bool Problem::constant_metric() const {
  if (metrics().size() == 0)
    return true;
  for (auto const& expr_ptr : metrics()) {
    if (typeid(*expr_ptr) != typeid(Value))
      return false;
  }
  return true;
}


/* Fills the given list with actions enabled in the given state. */
void Problem::enabledActions(VecActionPtr& rv, State const& s) const {
  for (auto const& a : actions()) {
    // FWT: Why this fails? assert(a != nullptr);
    if (a != nullptr && a->enabled(s)) {
      rv.push_back(a);
    }
  }
}


/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p) {
  os << "name: " << p.name();
  os << std::endl << "domain: " << p.domain().name();
  os << std::endl << "objects:" << p.terms();
  os << std::endl << "init:";
  for (AtomSet::const_iterator ai = p.init_atoms().begin();
       ai != p.init_atoms().end(); ai++) {
    os << std::endl << "  " << **ai;
  }
  for (ValueMap::const_iterator vi = p.init_values().begin();
       vi != p.init_values().end(); vi++) {
    os << std::endl << "  (= " << *(*vi).first << ' ' << (*vi).second << ")";
  }
  for (EffectList::const_iterator ei = p.init_effects().begin();
       ei != p.init_effects().end(); ei++) {
    os << std::endl << "  " << **ei;
  }
  os << std::endl << "goal: " << p.goal();
  if (p.goal_reward() != 0) {
    os << std::endl << "goal reward: " << p.goal_reward()->expression();
  }
  os << std::endl << "metric: " << p.metric();
  os << std::endl << "actions:";
  for (auto ai = p.actions().begin(); ai != p.actions().end(); ++ai) {
    os << std::endl << "  " << **ai;
  }
  return os;
}

const Atom* getAtom(const Problem &problem, const std::string &pred_name,
                    const std::vector<std::string> &term_names) {
  const Predicate* p =
    problem.domain().predicates().find_predicate(pred_name);
  if (!p) {
    return nullptr;
  }

  TermList terms;
  for (size_t argIndex = 0; argIndex < term_names.size(); argIndex++) {
    if (argIndex >= PredicateTable::parameters(*p).size()) {
      return nullptr;
    }
    Type correctType = PredicateTable::parameters(*p)[argIndex];

    std::string term_name = term_names.at(argIndex);
    const Object* o = problem.terms().find_object(term_name);
    if (o != 0) {
      if (!TypeTable::subtype(TermTable::type(*o), correctType)) {
        return 0;
      }
    } else {
      o = problem.domain().terms().find_object(term_name);
      if (o == 0) {
        return 0;
      } else if (!TypeTable::subtype(TermTable::type(*o), correctType)) {
        return 0;
      }
    }

    terms.push_back(*o);
  }

  if (PredicateTable::parameters(*p).size() != terms.size()) {
    return nullptr;
  }

  // LEGACY(FWT): Ignoring the flag saying if the atom is new (as in the original
  // code) since it seems to not matter here
  return &(Atom::make(*p, terms).first);
}
}  // namespace PPDDL
