/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_TAB_HH_INCLUDED
# define YY_YY_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    DEFINE = 258,
    DOMAIN_TOKEN = 259,
    PROBLEM = 260,
    REQUIREMENTS = 261,
    TYPES = 262,
    CONSTANTS = 263,
    PREDICATES = 264,
    FUNCTIONS = 265,
    STRIPS = 266,
    TYPING = 267,
    NEGATIVE_PRECONDITIONS = 268,
    DISJUNCTIVE_PRECONDITIONS = 269,
    EQUALITY = 270,
    EXISTENTIAL_PRECONDITIONS = 271,
    UNIVERSAL_PRECONDITIONS = 272,
    QUANTIFIED_PRECONDITIONS = 273,
    CONDITIONAL_EFFECTS = 274,
    FLUENTS = 275,
    ADL = 276,
    DURATIVE_ACTIONS = 277,
    DURATION_INEQUALITIES = 278,
    CONTINUOUS_EFFECTS = 279,
    PROBABILISTIC_EFFECTS = 280,
    REWARDS = 281,
    MDP = 282,
    ACTION = 283,
    PARAMETERS = 284,
    PRECONDITION = 285,
    EFFECT = 286,
    PDOMAIN = 287,
    OBJECTS = 288,
    INIT = 289,
    GOAL = 290,
    GOAL_REWARD = 291,
    METRIC = 292,
    TOTAL_TIME = 293,
    GOAL_ACHIEVED = 294,
    WHEN = 295,
    NOT = 296,
    AND = 297,
    OR = 298,
    IMPLY = 299,
    EXISTS = 300,
    FORALL = 301,
    PROBABILISTIC = 302,
    ASSIGN = 303,
    SCALE_UP = 304,
    SCALE_DOWN = 305,
    INCREASE = 306,
    DECREASE = 307,
    MINIMIZE = 308,
    MAXIMIZE = 309,
    NUMBER_TOKEN = 310,
    OBJECT_TOKEN = 311,
    EITHER = 312,
    LE = 313,
    GE = 314,
    NAME = 315,
    VARIABLE = 316,
    NUMBER = 317,
    ILLEGAL_TOKEN = 318
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 270 "parser.yy"

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

#line 136 "parser.tab.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_HH_INCLUDED  */
