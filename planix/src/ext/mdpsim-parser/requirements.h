/* -*-C++-*- */
/*
 * PDDL requirements.
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
#ifndef REQUIREMENTS_H
#define REQUIREMENTS_H



namespace PPDDL {

/* ====================================================================== */
/* Requirements */

/*
 * PDDL requirements.
 */
struct Requirements {
  /* Required action style. */
  bool strips;
  /* Whether support for types is required. */
  bool typing;
  /* Whether support for negative preconditions is required. */
  bool negative_preconditions;
  /* Whether support for disjunctive preconditions is required. */
  bool disjunctive_preconditions;
  /* Whether support for equality predicate is required. */
  bool equality;
  /* Whether support for existentially quantified preconditions is
     required. */
  bool existential_preconditions;
  /* Whether support for universally quantified preconditions is
     required. */
  bool universal_preconditions;
  /* Whether support for conditional effects is required. */
  bool conditional_effects;
  /* Whether support for fluents is required. */
  bool fluents;
  /* Whether support for probabilistic effects is required. */
  bool probabilistic_effects;
  /* Whether support for rewards is required. */
  bool rewards;

  /* Constructs a default requirements object. */
  Requirements();

  /* Enables quantified preconditions. */
  void quantified_preconditions();

  /* Enables ADL style actions. */
  void adl();

  /* Enables MDP planning problems. */
  void mdp();
};


}  // namespace PPDDL
#endif /* REQUIREMENTS_H */
