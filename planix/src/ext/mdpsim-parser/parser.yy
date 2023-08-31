/* -*-C++-*- */
/*
 * Parser.
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
#include "problems.h"
#include "domains.h"
#include "actions.h"
#include "effects.h"
#include "formulas.h"
#include "expressions.h"
#include "functions.h"
#include "predicates.h"
#include "terms.h"
#include "types.h"
#include "rational.h"
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <typeinfo>


/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1

using namespace PPDDL;

/*
 * Context of free variables.
 */
struct Context {
  void push_frame() {
    frames_.push_back(VariableMap());
  }

  void pop_frame() {
    frames_.pop_back();
  }

  void insert(const std::string& name, const Variable& v) {
    frames_.back().insert(std::make_pair(name, v));
  }

  const Variable* shallow_find(const std::string& name) const {
    VariableMap::const_iterator vi = frames_.back().find(name);
    if (vi != frames_.back().end()) {
      return &(*vi).second;
    } else {
      return 0;
    }
  }

  const Variable* find(const std::string& name) const {
    for (std::vector<VariableMap>::const_reverse_iterator fi =
           frames_.rbegin(); fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi).find(name);
      if (vi != (*fi).end()) {
        return &(*vi).second;
      }
    }
    return 0;
  }

private:
  using VariableMap = std::map<std::string, Variable>;

  std::vector<VariableMap> frames_;
};


/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;
/*extern std::string current_file;*/

namespace PPDDL {
  /* Name of current file. */
  std::string current_file;
}  // namespace

/* Level of warnings. */
/*extern int warning_level;*/

/* Whether the last parsing attempt succeeded. */
static bool success = true;
/* Current domain. */
static Domain* domain;
/* Domains. */
static std::map<std::string, Domain*> domains;
/* Pointer to problem being parsed, or 0 if no problem is being parsed. */
static Problem* problem;
/* Current requirements. */
static Requirements* requirements;
/* The reward function, if rewards are required. */
static Function reward_function(-1);
/* Predicate being parsed. */
static const Predicate* predicate;
/* Whether predicate declaration is repeated. */
static bool repeated_predicate;
/* Function being parsed. */
static const Function* function;
/* Whether function declaration is repeated. */
static bool repeated_function;
/* Pointer to action being parsed, or 0 if no action is being parsed. */
static ActionSchema* action;
/* Current variable context. */
static Context context;
/* Predicate for atomic state formula being parsed. */
static const Predicate* atom_predicate;
/* Whether the predicate of the currently parsed atom was undeclared. */
static bool undeclared_atom_predicate;
/* Whether parsing effect fluents. */
static bool effect_fluent;
/* Whether parsing metric fluent. */
static bool metric_fluent;
/* Function for fluent being parsed. */
static const Function* fluent_function;
/* Whether the function of the currently parsed fluent was undeclared. */
static bool undeclared_fluent_function;
/* Paramerers for atomic state formula or fluent being parsed. */
static TermList term_parameters;
/* Quantified variables for effect or formula being parsed. */
static TermList quantified;
/* Most recently parsed term for equality formula. */
static Term eq_term(0);
/* Most recently parsed expression for equality formula. */
static const Expression* eq_expr;
/* The first term for equality formula. */
static Term first_eq_term(0);
/* The first expression for equality formula. */
static const Expression* first_eq_expr;
/* Kind of name map being parsed. */
static enum { TYPE_KIND, CONSTANT_KIND, OBJECT_KIND, VOID_KIND } name_kind;

/* Outputs an error message. */
static void yyerror(const std::string& s);
/* Outputs a warning message. */
static void yywarning(const std::string& s);
/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name);
/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
                         const std::string* domain_name);
/* Adds :typing to the requirements. */
static void require_typing();
/* Adds :fluents to the requirements. */
static void require_fluents();
/* Adds :disjunctive-preconditions to the requirements. */
static void require_disjunction();
/* Adds :conditional-effects to the requirements. */
static void require_conditional_effects();
/* Returns a simple type with the given name. */
static const Type& make_type(const std::string* name);
/* Returns the union of the given types. */
static Type make_type(const TypeSet& types);
/* Returns a simple term with the given name. */
static Term make_term(const std::string* name);
/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name);
/* Creates a function with the given name. */
static void make_function(const std::string* name);
/* Creates an action with the given name. */
static void make_action(const std::string* name);
/* Adds the current action to the current domain. */
static void add_action();
/* Prepares for the parsing of a universally quantified effect. */
static void prepare_forall_effect();
/* Creates a universally quantified effect. */
static const Effect* make_forall_effect(const Effect& effect);
/* Adds an outcome to the given probabilistic effect. */
static void add_outcome(std::vector<std::pair<Rational, const Effect*> >& os,
                        const Rational* p, const Effect& effect);
/* Creates a probabilistic effect. */
static const Effect*
make_prob_effect(const std::vector<std::pair<Rational, const Effect*> >* os);
/* Creates an add effect. */
static const Effect* make_add_effect(const Atom& atom);
/* Creates a delete effect. */
static const Effect* make_delete_effect(const Atom& atom);
/* Creates an assign update effect. */
static const Effect* make_assign_effect(const Fluent& fluent,
                                        const Expression& expr);
/* Creates a scale-up update effect. */
static const Effect* make_scale_up_effect(const Fluent& fluent,
                                          const Expression& expr);
/* Creates a scale-down update effect. */
static const Effect* make_scale_down_effect(const Fluent& fluent,
                                            const Expression& expr);
/* Creates an increase update effect. */
static const Effect* make_increase_effect(const Fluent& fluent,
                                          const Expression& expr);
/* Creates a decrease update effect. */
static const Effect* make_decrease_effect(const Fluent& fluent,
                                          const Expression& expr);
/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names,
                      const Type& type);
/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
                          const Type& type);
/* Prepares for the parsing of an atomic state formula. */
static void prepare_atom(const std::string* name);
/* Prepares for the parsing of a fluent. */
static void prepare_fluent(const std::string* name);
/* Adds a term with the given name to the current atomic state formula. */
static void add_term(const std::string* name);
/* Creates the atomic formula just parsed. */
static const Atom* make_atom();
/* Creates the fluent just parsed. */
static const Fluent* make_fluent();
/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
                                          const Expression* opt_term);
/* Creates an atom or fluent for the given name to be used in an
   equality formula. */
static void make_eq_name(const std::string* name);
/* Creates an equality formula. */
static const StateFormula* make_equality();
/* Creates a negated formula. */
static const StateFormula* make_negation(const StateFormula& negand);
/* Creates an implication. */
static const StateFormula* make_implication(const StateFormula& f1,
                                            const StateFormula& f2);
/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists();
/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall();
/* Creates an existentially quantified formula. */
static const StateFormula* make_exists(const StateFormula& body);
/* Creates a universally quantified formula. */
static const StateFormula* make_forall(const StateFormula& body);
/* Sets the goal reward for the current problem. */
static void set_goal_reward(const Expression& goal_reward);
/* Sets the default metric for the current problem. */
static void set_default_metric();
%}

%token DEFINE DOMAIN_TOKEN PROBLEM
%token REQUIREMENTS TYPES CONSTANTS PREDICATES FUNCTIONS
%token STRIPS TYPING NEGATIVE_PRECONDITIONS DISJUNCTIVE_PRECONDITIONS EQUALITY
%token EXISTENTIAL_PRECONDITIONS UNIVERSAL_PRECONDITIONS
%token QUANTIFIED_PRECONDITIONS CONDITIONAL_EFFECTS FLUENTS ADL
%token DURATIVE_ACTIONS DURATION_INEQUALITIES CONTINUOUS_EFFECTS
%token PROBABILISTIC_EFFECTS REWARDS MDP
%token ACTION PARAMETERS PRECONDITION EFFECT
%token PDOMAIN OBJECTS INIT GOAL GOAL_REWARD METRIC TOTAL_TIME GOAL_ACHIEVED
%token WHEN NOT AND OR IMPLY EXISTS FORALL PROBABILISTIC
%token ASSIGN SCALE_UP SCALE_DOWN INCREASE DECREASE MINIMIZE MAXIMIZE
%token NUMBER_TOKEN OBJECT_TOKEN EITHER
%token LE GE NAME VARIABLE NUMBER
%token ILLEGAL_TOKEN

%union {
  const PPDDL::Effect* effect;
  std::vector<std::pair<PPDDL::Rational, const PPDDL::Effect*> >* outcomes;
  const PPDDL::StateFormula* formula;
  const PPDDL::Atom* atom;
  const PPDDL::Expression* expr;
  PPDDL::VecExpression* vec_expr;
  const PPDDL::Fluent* fluent;
  const PPDDL::Type* type;
  PPDDL::TypeSet* types;
  const std::string* str;
  std::vector<const std::string*>* strs;
  const PPDDL::Rational* num;
}

%type <effect> eff_formula p_effect simple_init one_init eff_formulas one_inits
%type <outcomes> prob_effs prob_inits
%type <formula> formula conjuncts disjuncts
%type <atom> atomic_name_formula atomic_term_formula
%type <expr> value f_exp opt_f_exp ground_f_exp opt_ground_f_exp
%type <vec_expr> list_ground_f_exp
%type <fluent> ground_f_head f_head
%type <strs> name_seq variable_seq
%type <type> type_spec type
%type <types> types
%type <str> type_name predicate function name variable
%type <str> DEFINE DOMAIN_TOKEN PROBLEM
%type <str> WHEN NOT AND OR IMPLY EXISTS FORALL PROBABILISTIC
%type <str> ASSIGN SCALE_UP SCALE_DOWN INCREASE DECREASE MINIMIZE MAXIMIZE
%type <str> NUMBER_TOKEN OBJECT_TOKEN EITHER
%type <str> NAME VARIABLE
%type <num> probability NUMBER

%%

file : { success = true; line_number = 1; } domains_and_problems
         { if (!success) YYERROR; }
     ;

domains_and_problems : /* empty */
                     | domains_and_problems domain_def
                     | domains_and_problems problem_def
                     ;


/* ====================================================================== */
/* Domain definitions. */

domain_def : '(' define '(' domain name ')' { make_domain($5); }
               domain_body ')'
           ;

domain_body : /* empty */
            | require_def
            | require_def domain_body2
            | domain_body2
            ;

domain_body2 : types_def
             | types_def domain_body3
             | domain_body3
             ;

domain_body3 : constants_def
             | predicates_def
             | functions_def
             | constants_def domain_body4
             | predicates_def domain_body5
             | functions_def domain_body6
             | structure_defs
             ;

domain_body4 : predicates_def
             | functions_def
             | predicates_def domain_body7
             | functions_def domain_body8
             | structure_defs
             ;

domain_body5 : constants_def
             | functions_def
             | constants_def domain_body7
             | functions_def domain_body9
             | structure_defs
             ;

domain_body6 : constants_def
             | predicates_def
             | constants_def domain_body8
             | predicates_def domain_body9
             | structure_defs
             ;

domain_body7 : functions_def
             | functions_def structure_defs
             | structure_defs
             ;

domain_body8 : predicates_def
             | predicates_def structure_defs
             | structure_defs
             ;

domain_body9 : constants_def
             | constants_def structure_defs
             | structure_defs
             ;

structure_defs : structure_def
               | structure_defs structure_def
               ;

structure_def : action_def
              ;

require_def : '(' REQUIREMENTS require_keys ')'
            ;

require_keys : require_key
             | require_keys require_key
             ;

require_key : STRIPS { requirements->strips = true; }
            | TYPING { requirements->typing = true; }
            | NEGATIVE_PRECONDITIONS
                { requirements->negative_preconditions = true; }
            | DISJUNCTIVE_PRECONDITIONS
                { requirements->disjunctive_preconditions = true; }
            | EQUALITY { requirements->equality = true; }
            | EXISTENTIAL_PRECONDITIONS
                { requirements->existential_preconditions = true; }
            | UNIVERSAL_PRECONDITIONS
                { requirements->universal_preconditions = true; }
            | QUANTIFIED_PRECONDITIONS
                { requirements->quantified_preconditions(); }
            | CONDITIONAL_EFFECTS { requirements->conditional_effects = true; }
            | FLUENTS { requirements->fluents = true; }
            | ADL { requirements->adl(); }
            | DURATIVE_ACTIONS
                {
                  throw std::runtime_error("`:durative-actions'"
                                           " not supported");
                }
            | DURATION_INEQUALITIES
                {
                  throw std::runtime_error("`:duration-inequalities'"
                                           " not supported");
                }
            | CONTINUOUS_EFFECTS
                {
                  throw std::runtime_error("`:continuous-effects'"
                                           " not supported");
                }
            | PROBABILISTIC_EFFECTS
                {
                  requirements->probabilistic_effects = true;
                }
            | REWARDS
                {
                  requirements->rewards = true;
                  reward_function = domain->functions().add_function("reward");
                }
            | MDP
                {
                  requirements->mdp();
                  reward_function = domain->functions().add_function("reward");
                }
            ;

types_def : '(' TYPES { require_typing(); name_kind = TYPE_KIND; }
              typed_names ')' { name_kind = VOID_KIND; }
          ;

constants_def : '(' CONSTANTS { name_kind = CONSTANT_KIND; } typed_names ')'
                  { name_kind = VOID_KIND; }
              ;

predicates_def : '(' PREDICATES predicate_decls ')'
               ;

functions_def : '(' FUNCTIONS { require_fluents(); } function_decls ')'
              ;


/* ====================================================================== */
/* Predicate and function declarations. */

predicate_decls : /* empty */
                | predicate_decls predicate_decl
                ;

predicate_decl : '(' predicate { make_predicate($2); } variables ')'
                   { predicate = 0; }
               ;

function_decls : /* empty */
               | function_decl_seq
               | function_decl_seq function_type_spec function_decls
               ;

function_decl_seq : function_decl
                  | function_decl_seq function_decl
                  ;

function_type_spec : '-' { require_typing(); } function_type
                   ;

function_decl : '(' function { make_function($2); } variables ')'
                  { function = 0; }
              ;


/* ====================================================================== */
/* Action definitions. */

action_def : '(' ACTION name { make_action($3); }
               parameters action_body ')' { add_action(); }
           ;

parameters : /* empty */
           | PARAMETERS '(' variables ')'
           ;

action_body : precondition action_body2
            | action_body2
            ;

action_body2 : /* empty */
             | effect
             ;

precondition : PRECONDITION formula { action->set_precondition(*$2); }
             ;

effect : EFFECT eff_formula { action->set_effect(*$2); }
       ;


/* ====================================================================== */
/* Effect formulas. */

eff_formula : p_effect
            | '(' and eff_formulas ')' { $$ = $3; }
            | '(' forall { prepare_forall_effect(); } '(' variables ')'
                eff_formula ')' { $$ = make_forall_effect(*$7); }
            | '(' when { require_conditional_effects(); } formula
                eff_formula ')' { $$ = &ConditionalEffect::make(*$4, *$5); }
            | '(' probabilistic prob_effs ')'
                { $$ = make_prob_effect($3); }
            ;

eff_formulas : /* empty */ { $$ = &Effect::EMPTY; }
             | eff_formulas eff_formula { $$ = &(*$1 && *$2); }
             ;

prob_effs : probability eff_formula
              {
                $$ = new std::vector<std::pair<Rational, const Effect*> >();
                add_outcome(*$$, $1, *$2);
              }
          | prob_effs probability eff_formula
              { $$ = $1; add_outcome(*$$, $2, *$3); }
          ;

probability : NUMBER
            ;

p_effect : atomic_term_formula { $$ = make_add_effect(*$1); }
         | '(' not atomic_term_formula ')' { $$ = make_delete_effect(*$3); }
         | '(' assign { effect_fluent = true; } f_head f_exp ')'
             { $$ = make_assign_effect(*$4, *$5); }
         | '(' scale_up { effect_fluent = true; } f_head f_exp ')'
             { $$ = make_scale_up_effect(*$4, *$5); }
         | '(' scale_down { effect_fluent = true; } f_head f_exp ')'
             { $$ = make_scale_down_effect(*$4, *$5); }
         | '(' increase { effect_fluent = true; } f_head f_exp ')'
             { $$ = make_increase_effect(*$4, *$5); }
         | '(' decrease { effect_fluent = true; } f_head f_exp ')'
             { $$ = make_decrease_effect(*$4, *$5); }
         ;


/* ====================================================================== */
/* Problem definitions. */

problem_def : '(' define '(' problem name ')' '(' PDOMAIN name ')'
                { make_problem($5, $9); } problem_body ')'
                { problem->ground(); delete requirements; }
            ;

problem_body : require_def problem_body2
             | problem_body2
             ;

problem_body2 : object_decl problem_body3
              | problem_body3
              ;

problem_body3 : init goal_spec
              | goal_spec
              ;

object_decl : '(' OBJECTS { name_kind = OBJECT_KIND; } typed_names ')'
                { name_kind = VOID_KIND; }
            ;

init : '(' INIT init_elements ')'
     ;

init_elements : /* empty */
              | init_elements init_element
              ;

init_element : atomic_name_formula { problem->add_init_atom(*$1); }
             | '(' '=' ground_f_head NUMBER ')'
                 { problem->add_init_value(*$3, *$4); delete $4; }
             | '(' probabilistic prob_inits ')'
                 { problem->add_init_effect(*make_prob_effect($3)); }
             ;

prob_inits : probability simple_init
               {
                 $$ = new std::vector<std::pair<Rational, const Effect*> >();
                 add_outcome(*$$, $1, *$2);
               }
           | prob_inits probability simple_init
               { $$ = $1; add_outcome(*$$, $2, *$3); }
           ;

simple_init : one_init
            | '(' and one_inits ')' { $$ = $3; }
            ;

one_inits : /* empty */ { $$ = &Effect::EMPTY; }
          | one_inits one_init { $$ = &(*$1 && *$2); }
          ;

one_init : atomic_name_formula { $$ = make_add_effect(*$1); }
         | '(' '=' ground_f_head value ')'
             { $$ = make_assign_effect(*$3, *$4); }
         ;

value : NUMBER { $$ = new Value(*$1); delete $1; }
      ;

goal_spec : '(' GOAL formula ')' goal_reward { problem->set_goal(*$3); }
          | metric_spec
          ;

goal_reward : metric_spec
            | '(' GOAL_REWARD ground_f_exp ')' metric_spec
                { set_goal_reward(*$3); }
            ;

metric_spec : /* empty */ { set_default_metric(); }
            | '(' METRIC maximize { metric_fluent = true; } list_ground_f_exp ')'
                { problem->set_metrics(*$5, false); metric_fluent = false; }
            | '(' METRIC minimize { metric_fluent = true; } list_ground_f_exp ')'
                { problem->set_metrics(*$5, true); metric_fluent = false; }
            ;

/* ====================================================================== */
/* Formulas. */

formula : atomic_term_formula { $$ = $1; }
        | '(' '=' term_or_f_exp
            { first_eq_term = eq_term; first_eq_expr = eq_expr; }
            term_or_f_exp ')' { $$ = make_equality(); }
        | '(' '<' { require_fluents(); } f_exp f_exp ')'
            { $$ = &LessThan::make(*$4, *$5); }
        | '(' LE { require_fluents(); } f_exp f_exp ')'
            { $$ = &LessThanOrEqualTo::make(*$4, *$5); }
        | '(' GE { require_fluents(); } f_exp f_exp ')'
            { $$ = &GreaterThanOrEqualTo::make(*$4, *$5); }
        | '(' '>' { require_fluents(); } f_exp f_exp ')'
            { $$ = &GreaterThan::make(*$4, *$5); }
        | '(' not formula ')' { $$ = make_negation(*$3); }
        | '(' and conjuncts ')' { $$ = $3; }
        | '(' or { require_disjunction(); } disjuncts ')' { $$ = $4; }
        | '(' imply formula formula ')' { $$ = make_implication(*$3, *$4); }
        | '(' exists { prepare_exists(); } '(' variables ')' formula ')'
            { $$ = make_exists(*$7); }
        | '(' forall { prepare_forall(); } '(' variables ')' formula ')'
            { $$ = make_forall(*$7); }
        ;

conjuncts : /* empty */ { $$ = &StateFormula::TRUE; }
          | conjuncts formula { $$ = &(*$1 && *$2); }
          ;

disjuncts : /* empty */ { $$ = &StateFormula::FALSE; }
          | disjuncts formula { $$ = &(*$1 || *$2); }
          ;

atomic_term_formula : '(' predicate { prepare_atom($2); } terms ')'
                        { $$ = make_atom(); }
                    | predicate { prepare_atom($1); $$ = make_atom(); }
                    ;

atomic_name_formula : '(' predicate { prepare_atom($2); } names ')'
                        { $$ = make_atom(); }
                    | predicate { prepare_atom($1); $$ = make_atom(); }
                    ;


/* ====================================================================== */
/* Function expressions. */

f_exp : NUMBER { $$ = new Value(*$1); delete $1; }
      | '(' '+' f_exp f_exp ')' { $$ = &Addition::make(*$3, *$4); }
      | '(' '-' f_exp opt_f_exp ')' { $$ = make_subtraction(*$3, $4); }
      | '(' '*' f_exp f_exp ')' { $$ = &Multiplication::make(*$3, *$4); }
      | '(' '/' f_exp f_exp ')' { $$ = &Division::make(*$3, *$4); }
      | f_head { $$ = $1; }
      ;

term_or_f_exp : NUMBER
                  { require_fluents(); eq_expr = new Value(*$1); delete $1; }
              | '(' '+' { require_fluents(); } f_exp f_exp ')'
                  { eq_expr = &Addition::make(*$4, *$5); }
              | '(' '-' { require_fluents(); } f_exp opt_f_exp ')'
                  { eq_expr = make_subtraction(*$4, $5); }
              | '(' '*' { require_fluents(); } f_exp f_exp ')'
                  { eq_expr = &Multiplication::make(*$4, *$5); }
              | '(' '/' { require_fluents(); } f_exp f_exp ')'
                  { eq_expr = &Division::make(*$4, *$5); }
              | '(' function { require_fluents(); prepare_fluent($2); }
                  terms ')' { eq_expr = make_fluent(); }
              | name { make_eq_name($1); }
              | variable { eq_term = make_term($1); eq_expr = 0; }
              ;

opt_f_exp : /* empty */ { $$ = 0; }
          | f_exp
          ;

f_head : '(' function { prepare_fluent($2); } terms ')' { $$ = make_fluent(); }
       | function { prepare_fluent($1); $$ = make_fluent(); }
       ;

ground_f_exp : NUMBER { $$ = new Value(*$1); delete $1; }
             | '(' '+' ground_f_exp ground_f_exp ')'
                 { $$ = &Addition::make(*$3, *$4); }
             | '(' '-' ground_f_exp opt_ground_f_exp ')'
                 { $$ = make_subtraction(*$3, $4); }
             | '(' '*' ground_f_exp ground_f_exp ')'
                 { $$ = &Multiplication::make(*$3, *$4); }
             | '(' '/' ground_f_exp ground_f_exp ')'
                 { $$ = &Division::make(*$3, *$4); }
             | ground_f_head { $$ = $1; }
             | '(' TOTAL_TIME ')'
                 { $$ = &Fluent::make(domain->total_time(), TermList()); }
             | TOTAL_TIME
                 { $$ = &Fluent::make(domain->total_time(), TermList()); }
             | '(' GOAL_ACHIEVED ')'
                 { $$ = &Fluent::make(domain->goal_achieved(), TermList()); }
             | GOAL_ACHIEVED
                 { $$ = &Fluent::make(domain->goal_achieved(), TermList()); }
             ;

list_ground_f_exp : ground_f_exp { $$ = new VecExpression{$1}; }
                  | ground_f_exp list_ground_f_exp { $2->push_back($1); $$ = $2; }
                  
opt_ground_f_exp : /* empty */ { $$ = 0; }
                 | ground_f_exp
                 ;

ground_f_head : '(' function { prepare_fluent($2); } names ')'
                  { $$ = make_fluent(); }
              | function { prepare_fluent($1); $$ = make_fluent(); }
              ;


/* ====================================================================== */
/* Terms and types. */

terms : /* empty */
      | terms name { add_term($2); }
      | terms variable { add_term($2); }
      ;

names : /* empty */
      | names name { add_term($2); }
      ;

variables : /* empty */
          | variable_seq { add_variables($1, TypeTable::OBJECT); }
          | variable_seq type_spec { add_variables($1, *$2); delete $2; }
              variables
          ;

variable_seq : variable { $$ = new std::vector<const std::string*>(1, $1); }
             | variable_seq variable { $$ = $1; $$->push_back($2); }
             ;

typed_names : /* empty */
            | name_seq { add_names($1, TypeTable::OBJECT); }
            | name_seq type_spec { add_names($1, *$2); delete $2; } typed_names
            ;

name_seq : name { $$ = new std::vector<const std::string*>(1, $1); }
         | name_seq name { $$ = $1; $$->push_back($2); }
         ;

type_spec : '-' { require_typing(); } type { $$ = $3; }
          ;

type : object { $$ = new Type(TypeTable::OBJECT); }
     | type_name { $$ = new Type(make_type($1)); }
     | '(' either types ')' { $$ = new Type(make_type(*$3)); delete $3; }
     ;

types : object { $$ = new TypeSet(); }
      | type_name { $$ = new TypeSet(); $$->insert(make_type($1)); }
      | types object { $$ = $1; }
      | types type_name { $$ = $1; $$->insert(make_type($2)); }
      ;

function_type : number
              ;


/* ====================================================================== */
/* Tokens. */

define : DEFINE { delete $1; }
       ;

domain : DOMAIN_TOKEN { delete $1; }
       ;

problem : PROBLEM { delete $1; }
        ;

when : WHEN { delete $1; }
     ;

not : NOT { delete $1; }
    ;

and : AND { delete $1; }
    ;

or : OR { delete $1; }
   ;

imply : IMPLY { delete $1; }
      ;

exists : EXISTS { delete $1; }
       ;

forall : FORALL { delete $1; }
       ;

probabilistic : PROBABILISTIC { delete $1; }
              ;

assign : ASSIGN { delete $1; }
       ;

scale_up : SCALE_UP { delete $1; }
         ;

scale_down : SCALE_DOWN { delete $1; }
           ;

increase : INCREASE { delete $1; }
         ;

decrease : DECREASE { delete $1; }
         ;

minimize : MINIMIZE { delete $1; }
         ;

maximize : MAXIMIZE { delete $1; }
         ;

number : NUMBER_TOKEN { delete $1; }
       ;

object : OBJECT_TOKEN { delete $1; }
       ;

either : EITHER { delete $1; }
       ;

type_name : DEFINE | DOMAIN_TOKEN | PROBLEM
          | EITHER
          | MINIMIZE | MAXIMIZE
          | NAME
          ;

predicate : type_name
          | OBJECT_TOKEN | NUMBER_TOKEN
          ;

function : name
         ;

name : DEFINE | DOMAIN_TOKEN | PROBLEM
     | NUMBER_TOKEN | OBJECT_TOKEN | EITHER
     | WHEN | NOT | AND | OR | IMPLY | EXISTS | FORALL | PROBABILISTIC
     | ASSIGN | SCALE_UP | SCALE_DOWN | INCREASE | DECREASE
     | MINIMIZE | MAXIMIZE
     | NAME
     ;

variable : VARIABLE
         ;

%%

/* Outputs an error message. */
static void yyerror(const std::string& s) {
  std::cerr << "mdpsim: " << PPDDL::current_file << ':' << line_number << ": " << s
            << std::endl;
  success = false;
}


/* Outputs a warning. */
static void yywarning(const std::string& s) {
/*  if (warning_level > 0) {*/
    std::cerr << "mdpsim: " << PPDDL::current_file << ':' << line_number << ": " << s
              << std::endl;
/*    if (warning_level > 1) {*/
      success = false;
/*    }*/
/*  }*/
}


/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name) {
  domain = new Domain(*name);
  domains[*name] = domain;
  requirements = &domain->requirements;
  problem = 0;
  delete name;
}


/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
                         const std::string* domain_name) {
  std::map<std::string, Domain*>::const_iterator di =
    domains.find(*domain_name);
  if (di != domains.end()) {
    domain = (*di).second;
  } else {
    domain = new Domain(*domain_name);
    domains[*domain_name] = domain;
    yyerror("undeclared domain `" + *domain_name + "' used");
  }
  requirements = new Requirements(domain->requirements);
  problem = new Problem(*name, *domain);
/*  const Fluent& total_time_fluent =*/
/*    Fluent::make(domain->total_time(), TermList());*/
/*  const Update* total_time_update =*/
/*    new Assign(total_time_fluent, *new Value(0));*/
/*  problem->add_init_effect(UpdateEffect::make(*total_time_update));*/
/*  const Fluent& goal_achieved_fluent =*/
/*    Fluent::make(domain->goal_achieved(), TermList());*/
/*  const Update* goal_achieved_update =*/
/*    new Assign(goal_achieved_fluent, *new Value(0));*/
/*  problem->add_init_effect(UpdateEffect::make(*goal_achieved_update));*/
  if (requirements->rewards) {
    reward_function = *domain->functions().find_function("reward");
    const Fluent& reward_fluent = Fluent::make(reward_function, TermList());
    const Update* reward_update = new Assign(reward_fluent, *new Value(0));
    problem->add_init_effect(UpdateEffect::make(*reward_update));
  }
  delete name;
  delete domain_name;
}


/* Adds :typing to the requirements. */
static void require_typing() {
  if (!requirements->typing) {
    yywarning("assuming `:typing' requirement");
    requirements->typing = true;
  }
}


/* Adds :fluents to the requirements. */
static void require_fluents() {
  if (!requirements->fluents) {
    yywarning("assuming `:fluents' requirement");
    requirements->fluents = true;
  }
}


/* Adds :disjunctive-preconditions to the requirements. */
static void require_disjunction() {
  if (!requirements->disjunctive_preconditions) {
    yywarning("assuming `:disjunctive-preconditions' requirement");
    requirements->disjunctive_preconditions = true;
  }
}


/* Adds :conditional-effects to the requirements. */
static void require_conditional_effects() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
}


/* Returns a simple type with the given name. */
static const Type& make_type(const std::string* name) {
  const Type* t = domain->types().find_type(*name);
  if (t == 0) {
    t = &domain->types().add_type(*name);
    if (name_kind != TYPE_KIND) {
      yywarning("implicit declaration of type `" + *name + "'");
    }
  }
  delete name;
  return *t;
}


/* Returns the union of the given types. */
static Type make_type(const TypeSet& types) {
  return TypeTable::union_type(types);
}


/* Returns a simple term with the given name. */
static Term make_term(const std::string* name) {
  if ((*name)[0] == '?') {
    const Variable* vp = context.find(*name);
    if (vp != 0) {
      delete name;
      return *vp;
    } else {
      Variable v = TermTable::add_variable(TypeTable::OBJECT);
      context.insert(*name, v);
      yyerror("free variable `" + *name + "' used");
      delete name;
      return v;
    }
  } else {
    TermTable& terms = (problem != 0) ? problem->terms() : domain->terms();
    const Object* o = terms.find_object(*name);
    if (o == 0) {
      size_t n = term_parameters.size();
      if (atom_predicate != 0
          && PredicateTable::parameters(*atom_predicate).size() > n) {
        const Type& t = PredicateTable::parameters(*atom_predicate)[n];
        o = &terms.add_object(*name, t);
      } else {
        o = &terms.add_object(*name, TypeTable::OBJECT);
      }
      yywarning("implicit declaration of object `" + *name + "'");
    }
    delete name;
    return *o;
  }
}


/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name) {
  predicate = domain->predicates().find_predicate(*name);
  if (predicate == 0) {
    repeated_predicate = false;
    predicate = &domain->predicates().add_predicate(*name);
  } else {
    repeated_predicate = true;
    yywarning("ignoring repeated declaration of predicate `" + *name + "'");
  }
  delete name;
}


/* Creates a function with the given name. */
static void make_function(const std::string* name) {
  repeated_function = false;
  function = domain->functions().find_function(*name);
  if (function == 0) {
    function = &domain->functions().add_function(*name);
  } else {
    repeated_function = true;
    if (requirements->rewards && *name == "reward") {
      yywarning("ignoring declaration of reserved function `reward'");
    } else if (*name == "total-time" || *name == "goal-achieved") {
      yywarning("ignoring declaration of reserved function `" + *name + "'");
    } else {
      yywarning("ignoring repeated declaration of function `" + *name + "'");
    }
  }
  delete name;
}


/* Creates an action with the given name. */
static void make_action(const std::string* name) {
  context.push_frame();
  action = new ActionSchema(*name);
  delete name;
}


/* Adds the current action to the current domain. */
static void add_action() {
  context.pop_frame();
  if (domain->find_action(action->name()) == 0) {
    domain->add_action(*action);
  } else {
    yywarning("ignoring repeated declaration of action `"
              + action->name() + "'");
    delete action;
  }
  action = 0;
}


/* Prepares for the parsing of a universally quantified effect. */
static void prepare_forall_effect() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  context.push_frame();
  quantified.push_back(Term(0));
}


/* Creates a universally quantified effect. */
static const Effect* make_forall_effect(const Effect& effect) {
  context.pop_frame();
  size_t n = quantified.size() - 1;
  size_t m = n;
  while (quantified[n].variable()) {
    n--;
  }
  VariableList parameters;
  for (size_t i = n + 1; i <= m; i++) {
    parameters.push_back(quantified[i].as_variable());
  }
  quantified.resize(n, Term(0));
  return &QuantifiedEffect::make(parameters, effect);
}


/* Adds an outcome to the given probabilistic effect. */
static void add_outcome(std::vector<std::pair<Rational, const Effect*> >& os,
                        const Rational* p, const Effect& effect) {
  if (!requirements->probabilistic_effects) {
    yywarning("assuming `:probabilistic-effects' requirement");
    requirements->probabilistic_effects = true;
  }
  if (*p < 0 || *p > 1) {
    yyerror("outcome probability needs to be in the interval [0,1]");
  }
  os.push_back(std::make_pair(*p, &effect));
  delete p;
}


/* Creates a probabilistic effect. */
static const Effect*
make_prob_effect(const std::vector<std::pair<Rational, const Effect*> >* os) {
  Rational psum = 0;
  for (size_t i = 0; i < os->size(); i++) {
    psum = psum + (*os)[i].first;
  }
  // Notice that it is OK if psum < 1 because the missing mass represent the
  // NO-OP effect (i.e., nothing happens/changes)
  if (double(psum) - 1 > 1e-6) {
    yyerror("effect outcome probabilities add up to more than 1");
    delete os;
    return &Effect::EMPTY;
  } else {
    const Effect& peff = ProbabilisticEffect::make(*os);
    delete os;
    return &peff;
  }
}


/* Creates an add effect. */
static const Effect* make_add_effect(const Atom& atom) {
  PredicateTable::make_dynamic(atom.predicate());
  return new AddEffect(atom);
}


/* Creates a delete effect. */
static const Effect* make_delete_effect(const Atom& atom) {
  PredicateTable::make_dynamic(atom.predicate());
  return new DeleteEffect(atom);
}


/* Creates an assign update effect. */
static const Effect* make_assign_effect(const Fluent& fluent,
                                        const Expression& expr) {
  if (requirements->rewards && fluent.function() == reward_function) {
    yyerror("only constant reward increments/decrements allowed");
  } else {
    require_fluents();
  }
  effect_fluent = false;
  FunctionTable::make_dynamic(fluent.function());
  return &UpdateEffect::make(*new Assign(fluent, expr));
}


/* Creates a scale-up update effect. */
static const Effect* make_scale_up_effect(const Fluent& fluent,
                                          const Expression& expr) {
  if (requirements->rewards && fluent.function() == reward_function) {
    yyerror("only constant reward increments/decrements allowed");
  } else {
    require_fluents();
  }
  effect_fluent = false;
  FunctionTable::make_dynamic(fluent.function());
  return &UpdateEffect::make(*new ScaleUp(fluent, expr));
}


/* Creates a scale-down update effect. */
static const Effect* make_scale_down_effect(const Fluent& fluent,
                                            const Expression& expr) {
  if (requirements->rewards && fluent.function() == reward_function) {
    yyerror("only constant reward increments/decrements allowed");
  } else {
    require_fluents();
  }
  effect_fluent = false;
  FunctionTable::make_dynamic(fluent.function());
  return &UpdateEffect::make(*new ScaleDown(fluent, expr));
}


/* Creates an increase update effect. */
static const Effect* make_increase_effect(const Fluent& fluent,
                                          const Expression& expr) {
  if (requirements->rewards && fluent.function() == reward_function) {
    if (typeid(expr) != typeid(Value)) {
      yyerror("only constant reward increments/decrements allowed");
    }
  } else {
    require_fluents();
  }
  effect_fluent = false;
  FunctionTable::make_dynamic(fluent.function());
  return &UpdateEffect::make(*new Increase(fluent, expr));
}


/* Creates a decrease update effect. */
static const Effect* make_decrease_effect(const Fluent& fluent,
                                          const Expression& expr) {
  if (requirements->rewards && fluent.function() == reward_function) {
    if (typeid(expr) != typeid(Value)) {
      yyerror("only constant reward increments/decrements allowed");
    }
  } else {
    require_fluents();
  }
  effect_fluent = false;
  FunctionTable::make_dynamic(fluent.function());
  return &UpdateEffect::make(*new Decrease(fluent, expr));
}


/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names,
                      const Type& type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (name_kind == TYPE_KIND) {
      if (*s == TypeTable::OBJECT_NAME) {
        yywarning("ignoring declaration of reserved type `object'");
      } else if (*s == TypeTable::NUMBER_NAME) {
        yywarning("ignoring declaration of reserved type `number'");
      } else {
        const Type* t = domain->types().find_type(*s);
        if (t == 0) {
          t = &domain->types().add_type(*s);
        }
        if (!TypeTable::add_supertype(*t, type)) {
          yyerror("cyclic type hierarchy");
        }
      }
    } else if (name_kind == CONSTANT_KIND) {
      const Object* o = domain->terms().find_object(*s);
      if (o == 0) {
        domain->terms().add_object(*s, type);
      } else {
        TypeSet components;
        TypeTable::components(components, TermTable::type(*o));
        components.insert(type);
        TermTable::set_type(*o, make_type(components));
      }
    } else { /* name_kind == OBJECT_KIND */
      if (domain->terms().find_object(*s) != 0) {
        yywarning("ignoring declaration of object `" + *s
                  + "' previously declared as constant");
      } else {
        const Object* o = problem->terms().find_object(*s);
        if (o == 0) {
          problem->terms().add_object(*s, type);
        } else {
          TypeSet components;
          TypeTable::components(components, TermTable::type(*o));
          components.insert(type);
          TermTable::set_type(*o, make_type(components));
        }
      }
    }
    delete s;
  }
  delete names;
}


/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
                          const Type& type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (predicate != 0) {
      if (!repeated_predicate) {
        PredicateTable::add_parameter(*predicate, type);
      }
    } else if (function != 0) {
      if (!repeated_function) {
        FunctionTable::add_parameter(*function, type);
      }
    } else {
      if (context.shallow_find(*s) != 0) {
        yyerror("repetition of parameter `" + *s + "'");
      } else if (context.find(*s) != 0) {
        yywarning("shadowing parameter `" + *s + "'");
      }
      Variable var = TermTable::add_variable(type);
      context.insert(*s, var);
      if (!quantified.empty()) {
        quantified.push_back(var);
      } else { /* action != 0 */
        action->add_parameter(var);
      }
    }
    delete s;
  }
  delete names;
}


/* Prepares for the parsing of an atomic state formula. */
static void prepare_atom(const std::string* name) {
  atom_predicate = domain->predicates().find_predicate(*name);
  if (atom_predicate == 0) {
    atom_predicate = &domain->predicates().add_predicate(*name);
    undeclared_atom_predicate = true;
    if (problem != 0) {
      yywarning("undeclared predicate `" + *name + "' used");
    } else {
      yywarning("implicit declaration of predicate `" + *name + "'");
    }
  } else {
    undeclared_atom_predicate = false;
  }
  term_parameters.clear();
  delete name;
}


/* Prepares for the parsing of a fluent. */
static void prepare_fluent(const std::string* name) {
  fluent_function = domain->functions().find_function(*name);
  if (fluent_function == 0) {
    fluent_function = &domain->functions().add_function(*name);
    undeclared_fluent_function = true;
    if (problem != 0) {
      yywarning("undeclared function `" + *name + "' used");
    } else {
      yywarning("implicit declaration of function `" + *name + "'");
    }
  } else {
    undeclared_fluent_function = false;
  }
  if (requirements->rewards && *name == "reward") {
    if (!effect_fluent && !metric_fluent) {
      yyerror("reserved function `reward' not allowed here");
    }
  } else if ((*name == "total-time" || *name == "goal-achieved")) {
    if (!metric_fluent) {
      yyerror("reserved function `" + *name + "' not allowed here");
    }
  } else {
    require_fluents();
  }
  term_parameters.clear();
  delete name;
}


/* Adds a term with the given name to the current atomic state formula. */
static void add_term(const std::string* name) {
  const Term& term = make_term(name);
  if (atom_predicate != 0) {
    size_t n = term_parameters.size();
    if (undeclared_atom_predicate) {
      PredicateTable::add_parameter(*atom_predicate, TermTable::type(term));
    } else {
      const TypeList& params = PredicateTable::parameters(*atom_predicate);
      if (params.size() > n
          && !TypeTable::subtype(TermTable::type(term), params[n])) {
        yyerror("type mismatch");
      }
    }
  } else if (fluent_function != 0) {
    size_t n = term_parameters.size();
    if (undeclared_fluent_function) {
      FunctionTable::add_parameter(*fluent_function, TermTable::type(term));
    } else {
      const TypeList& params = FunctionTable::parameters(*fluent_function);
      if (params.size() > n
          && !TypeTable::subtype(TermTable::type(term), params[n])) {
        yyerror("type mismatch");
      }
    }
  }
  term_parameters.push_back(term);
}


/* Creates the atomic formula just parsed. */
static const Atom* make_atom() {
  size_t n = term_parameters.size();
  if (PredicateTable::parameters(*atom_predicate).size() < n) {
    yyerror("too many parameters passed to predicate `"
            + PredicateTable::name(*atom_predicate) + "'");
  } else if (PredicateTable::parameters(*atom_predicate).size() > n) {
    yyerror("too few parameters passed to predicate `"
            + PredicateTable::name(*atom_predicate) + "'");
  }
  // LEGACY(FWT): Ignoring the flag saying if the atom is new (as in the original
  // code) since it does not matter here.
  Atom const& atom = Atom::make(*atom_predicate, term_parameters).first;
  atom_predicate = 0;
  return &atom;
}


/* Creates the fluent just parsed. */
static const Fluent* make_fluent() {
  size_t n = term_parameters.size();
  if (FunctionTable::parameters(*fluent_function).size() < n) {
    yyerror("too many parameters passed to function `"
            + FunctionTable::name(*fluent_function) + "'");
  } else if (FunctionTable::parameters(*fluent_function).size() > n) {
    yyerror("too few parameters passed to function `"
            + FunctionTable::name(*fluent_function) + "'");
  }
  const Fluent& fluent = Fluent::make(*fluent_function, term_parameters);
  fluent_function = 0;
  return &fluent;
}


/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
                                          const Expression* opt_term) {
  if (opt_term != 0) {
    return &Subtraction::make(term, *opt_term);
  } else {
    return &Subtraction::make(*new Value(0), term);
  }
}


/* Creates an atom or fluent for the given name to be used in an
   equality formula. */
static void make_eq_name(const std::string* name) {
  const Function* f = domain->functions().find_function(*name);
  if (f != 0) {
    prepare_fluent(name);
    eq_expr = make_fluent();
  } else {
    /* Assume this is a term. */
    eq_term = make_term(name);
    eq_expr = 0;
  }
}


/* Creates an equality formula. */
static const StateFormula* make_equality() {
  if (!requirements->equality) {
    yywarning("assuming `:equality' requirement");
    requirements->equality = true;
  }
  if (first_eq_expr != 0 && eq_expr != 0) {
    return &EqualTo::make(*first_eq_expr, *eq_expr);
  } else if (first_eq_expr == 0 && eq_expr == 0) {
    if (TypeTable::subtype(TermTable::type(first_eq_term),
                           TermTable::type(eq_term))
        || TypeTable::subtype(TermTable::type(eq_term),
                              TermTable::type(first_eq_term))) {
      return &Equality::make(first_eq_term, eq_term);
    } else {
      return &StateFormula::FALSE;
    }
  } else {
    yyerror("comparison of term and numeric expression");
    return &StateFormula::FALSE;
  }
}


/* Creates a negated formula. */
static const StateFormula* make_negation(const StateFormula& negand) {
  if (typeid(negand) == typeid(Atom)) {
    if (!requirements->negative_preconditions) {
      yywarning("assuming `:negative-preconditions' requirement");
      requirements->negative_preconditions = true;
    }
  } else if (typeid(negand) != typeid(Equality)
             && dynamic_cast<const Comparison*>(&negand) != 0) {
    require_disjunction();
  }
  return &Negation::make(negand);
}


/* Creates an implication. */
static const StateFormula* make_implication(const StateFormula& f1,
                                            const StateFormula& f2) {
  require_disjunction();
  return &(Negation::make(f1) || f2);
}


/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists() {
  if (!requirements->existential_preconditions) {
    yywarning("assuming `:existential-preconditions' requirement");
    requirements->existential_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(Term(0));
}


/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall() {
  if (!requirements->universal_preconditions) {
    yywarning("assuming `:universal-preconditions' requirement");
    requirements->universal_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(Term(0));
}


/* Creates an existentially quantified formula. */
static const StateFormula* make_exists(const StateFormula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (quantified[n].variable()) {
    n--;
  }
  if (n < m) {
    VariableList parameters;
    for (size_t i = n + 1; i <= m; i++) {
      parameters.push_back(quantified[i].as_variable());
    }
    quantified.resize(n, Term(0));
    return &Exists::make(parameters, body);
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Creates a universally quantified formula. */
static const StateFormula* make_forall(const StateFormula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (quantified[n].variable()) {
    n--;
  }
  if (n < m) {
    VariableList parameters;
    for (size_t i = n + 1; i <= m; i++) {
      parameters.push_back(quantified[i].as_variable());
    }
    quantified.resize(n, Term(0));
    return &Forall::make(parameters, body);
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Sets the goal reward for the current problem. */
void set_goal_reward(const Expression& goal_reward) {
  if (!requirements->rewards) {
    yyerror("goal reward only allowed with the `:rewards' requirement");
  } else {
    FunctionTable::make_dynamic(reward_function);
    const Fluent& reward_fluent = Fluent::make(reward_function, TermList());
    problem->set_goal_reward(*new Increase(reward_fluent, goal_reward));
  }
}


/* Sets the default metric for the current problem. */
static void set_default_metric() {
/*  if (requirements->rewards) {*/
/*    problem->set_metric(Fluent::make(reward_function, TermList()));*/
/*  } else if (requirements->probabilistic_effects) {*/
/*    problem->set_metric(Fluent::make(domain->goal_achieved(), TermList()));*/
/*  } else {*/
/*    problem->set_metric(Fluent::make(domain->total_time(), TermList()), true);*/
/*  }*/
}
