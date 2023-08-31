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
#include "types.h"
#include <stdexcept>


namespace PPDDL {

/* ====================================================================== */
/* Type */

/* Output operator for types. */
std::ostream& operator<<(std::ostream& os, const Type& t) {
  if (!t.simple()) {
    const TypeSet& types = TypeTable::utypes_[-t.index_ - 1];
    os << "(either";
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      os << ' ' << TypeTable::names_[(*ti).index_];
    }
    os << ")";
  } else if (t == TypeTable::OBJECT) {
    os << TypeTable::OBJECT_NAME;
  } else {
    os << TypeTable::names_[t.index_ - 1];
  }
  return os;
}


/* ====================================================================== */
/* TypeTable */

/* The object type. */
const Type TypeTable::OBJECT(0);
/* Name of object type. */
const std::string TypeTable::OBJECT_NAME("object");
/* Name of number type. */
const std::string TypeTable::NUMBER_NAME("number");

/* Type names. */
std::vector<std::string> TypeTable::names_;
/* Transitive closure of subtype relation. */
std::vector<std::vector<bool> > TypeTable::subtype_;
/* Union types. */
std::vector<TypeSet> TypeTable::utypes_;


/* Adds a union type of the given types to this table and returns the
   union type. */
Type TypeTable::union_type(const TypeSet& types) {
  if (types.empty()) {
    throw std::logic_error("empty union type");
  } else if (types.size() == 1) {
    return *types.begin();
  } else {
    utypes_.push_back(types);
    return Type(-utypes_.size());
  }
}


/* Adds the second type as a supertype of the first type.  Returns
   false if the intended supertype is a subtype of the first
   type. */
bool TypeTable::add_supertype(const Type& type1, const Type& type2) {
  if (!type2.simple()) {
    /*
     * Add all component types of type2 as supertypes of type1.
     */
    const TypeSet& types = utypes_[-type2.index_ - 1];
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      if (!add_supertype(type1, *ti)) {
        return false;
      }
    }
    return true;
  } else if (subtype(type1, type2)) {
    /* The first type is already a subtype of the second type. */
    return true;
  } else if (subtype(type2, type1)) {
    /* The second type is already a subtype of the first type. */
    return false;
  } else {
    /*
     * Make all subtypes of type1 subtypes of all supertypes of type2.
     */
    size_t n = names_.size();
    for (size_t k = 1; k <= n; k++) {
      if (subtype(Type(k), type1) && !subtype(Type(k), type2)) {
        for (size_t l = 1; l <= n; l++) {
          if (subtype(type2, Type(l))) {
            if (k > l) {
              subtype_[k - 2][2*k - l - 2] = true;
            } else {
              subtype_[l - 2][k - 1] = true;
            }
          }
        }
      }
    }
    return true;
  }
}


/* Tests if the first type is a subtype of the second type. */
bool TypeTable::subtype(const Type& type1, const Type& type2) {
  if (type1 == type2) {
    /* Same types. */
    return true;
  } else if (!type1.simple()) {
    /* Every component type of type1 must be a subtype of type2. */
    const TypeSet& types = utypes_[-type1.index_ - 1];
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      if (!subtype(*ti, type2)) {
        return false;
      }
    }
    return true;
  } else if (!type2.simple()) {
    /* type1 one must be a subtype of some component type of type2. */
    const TypeSet& types = utypes_[-type2.index_ - 1];
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      if (subtype(type1, *ti)) {
        return true;
      }
    }
    return false;
  } else if (type1 == OBJECT) {
    return false;
  } else if (type2 == OBJECT) {
    return true;
  } else if (type2 < type1) {
    return subtype_[type1.index_ - 2][2*type1.index_ - type2.index_ - 2];
  } else {
    return subtype_[type2.index_ - 2][type1.index_ - 1];
  }
}


/* Fills the provided set with the components of the given type. */
void TypeTable::components(TypeSet& components, const Type& type) {
  if (!type.simple()) {
    components = utypes_[-type.index_ - 1];
  } else if (type != OBJECT) {
    components.insert(type);
  }
}


/* Adds a simple type with the given name to this table and returns
   the type. */
const Type& TypeTable::add_type(const std::string& name) {
  names_.push_back(name);
  auto ti = types_.insert(std::make_pair(name, Type(names_.size())));
  const Type& type = (*ti.first).second;
  if (type.index_ > 1) {
    subtype_.push_back(std::vector<bool>(2*(type.index_ - 1), false));
  }
  return type;
}


/* Returns a pointer to the type with the given name, or 0 if no
   type with the given name exists in this table. */
const Type* TypeTable::find_type(const std::string& name) const {
  std::map<std::string, Type>::const_iterator ti = types_.find(name);
  if (ti != types_.end()) {
    return &(*ti).second;
  } else {
    return 0;
  }
}


/* Output operator for type tables. */
std::ostream& operator<<(std::ostream& os, const TypeTable& t) {
  for (std::map<std::string, Type>::const_iterator ti = t.types_.begin();
       ti != t.types_.end(); ti++) {
    const Type& t1 = (*ti).second;
    os << std::endl << "  " << t1;
    bool first = true;
    for (std::map<std::string, Type>::const_iterator tj = t.types_.begin();
         tj != t.types_.end(); tj++) {
      const Type& t2 = (*tj).second;
      if (t1 != t2 && TypeTable::subtype(t1, t2)) {
        if (first) {
          os << " <:";
          first = false;
        }
        os << ' ' << t2;
      }
    }
  }
  return os;
}
}  // namespace PPDDL
