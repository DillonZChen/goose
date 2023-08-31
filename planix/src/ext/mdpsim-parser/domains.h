/* -*-C++-*- */
/*
 * Domain descriptions.
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
#ifndef DOMAINS_H
#define DOMAINS_H

#include <iostream>
#include <map>
#include <string>

#include "actions.h"
#include "functions.h"
#include "predicates.h"
#include "terms.h"
#include "types.h"
#include "requirements.h"


namespace PPDDL {

struct Problem;


/* ====================================================================== */
/* Domain */

/*
 * Domain definition.
 */
struct Domain {
  /* Table of domain definitions. */
  using DomainMap = std::map<std::string, const Domain*>;

  /* Requirements for this domain. */
  Requirements requirements;

  /* Returns a const_iterator pointing to the first domain. */
  static DomainMap::const_iterator begin();

  /* Returns a const_iterator pointing beyond the last domain. */
  static DomainMap::const_iterator end();

  /* Returns a pointer to the domain with the given name, or 0 if it
     is undefined. */
  static const Domain* find(const std::string& name);

  /* Removes all defined domains. */
  static void clear();

  /* Constructs an empty domain with the given name. */
  Domain(const std::string& name);

  /* Deletes a domain. */
  ~Domain();

  /* Returns the name of this domain. */
  const std::string& name() const { return name_; }

  /* Returns the type table of this domain. */
  TypeTable& types() { return types_; }

  /* Returns the type table of this domain. */
  const TypeTable& types() const { return types_; }

  /* Returns the predicate table of this domain. */
  PredicateTable& predicates() { return predicates_; }

  /* Returns the predicate table of this domain. */
  const PredicateTable& predicates() const { return predicates_; }

  /* Returns the function table of this domain. */
  FunctionTable& functions() { return functions_; }

  /* Returns the function table of this domain. */
  const FunctionTable& functions() const { return functions_; }

  /* Returns the `total-time' function. */
  const Function& total_time() const { return total_time_; }

  /* Returns the `goal-achieved' function. */
  const Function& goal_achieved() const { return goal_achieved_; }

  /* Returns the term table of this domain. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this domain. */
  const TermTable& terms() const { return terms_; }

  /* Adds the given action to this domain. */
  void add_action(const ActionSchema& action);

  /* Returns a pointer to the action with the given name, or 0 if
     there is no action with the given name. */
  const ActionSchema* find_action(const std::string& name) const;

  /* Returns the actions of this domain. */
  const ActionSchemaMap& actions() const { return actions_; }

 private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Name of this domain. */
  std::string name_;
  /* Domain types. */
  TypeTable types_;
  /* Domain predicates. */
  PredicateTable predicates_;
  /* Domain functions. */
  FunctionTable functions_;
  /* The `total-time' function. */
  Function total_time_;
  /* The `goal-achieved' function. */
  Function goal_achieved_;
  /* Domain terms. */
  TermTable terms_;
  /* Domain actions. */
  ActionSchemaMap actions_;
};

/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d);


}  // namespace PPDDL
#endif /* DOMAINS_H */
