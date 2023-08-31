/* -*-C++-*- */
/*
 * Tokenizer.
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
%{
#include <cctype>
#include <string>

#include "effects.h"
#include "formulas.h"
#include "expressions.h"
#include "types.h"
#include "rational.h"

#include "parser.tab.hh"


/* Current line number. */
size_t line_number;

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token);
/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s);

%}

%option case-insensitive never-interactive nounput noyywrap

IDENT   [A-Za-z]([A-Za-z0-9\-_])*

%%

define                          return make_string(yytext, DEFINE);
domain                          return make_string(yytext, DOMAIN_TOKEN);
problem                         return make_string(yytext, PROBLEM);
:requirements                   return REQUIREMENTS;
:types                          return TYPES;
:constants                      return CONSTANTS;
:predicates                     return PREDICATES;
:functions                      return FUNCTIONS;
:strips                         return STRIPS;
:typing                         return TYPING;
:negative-preconditions         return NEGATIVE_PRECONDITIONS;
:disjunctive-preconditions      return DISJUNCTIVE_PRECONDITIONS;
:equality                       return EQUALITY;
:existential-preconditions      return EXISTENTIAL_PRECONDITIONS;
:universal-preconditions        return UNIVERSAL_PRECONDITIONS;
:quantified-preconditions       return QUANTIFIED_PRECONDITIONS;
:conditional-effects            return CONDITIONAL_EFFECTS;
:fluents                        return FLUENTS;
:adl                            return ADL;
:durative-actions               return DURATIVE_ACTIONS;
:duration-inequalities          return DURATION_INEQUALITIES;
:continuous-effects             return CONTINUOUS_EFFECTS;
:probabilistic-effects          return PROBABILISTIC_EFFECTS;
:rewards                        return REWARDS;
:mdp                            return MDP;
:action                         return ACTION;
:parameters                     return PARAMETERS;
:precondition                   return PRECONDITION;
:effect                         return EFFECT;
:domain                         return PDOMAIN;
:objects                        return OBJECTS;
:init                           return INIT;
:goal-reward                    return GOAL_REWARD;
:goal                           return GOAL;
:metric                         return METRIC;
total-time                      return TOTAL_TIME;
goal-achieved                   return GOAL_ACHIEVED;
number                          return make_string(yytext, NUMBER_TOKEN);
object                          return make_string(yytext, OBJECT_TOKEN);
either                          return make_string(yytext, EITHER);
when                            return make_string(yytext, WHEN);
not                             return make_string(yytext, NOT);
and                             return make_string(yytext, AND);
or                              return make_string(yytext, OR);
imply                           return make_string(yytext, IMPLY);
exists                          return make_string(yytext, EXISTS);
forall                          return make_string(yytext, FORALL);
probabilistic                   return make_string(yytext, PROBABILISTIC);
assign                          return make_string(yytext, ASSIGN);
scale-up                        return make_string(yytext, SCALE_UP);
scale-down                      return make_string(yytext, SCALE_DOWN);
increase                        return make_string(yytext, INCREASE);
decrease                        return make_string(yytext, DECREASE);
minimize                        return make_string(yytext, MINIMIZE);
maximize                        return make_string(yytext, MAXIMIZE);
{IDENT}                         return make_string(yytext, NAME);
\?{IDENT}                       return make_string(yytext, VARIABLE);
[0-9]*[./]?[0-9]+               return make_number(yytext);
\<=                             return LE;
>=                              return GE;
[()\-=<>+*/]                    return yytext[0];
;.*$                            /* comment */
[ \t\r]+                        /* whitespace */
\n                              line_number++;
.                               return ILLEGAL_TOKEN;

%%

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token) {
  std::string* result = new std::string();
  for (const char* p = s; *p != '\0'; p++) {
    *result += tolower(*p);
  }
  yylval.str = result;
  return token;
}


/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s) {
  yylval.num = new PPDDL::Rational(s);
  return NUMBER;
}
