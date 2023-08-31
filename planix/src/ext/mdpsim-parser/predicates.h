/* -*-C++-*- */
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
#ifndef PREDICATES_H
#define PREDICATES_H

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "types.h"


namespace PPDDL {

/* ====================================================================== */
/* Predicate */

/*
 * A predicate.
 */
struct Predicate {
  /* Constructs a predicate. */
  explicit Predicate(int index) : index_(index) {}

 private:
  /* Predicate index. */
  int index_;

  friend bool operator==(const Predicate& p1, const Predicate& p2);
  friend bool operator<(const Predicate& p1, const Predicate& p2);
  friend std::ostream& operator<<(std::ostream& os, const Predicate& p);
  friend struct PredicateTable;
};

/* Equality operator for predicates. */
inline bool operator==(const Predicate& p1, const Predicate& p2) {
  return p1.index_ == p2.index_;
}

/* Inequality operator for predicates. */
inline bool operator!=(const Predicate& p1, const Predicate& p2) {
  return !(p1 == p2);
}

/* Less-than operator for predicates. */
inline bool operator<(const Predicate& p1, const Predicate& p2) {
  return p1.index_ < p2.index_;
}

/* Output operator for predicates. */
std::ostream& operator<<(std::ostream& os, const Predicate& p);


/* ====================================================================== */
/* PredicateSet */

/* Set of predicate declarations. */
using PredicateSet = std::set<Predicate>;


/* ====================================================================== */
/* PredicateTable */

/*
 * Predicate table.
 */
struct PredicateTable {
  /* Adds a parameter with the given type to the given predicate. */
  static void add_parameter(const Predicate& predicate, const Type& type);

  /* Returns the name of the given predicate. */
  static const std::string& name(const Predicate& predicate);

  /* Returns the parameter types of the given predicate. */
  static const TypeList& parameters(const Predicate& predicate);

  /* Makes the given predicate dynamic. */
  static void make_dynamic(const Predicate& predicate);

  /* Tests if the given predicate is static. */
  static bool static_predicate(const Predicate& predicate);

  /* Adds a predicate with the given name to this table and returns
     the predicate. */
  const Predicate& add_predicate(const std::string& name);

  /* Returns a pointer to the predicate with the given name, or 0 if
     no predicate with the given name exists. */
  const Predicate* find_predicate(const std::string& name) const;

 private:
  /* Predicate names. */
  static std::vector<std::string> names_;
  /* Predicate parameters. */
  static std::vector<TypeList> parameters_;
  /* Static predicates. */
  static PredicateSet static_predicates_;

  /* Mapping of predicate names to predicates. */
  std::map<std::string, Predicate> predicates_;

  friend std::ostream& operator<<(std::ostream& os, const PredicateTable& t);
  friend std::ostream& operator<<(std::ostream& os, const Predicate& p);
};

/* Output operator for predicate tables. */
std::ostream& operator<<(std::ostream& os, const PredicateTable& t);


}  // namespace PPDDL
#endif /* PREDICATES_H */
