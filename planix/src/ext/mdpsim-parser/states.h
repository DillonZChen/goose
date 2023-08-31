/* -*-C++-*- */
/*
 * States.
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
#ifndef STATES_H
#define STATES_H

#include <iostream>
#include <set>
#include <unordered_set>
#include <boost/functional/hash.hpp>

//#include "problems.h"
//#include "actions.h"
#include "formulas.h"
#include "expressions.h"


namespace PPDDL {

struct State {
  AtomSet atoms;
  ValueMap values;
  bool operator==(State const& s) const = default;
};

/* Output operator for State. */
std::ostream& operator<<(std::ostream& os, State const& s);

struct StateHash {
  size_t operator()(State const& s) const {
    std::size_t key = boost::hash_range(s.atoms.begin(), s.atoms.end());
    boost::hash_combine(key, boost::hash_range(s.values.begin(), s.values.end()));
    return key;
  }
};

template<typename T>
using PrState = std::unordered_map<State, T, StateHash>;

}  // namespace PPDDL
#endif /* STATES_H */
