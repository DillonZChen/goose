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
#include "requirements.h"



namespace PPDDL {

/* ====================================================================== */
/* Requirements */

/* Constructs a default requirements object. */
Requirements::Requirements()
  : strips(true), typing(false), negative_preconditions(false),
    disjunctive_preconditions(false), equality(false),
    existential_preconditions(false), universal_preconditions(false),
    conditional_effects(false), fluents(false), probabilistic_effects(false),
    rewards(false) {}


/* Enables quantified preconditions. */
void Requirements::quantified_preconditions() {
  existential_preconditions = true;
  universal_preconditions = true;
}


/* Enables ADL style actions. */
void Requirements::adl() {
  strips = true;
  typing = true;
  negative_preconditions = true;
  disjunctive_preconditions = true;
  equality = true;
  quantified_preconditions();
  conditional_effects = true;
}


/* Enables MDP planning problems. */
void Requirements::mdp() {
  probabilistic_effects = true;
  rewards = true;
}
}  // namespace PPDDL
