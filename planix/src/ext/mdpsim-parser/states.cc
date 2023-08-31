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
#include "states.h"


/* Verbosity level. */
//extern int verbosity;



namespace PPDDL {

/* Output operator for states. */
std::ostream& operator<<(std::ostream& os, State const& s) {
  bool first = true;
  for (auto const& ai : s.atoms) {
    Atom const& atom = *ai;
    if (!PredicateTable::static_predicate(atom.predicate())) {
      if (first) {
        first = false;
      } else {
        os << ' ';
      }
      os << atom;
    }
  }
  for (auto const& vi : s.values) {
    const Fluent& fluent = *(vi.first);
//    if (fluent.function() != s.problem().domain().total_time()
//        && fluent.function() != s.problem().domain().goal_achieved()
//        && !FunctionTable::static_function(fluent.function())) {
      if (first) {
        first = false;
      } else {
        os << ' ';
      }
      os << "(= " << fluent << ' ' << vi.second << " = " << static_cast<double>(vi.second) << ")";
//    }
  }
//  if (s.goal()) {
//    os << " <goal>";
//  }
  return os;
}

}  // namespace PPDDL
