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
#include "functions.h"


namespace PPDDL {

/* ====================================================================== */
/* Function */

std::string const& Function::name() const {
  return FunctionTable::names_[index_];
}

/* Output operator for functions. */
std::ostream& operator<<(std::ostream& os, const Function& f) {
  os << f.name();
  return os;
}


/* ====================================================================== */
/* FunctionTable */

/* Function names. */
std::vector<std::string> FunctionTable::names_;
/* Function parameters. */
std::vector<TypeList> FunctionTable::parameters_;
/* Static functions. */
FunctionSet FunctionTable::static_functions_;


/* Adds a parameter with the given type to the given function. */
void FunctionTable::add_parameter(const Function& function, const Type& type) {
  parameters_[function.index_].push_back(type);
}


/* Returns the name of the given function. */
const std::string& FunctionTable::name(const Function& function) {
  return names_[function.index_];
}


/* Returns the parameter types of the given function. */
const TypeList& FunctionTable::parameters(const Function& function) {
  return parameters_[function.index_];
}


/* Makes the given function dynamic. */
void FunctionTable::make_dynamic(const Function& function) {
  static_functions_.erase(function);
}


/* Tests if the given function is static. */
bool FunctionTable::static_function(const Function& function) {
  return static_functions_.find(function) != static_functions_.end();
}


/* Adds a function with the given name to this table and returns the
   function. */
const Function& FunctionTable::add_function(const std::string& name) {
  std::pair<std::map<std::string, Function>::const_iterator, bool> fi =
    functions_.insert(std::make_pair(name, Function(names_.size())));
  const Function& function = (*fi.first).second;
  names_.push_back(name);
  parameters_.push_back(TypeList());
  static_functions_.insert(function);
  return function;
}


/* Returns a pointer to the function with the given name, or 0 if no
   function with the given name exists. */
const Function* FunctionTable::find_function(const std::string& name) const {
  std::map<std::string, Function>::const_iterator fi = functions_.find(name);
  if (fi != functions_.end()) {
    return &(*fi).second;
  } else {
    return 0;
  }
}


/* Output operator for function tables. */
std::ostream& operator<<(std::ostream& os, const FunctionTable& t) {
  for (std::map<std::string, Function>::const_iterator fi =
         t.functions_.begin();
       fi != t.functions_.end(); fi++) {
    const Function& f = (*fi).second;
    os << std::endl << "  (" << f;
    const TypeList& types = FunctionTable::parameters(f);
    for (TypeList::const_iterator ti = types.begin();
         ti != types.end(); ti++) {
      os << " ?v - " << *ti;
    }
    os << ") - " << TypeTable::NUMBER_NAME;
    if (FunctionTable::static_function(f)) {
      os << " <static>";
    }
  }
  return os;
}
}  // namespace PPDDL
