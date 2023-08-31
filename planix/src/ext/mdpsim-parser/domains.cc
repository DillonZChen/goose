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
#include "domains.h"

namespace PPDDL {

/* ====================================================================== */
/* Domain */

/* Table of defined domains. */
Domain::DomainMap Domain::domains = Domain::DomainMap();


/* Returns a const_iterator pointing to the first domain. */
Domain::DomainMap::const_iterator Domain::begin() {
  return domains.begin();
}


/* Returns a const_iterator pointing beyond the last domain. */
Domain::DomainMap::const_iterator Domain::end() {
  return domains.end();
}


/* Returns a pointer to the domain with the given name, or 0 if it
   is undefined. */
const Domain* Domain::find(const std::string& name) {
  DomainMap::const_iterator di = domains.find(name);
  return (di != domains.end()) ? (*di).second : 0;
}


/* Removes all defined domains. */
void Domain::clear() {
  DomainMap::const_iterator di = begin();
  while (di != end()) {
    delete (*di).second;
    di = begin();
  }
  domains.clear();
}


/* Constructs an empty domain with the given name. */
Domain::Domain(const std::string& name)
  : name_(name), total_time_(functions_.add_function("total-time")),
    goal_achieved_(functions_.add_function("goal-achieved")) {
  const Domain* d = find(name);
  if (d != 0) {
    delete d;
  }
  domains[name] = this;
  FunctionTable::make_dynamic(total_time_);
  FunctionTable::make_dynamic(goal_achieved_);
}


/* Deletes a domain. */
Domain::~Domain() {
  domains.erase(name());
  for (ActionSchemaMap::const_iterator ai = actions().begin();
       ai != actions().end(); ai++) {
    delete (*ai).second;
  }
  if (Atom::sizeOfAtomTable() > 0) {
    std::cout << "[Domain DTOR]: number of Atom::atoms_.size() == "
              << Atom::sizeOfAtomTable()
              << "  This is evidence of a memory leak. Aborting with assert"
              << std::endl;
    assert(Atom::sizeOfAtomTable() == 0);
  }
}


/* Adds the given action to this domain. */
void Domain::add_action(const ActionSchema& action) {
  actions_.insert(std::make_pair(action.name(), &action));
}


/* Returns a pointer to the action with the given name, or 0 if
   there is no action with the given name. */
const ActionSchema* Domain::find_action(const std::string& name) const {
  ActionSchemaMap::const_iterator ai = actions_.find(name);
  return (ai != actions_.end()) ? (*ai).second : 0;
}


/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d) {
  os << "name: " << d.name();
  os << std::endl << "types:" << d.types();
  os << std::endl << "constants:" << d.terms();
  os << std::endl << "predicates:" << d.predicates();
  os << std::endl << "functions:" << d.functions();
  os << std::endl << "actions:";
  for (ActionSchemaMap::const_iterator ai = d.actions().begin();
       ai != d.actions().end(); ai++) {
    os << std::endl << *(*ai).second;
  }
  return os;
}
}  // namespace PPDDL
