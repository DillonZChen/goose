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
#include <stack>
#include <utility>
#include <sstream>

#include "actions.h"


namespace PPDDL {
/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema with the given name. */
ActionSchema::ActionSchema(const std::string& name)
  : name_(name), precondition_(&StateFormula::TRUE), effect_(&Effect::EMPTY) {
  RCObject::ref(precondition_);
  RCObject::ref(effect_);
}


/* Deletes this action schema. */
ActionSchema::~ActionSchema() {
  RCObject::destructive_deref(precondition_);
  RCObject::destructive_deref(effect_);
}


/* Sets the precondition of this action schema. */
void ActionSchema::set_precondition(const StateFormula& precondition) {
  if (&precondition != precondition_) {
    RCObject::ref(&precondition);
    RCObject::destructive_deref(precondition_);
    precondition_ = &precondition;
  }
}


/* Sets the effect of this action schema. */
void ActionSchema::set_effect(const Effect& effect) {
  if (&effect != effect_) {
    RCObject::ref(&effect);
    RCObject::destructive_deref(effect_);
    effect_ = &effect;
  }
}


/* Fills the provided list with instantiations of this action
   schema. */
void ActionSchema::instantiations(VecActionPtr& actions, const TermTable& terms,
                                  const AtomSet& atoms,
                                  const ValueMap& values) const {
  size_t n = parameters().size();
  if (n == 0) {
    const StateFormula& precond =
      precondition().instantiation(SubstitutionMap(), terms,
                                   atoms, values, false);
    if (!precond.contradiction()) {
      actions.push_back(&instantiation(SubstitutionMap(), terms, atoms, values, precond));
    }
  } else {
    SubstitutionMap args;
    std::vector<const ObjectList*> arguments(n);
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      Type t = TermTable::type(parameters()[i]);
      arguments[i] = &terms.compatible_objects(t);
      if (arguments[i]->empty()) {
        return;
      }
      next_arg.push_back(arguments[i]->begin());
    }
    std::stack<const StateFormula*> preconds;
    preconds.push(&precondition());
    RCObject::ref(preconds.top());
    for (size_t i = 0; i < n; ) {
      args.insert(std::make_pair(parameters()[i], *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const StateFormula& precond =
        preconds.top()->instantiation(pargs, terms, atoms, values, false);
      preconds.push(&precond);
      RCObject::ref(preconds.top());
      if (i + 1 == n || precond.contradiction()) {
        if (!precond.contradiction()) {
          actions.push_back(&instantiation(args, terms, atoms, values, precond));
        }
        for (int j = i; j >= 0; j--) {
          RCObject::destructive_deref(preconds.top());
          preconds.pop();
          args.erase(parameters()[j]);
          next_arg[j]++;
          if (next_arg[j] == arguments[j]->end()) {
            if (j == 0) {
              i = n;
              break;
            } else {
              next_arg[j] = arguments[j]->begin();
            }
          } else {
            i = j;
            break;
          }
        }
      } else {
        i++;
      }
    }
    while (!preconds.empty()) {
      RCObject::destructive_deref(preconds.top());
      preconds.pop();
    }
  }
}


/* Returns an instantiation of this action schema. */
const Action& ActionSchema::instantiation(const SubstitutionMap& subst,
                                          const TermTable& terms,
                                          const AtomSet& atoms,
                                          const ValueMap& values,
                                          const StateFormula& precond) const {
  // Instantiated name
  std::ostringstream ost;
  ost << name();
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    SubstitutionMap::const_iterator si = subst.find(*vi);
    ost << " " << (*si).second;
  }
  Action* action = new Action(ost.str());

  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    SubstitutionMap::const_iterator si = subst.find(*vi);
    action->add_argument((*si).second);
  }
  action->set_precondition(precond);
  action->set_effect(effect().instantiation(subst, terms, atoms, values));
  return *action;
}


/* Output operator for action schemas. */
std::ostream& operator<<(std::ostream& os, const ActionSchema& a) {
  os << "  " << a.name();
  os << std::endl << "    parameters:";
  for (VariableList::const_iterator vi = a.parameters().begin();
       vi != a.parameters().end(); vi++) {
    os << ' ' << *vi;
  }
  os << std::endl << "    precondition: " << a.precondition();
  os << std::endl << "    effect: " << a.effect();
  return os;
}


/* ====================================================================== */
/* Action */

/* Constructs an action with the given name. */
Action::Action(const std::string& name)
  : name_(name), precondition_(&StateFormula::TRUE), effect_(&Effect::EMPTY) {
  RCObject::ref(precondition_);
  RCObject::ref(effect_);
}


/* Deletes this action. */
Action::~Action() {
  RCObject::destructive_deref(precondition_);
  RCObject::destructive_deref(effect_);
}


/* Sets the precondition of this action. */
void Action::set_precondition(const StateFormula& precondition) {
  if (&precondition != precondition_) {
    RCObject::ref(&precondition);
    RCObject::destructive_deref(precondition_);
    precondition_ = &precondition;
  }
}


/* Sets the effect of this action. */
void Action::set_effect(const Effect& effect) {
  if (&effect != effect_) {
    RCObject::ref(&effect);
    RCObject::destructive_deref(effect_);
    effect_ = &effect;
  }
}


/* Tests if this action is enabled in the given state. */
bool Action::enabled(AtomSet const& atoms, ValueMap const& values) const {
  return precondition().holds(atoms, values);
}


/* Changes the given state according to the effects of this action. */
void Action::affect(State& s) const {
  AtomList adds;
  AtomList deletes;
  UpdateList updates;
  effect().state_change(adds, deletes, updates, s.atoms, s.values);
  for (AtomList::const_iterator ai = deletes.begin();
       ai != deletes.end(); ai++) {
    s.atoms.erase(*ai);
  }
  s.atoms.insert(adds.begin(), adds.end());
  for (UpdateList::const_iterator ui = updates.begin();
       ui != updates.end(); ui++) {
    (*ui)->affect(s.values);
  }
}

State Action::apply(State const& s) const {
  State result{s.atoms, s.values};
  affect(result);
  return result;
}

/* Output operator for actions. */
std::ostream& operator<<(std::ostream& os, const Action& a) {
  os << '(' << a.name();
  for (ObjectList::const_iterator oi = a.arguments().begin();
       oi != a.arguments().end(); oi++) {
    os << ' ' << *oi;
  }
  os << ')';
//  os << " eff = " << a.effect();
  return os;
}
}  // namespace PPDDL
