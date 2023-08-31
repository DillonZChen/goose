/* -*-C++-*- */
/*
 * Types.
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
#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>



namespace PPDDL {

/* ====================================================================== */
/* Type */

/*
 * A type.
 */
struct Type {
  /* Constructs a type. */
  explicit Type(int index) : index_(index) {}

  /* Tests if this is a simple type. */
  bool simple() const { return index_ >= 0; }

 private:
  /* Type index. */
  int index_;

  friend bool operator==(const Type& t1, const Type& t2);
  friend bool operator<(const Type& t1, const Type& t2);
  friend std::ostream& operator<<(std::ostream& os, const Type& t);
  friend struct TypeTable;
};

/* Equality operator for types. */
inline bool operator==(const Type& t1, const Type& t2) {
  return t1.index_ == t2.index_;
}

/* Inequality operator for types. */
inline bool operator!=(const Type& t1, const Type& t2) {
  return !(t1 == t2);
}

/* Less-than operator for types. */
inline bool operator<(const Type& t1, const Type& t2) {
  return t1.index_ < t2.index_;
}

/* Output operator for types. */
std::ostream& operator<<(std::ostream& os, const Type& t);


/* ====================================================================== */
/* TypeList */

using TypeList = std::vector<Type>;


/* ====================================================================== */
/* TypeSet */

using TypeSet = std::set<Type>;


/* ====================================================================== */
/* TypeTable */

/*
 * Type table.
 */
struct TypeTable {
  /* The object type. */
  static const Type OBJECT;
  /* Name of object type. */
  static const std::string OBJECT_NAME;
  /* Name of number type. */
  static const std::string NUMBER_NAME;

  /* Adds a union type of the given types to this table and returns
     the union type. */
  static Type union_type(const TypeSet& types);

  /* Adds the second type as a supertype of the first type.  Returns
     false if the second type is a proper subtype of the first
     type. */
  static bool add_supertype(const Type& type1, const Type& type2);

  /* Tests if the first type is a subtype of the second type. */
  static bool subtype(const Type& type1, const Type& type2);

  /* Fills the provided set with the components of the given type. */
  static void components(TypeSet& components, const Type& type);

  /* Adds a simple type with the given name to this table and returns
     the type. */
  const Type& add_type(const std::string& name);

  /* Returns a pointer to the type with the given name, or 0 if no
     type with the given name exists in this table. */
  const Type* find_type(const std::string& name) const;

 private:
  /* Type names. */
  static std::vector<std::string> names_;
  /* Transitive closure of subtype relation. */
  static std::vector<std::vector<bool> > subtype_;
  /* Union types. */
  static std::vector<TypeSet> utypes_;

  /* Mapping of type names to types. */
  std::map<std::string, Type> types_;

  friend std::ostream& operator<<(std::ostream& os, const TypeTable& t);
  friend std::ostream& operator<<(std::ostream& os, const Type& t);
};

/* Output operator for type tables. */
std::ostream& operator<<(std::ostream& os, const TypeTable& t);

}  // namespace PPDDL
#endif /* TYPES_H */
