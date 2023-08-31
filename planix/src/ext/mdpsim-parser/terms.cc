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
#include "terms.h"
#include <typeinfo>
#include <vector>
#include <memory>



namespace PPDDL {

/* ====================================================================== */
/* Term */

/* Converts this term to a variable.  Fails if the term is not a
   variable. */
Variable Term::as_variable() const {
  if (variable()) {
    return Variable(index_);
  } else {
    throw std::bad_cast();
  }
}


/* Output operator for terms. */
std::ostream& operator<<(std::ostream& os, const Term& t) {
  if (t.object()) {
    os << TermTable::names_[t.index_];
  } else {
    os << "?v" << -t.index_;
  }
  return os;
}


/* ====================================================================== */
/* TermTable */

/* Object names. */
std::vector<std::string> TermTable::names_;
/* Object types. */
TypeList TermTable::object_types_;
/* Variable types. */
TypeList TermTable::variable_types_;


/* Deletes this term table. */
TermTable::~TermTable() {
  // FWT: We don need to delete the items in compatible_ anymore because it holds
  // a shared_ptr now.
}


/* Adds a variable with the given name and type to this term table
   and returns the variable. */
Variable TermTable::add_variable(const Type& type) {
  variable_types_.push_back(type);
  return Variable(-variable_types_.size());
}


/* Sets the type of the given term. */
void TermTable::set_type(const Term& term, const Type& type) {
  if (term.object()) {
    object_types_[term.index_] = type;
  } else {
    variable_types_[-term.index_ - 1] = type;
  }
}


/* Returns the type of the given term. */
const Type& TermTable::type(const Term& term) {
  if (term.object()) {
    return object_types_[term.index_];
  } else {
    return variable_types_[-term.index_ - 1];
  }
}


/* Adds an object with the given name and type to this term table
   and returns the object. */
const Object& TermTable::add_object(const std::string& name,
                                    const Type& type) {
  std::pair<std::map<std::string, Object>::const_iterator, bool> oi =
    objects_.insert(std::make_pair(name, Object(names_.size())));
  names_.push_back(name);
  object_types_.push_back(type);
  return (*oi.first).second;
}


/* Returns the object with the given name, or 0 if no object with
   the given name exists. */
const Object* TermTable::find_object(const std::string& name) const {
  std::map<std::string, Object>::const_iterator oi = objects_.find(name);
  if (oi != objects_.end()) {
    return &(*oi).second;
  } else if (parent_ != 0) {
    return parent_->find_object(name);
  } else {
    return 0;
  }
}


/* Returns a list with objects that are compatible with the given
   type. */
const ObjectList& TermTable::compatible_objects(const Type& type) const {
  auto oi = compatible_.find(type);
  if (oi != compatible_.end()) {
    return *(*oi).second;
  }

  std::shared_ptr<ObjectList> objects;
  if (parent_ != 0) {
    objects = std::make_shared<ObjectList>(parent_->compatible_objects(type));
  } else {
    objects = std::make_shared<ObjectList>();
  }
  for (std::map<std::string, Object>::const_iterator oi = objects_.begin();
       oi != objects_.end(); oi++)
  {
    Object const& o = (*oi).second;
    if (TypeTable::subtype(TermTable::type(o), type)) {
      objects->push_back(o);
    }
  }
  // FWT: Adding it to the cache. Notice that before, the cache existed but was
  // not used.
  compatible_[type] = objects;
  return *objects;
}


/* Output operator for term tables. */
std::ostream& operator<<(std::ostream& os, const TermTable& t) {
  if (t.parent_ != 0) {
    os << *t.parent_;
  }
  for (std::map<std::string, Object>::const_iterator oi = t.objects_.begin();
       oi != t.objects_.end(); oi++) {
    const Object& o = (*oi).second;
    os << std::endl << "  " << o;
    os << " - " << TermTable::type(o);
  }
  return os;
}
}  // namespace PPDDL
