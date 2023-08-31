/* -*-C++-*- */
/*
 * Functions.
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
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "types.h"


namespace PPDDL {

/* ====================================================================== */
/* Function */

/*
 * A function.
 */
struct Function {
  /* Constructs a function. */
  explicit Function(int index) : index_(index) {}

  std::string const& name() const;
 private:
  /* Function index. */
  int index_;

  friend bool operator==(const Function& f1, const Function& f2);
  friend bool operator<(const Function& f1, const Function& f2);
  friend struct FunctionTable;
};

/* Equality operator for functions. */
inline bool operator==(const Function& f1, const Function& f2) {
  return f1.index_ == f2.index_;
}

/* Inequality operator for functions. */
inline bool operator!=(const Function& f1, const Function& f2) {
  return !(f1 == f2);
}

/* Less-than operator for functions. */
inline bool operator<(const Function& f1, const Function& f2) {
  return f1.index_ < f2.index_;
}

/* Output operator for functions. */
std::ostream& operator<<(std::ostream& os, const Function& f);


/* ====================================================================== */
/* FunctionSet */

/* Set of function declarations. */
using FunctionSet = std::set<Function>;


/* ====================================================================== */
/* FunctionTable */

/*
 * Function table.
 */
struct FunctionTable {
  /* Adds a parameter with the given type to the given function. */
  static void add_parameter(const Function& function, const Type& type);

  /* Returns the name of the given function. */
  static const std::string& name(const Function& function);

  /* Returns the parameter types of the given function. */
  static const TypeList& parameters(const Function& function);

  /* Makes the given function dynamic. */
  static void make_dynamic(const Function& function);

  /* Tests if the given function is static. */
  static bool static_function(const Function& function);

  /* Adds a function with the given name to this table and returns the
     function. */
  const Function& add_function(const std::string& name);

  /* Returns a pointer to the function with the given name, or 0 if no
     function with the given name exists. */
  const Function* find_function(const std::string& name) const;

 private:
  /* Function names. */
  static std::vector<std::string> names_;
  /* Function parameters. */
  static std::vector<TypeList> parameters_;
  /* Static functions. */
  static FunctionSet static_functions_;

  /* Mapping of function names to functions. */
  std::map<std::string, Function> functions_;

  friend std::ostream& operator<<(std::ostream& os, const FunctionTable& t);
  friend std::string const& Function::name() const;
};

/* Output operator for function tables. */
std::ostream& operator<<(std::ostream& os, const FunctionTable& t);


}  // namespace PPDDL
#endif /* FUNCTIONS_H */
