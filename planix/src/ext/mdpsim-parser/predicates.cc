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
#include "predicates.h"


namespace PPDDL {

/* ====================================================================== */
/* Predicate */

/* Output operator for predicates. */
std::ostream& operator<<(std::ostream& os, const Predicate& p) {
  os << PredicateTable::names_[p.index_];
  return os;
}


/* ====================================================================== */
/* PredicateTable */

/* Predicate names. */
std::vector<std::string> PredicateTable::names_;
/* Predicate parameters. */
std::vector<TypeList> PredicateTable::parameters_;
/* Static predicates. */
PredicateSet PredicateTable::static_predicates_;


/* Adds a parameter with the given type to the given predicate. */
void PredicateTable::add_parameter(const Predicate& predicate,
                                   const Type& type) {
  parameters_[predicate.index_].push_back(type);
}


/* Returns the name of the given predicate. */
const std::string& PredicateTable::name(const Predicate& predicate) {
  return names_[predicate.index_];
}


/* Returns the parameter types of the given predicate. */
const TypeList& PredicateTable::parameters(const Predicate& predicate) {
  return parameters_[predicate.index_];
}


/* Makes the given predicate dynamic. */
void PredicateTable::make_dynamic(const Predicate& predicate) {
  static_predicates_.erase(predicate);
}


/* Tests if the given predicate is static. */
bool PredicateTable::static_predicate(const Predicate& predicate) {
  return static_predicates_.find(predicate) != static_predicates_.end();
}


/* Adds a predicate with the given name to this table and returns
   the predicate. */
const Predicate& PredicateTable::add_predicate(const std::string& name) {
  std::pair<std::map<std::string, Predicate>::const_iterator, bool> pi =
    predicates_.insert(std::make_pair(name, Predicate(names_.size())));
  const Predicate& predicate = (*pi.first).second;
  names_.push_back(name);
  parameters_.push_back(TypeList());
  static_predicates_.insert(predicate);
  return predicate;
}


/* Returns a pointer to the predicate with the given name, or 0 if
   no predicate with the given name exists. */
const Predicate*
PredicateTable::find_predicate(const std::string& name) const {
  std::map<std::string, Predicate>::const_iterator pi = predicates_.find(name);
  if (pi != predicates_.end()) {
    return &(*pi).second;
  } else {
    return nullptr;
  }
}


/* Output operator for predicate tables. */
std::ostream& operator<<(std::ostream& os, const PredicateTable& t) {
  for (std::map<std::string, Predicate>::const_iterator pi =
         t.predicates_.begin();
       pi != t.predicates_.end(); pi++) {
    const Predicate& p = (*pi).second;
    os << std::endl << "  (" << p;
    const TypeList& types = PredicateTable::parameters(p);
    for (TypeList::const_iterator ti = types.begin();
         ti != types.end(); ti++) {
      os << " ?v - " << *ti;
    }
    os << ")";
    if (PredicateTable::static_predicate(p)) {
      os << " <static>";
    }
  }
  return os;
}
}  // namespace PPDDL
