/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 20 "parser.yy"

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

#line 305 "parser.tab.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 436 "parser.tab.cc"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_HH_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1139

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  144
/* YYNRULES -- Number of rules.  */
#define YYNRULES  310
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  513

#define YYUNDEFTOK  2
#define YYMAXUTOK   318


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      64,    65,    71,    70,     2,    66,     2,    72,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      68,    67,    69,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   305,   305,   305,   309,   310,   311,   318,   318,   322,
     323,   324,   325,   328,   329,   330,   333,   334,   335,   336,
     337,   338,   339,   342,   343,   344,   345,   346,   349,   350,
     351,   352,   353,   356,   357,   358,   359,   360,   363,   364,
     365,   368,   369,   370,   373,   374,   375,   378,   379,   382,
     385,   388,   389,   392,   393,   394,   396,   398,   399,   401,
     403,   405,   406,   407,   408,   413,   418,   423,   427,   432,
     439,   439,   443,   443,   447,   450,   450,   457,   458,   461,
     461,   465,   466,   467,   470,   471,   474,   474,   477,   477,
     485,   485,   489,   490,   493,   494,   497,   498,   501,   504,
     511,   512,   513,   513,   515,   515,   517,   521,   522,   525,
     530,   534,   537,   538,   539,   539,   541,   541,   543,   543,
     545,   545,   547,   547,   556,   555,   560,   561,   564,   565,
     568,   569,   572,   572,   576,   579,   580,   583,   584,   586,
     590,   595,   599,   600,   603,   604,   607,   608,   612,   615,
     616,   619,   620,   624,   625,   625,   627,   627,   634,   636,
     635,   638,   638,   640,   640,   642,   642,   644,   644,   646,
     647,   648,   648,   649,   650,   650,   652,   652,   656,   657,
     660,   661,   664,   664,   666,   669,   669,   671,   678,   679,
     680,   681,   682,   683,   686,   688,   688,   690,   690,   692,
     692,   694,   694,   696,   696,   698,   699,   702,   703,   706,
     706,   707,   710,   711,   713,   715,   717,   719,   720,   722,
     724,   726,   730,   731,   733,   734,   737,   737,   739,   746,
     747,   748,   751,   752,   755,   756,   757,   757,   761,   762,
     765,   766,   767,   767,   770,   771,   774,   774,   777,   778,
     779,   782,   783,   784,   785,   788,   795,   798,   801,   804,
     807,   810,   813,   816,   819,   822,   825,   828,   831,   834,
     837,   840,   843,   846,   849,   852,   855,   858,   858,   858,
     859,   860,   860,   861,   864,   865,   865,   868,   871,   871,
     871,   872,   872,   872,   873,   873,   873,   873,   873,   873,
     873,   873,   874,   874,   874,   874,   874,   875,   875,   876,
     879
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFINE", "DOMAIN_TOKEN", "PROBLEM",
  "REQUIREMENTS", "TYPES", "CONSTANTS", "PREDICATES", "FUNCTIONS",
  "STRIPS", "TYPING", "NEGATIVE_PRECONDITIONS",
  "DISJUNCTIVE_PRECONDITIONS", "EQUALITY", "EXISTENTIAL_PRECONDITIONS",
  "UNIVERSAL_PRECONDITIONS", "QUANTIFIED_PRECONDITIONS",
  "CONDITIONAL_EFFECTS", "FLUENTS", "ADL", "DURATIVE_ACTIONS",
  "DURATION_INEQUALITIES", "CONTINUOUS_EFFECTS", "PROBABILISTIC_EFFECTS",
  "REWARDS", "MDP", "ACTION", "PARAMETERS", "PRECONDITION", "EFFECT",
  "PDOMAIN", "OBJECTS", "INIT", "GOAL", "GOAL_REWARD", "METRIC",
  "TOTAL_TIME", "GOAL_ACHIEVED", "WHEN", "NOT", "AND", "OR", "IMPLY",
  "EXISTS", "FORALL", "PROBABILISTIC", "ASSIGN", "SCALE_UP", "SCALE_DOWN",
  "INCREASE", "DECREASE", "MINIMIZE", "MAXIMIZE", "NUMBER_TOKEN",
  "OBJECT_TOKEN", "EITHER", "LE", "GE", "NAME", "VARIABLE", "NUMBER",
  "ILLEGAL_TOKEN", "'('", "')'", "'-'", "'='", "'<'", "'>'", "'+'", "'*'",
  "'/'", "$accept", "file", "$@1", "domains_and_problems", "domain_def",
  "$@2", "domain_body", "domain_body2", "domain_body3", "domain_body4",
  "domain_body5", "domain_body6", "domain_body7", "domain_body8",
  "domain_body9", "structure_defs", "structure_def", "require_def",
  "require_keys", "require_key", "types_def", "$@3", "constants_def",
  "$@4", "predicates_def", "functions_def", "$@5", "predicate_decls",
  "predicate_decl", "$@6", "function_decls", "function_decl_seq",
  "function_type_spec", "$@7", "function_decl", "$@8", "action_def", "$@9",
  "parameters", "action_body", "action_body2", "precondition", "effect",
  "eff_formula", "$@10", "$@11", "eff_formulas", "prob_effs",
  "probability", "p_effect", "$@12", "$@13", "$@14", "$@15", "$@16",
  "problem_def", "$@17", "problem_body", "problem_body2", "problem_body3",
  "object_decl", "$@18", "init", "init_elements", "init_element",
  "prob_inits", "simple_init", "one_inits", "one_init", "value",
  "goal_spec", "goal_reward", "metric_spec", "$@19", "$@20", "formula",
  "$@21", "$@22", "$@23", "$@24", "$@25", "$@26", "$@27", "$@28",
  "conjuncts", "disjuncts", "atomic_term_formula", "$@29",
  "atomic_name_formula", "$@30", "f_exp", "term_or_f_exp", "$@31", "$@32",
  "$@33", "$@34", "$@35", "opt_f_exp", "f_head", "$@36", "ground_f_exp",
  "list_ground_f_exp", "opt_ground_f_exp", "ground_f_head", "$@37",
  "terms", "names", "variables", "$@38", "variable_seq", "typed_names",
  "$@39", "name_seq", "type_spec", "$@40", "type", "types",
  "function_type", "define", "domain", "problem", "when", "not", "and",
  "or", "imply", "exists", "forall", "probabilistic", "assign", "scale_up",
  "scale_down", "increase", "decrease", "minimize", "maximize", "number",
  "object", "either", "type_name", "predicate", "function", "name",
  "variable", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,    40,    41,    45,    61,    60,    62,
      43,    42,    47
};
# endif

#define YYPACT_NINF (-381)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -381,    62,  -381,  -381,     0,    85,  -381,  -381,  -381,    32,
      34,  -381,  -381,   995,   995,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,    35,    41,  -381,
      47,    58,   104,    25,    80,  -381,  -381,    88,  -381,    93,
     115,   125,   128,   134,  -381,   995,   722,  -381,  -381,  -381,
    -381,   995,  -381,   121,  -381,   192,  -381,   130,  -381,    27,
    -381,    88,   143,   146,   133,  -381,    88,   143,   147,    38,
    -381,    88,   146,   147,   106,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  1074,  -381,   995,   995,    64,   149,  -381,    67,
    -381,    88,    88,    13,  -381,    88,    88,  -381,    43,  -381,
      88,    88,  -381,  -381,  -381,  -381,  -381,   129,   522,  -381,
     156,  1053,  -381,  -381,   995,   158,   -36,  -381,   185,    88,
      88,    88,   160,  -381,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,   149,  -381,   161,   117,    23,   166,   167,
    -381,  -381,   169,   180,  -381,  -381,   966,   995,   170,   170,
     193,  -381,   170,   903,   937,   182,  -381,   227,  -381,  -381,
    -381,   903,   109,   148,  -381,  -381,   135,  -381,    70,  -381,
    -381,   202,  -381,  -381,  -381,  -381,  -381,   196,   -12,  -381,
     197,  -381,  -381,  -381,   198,   490,  -381,  -381,  -381,  1024,
    -381,  -381,  -381,  -381,  -381,   995,   342,   199,  -381,  -381,
    -381,  -381,  -381,   120,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,   783,  -381,  -381,
     903,  -381,  -381,   903,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,   949,  -381,  -381,   206,  -381,
    -381,  -381,  -381,  -381,   204,   162,  -381,  -381,  -381,  -381,
     211,   755,   755,    22,  -381,  -381,   170,   817,   817,  -381,
     400,  -381,  -381,  -381,   817,   817,   219,   363,  -381,   903,
     221,   222,  -381,   903,  1053,   228,   421,   230,  -381,    51,
     937,   845,   845,   845,   845,   845,  -381,   879,   206,  -381,
     151,  -381,  -381,  -381,  -381,  -381,   267,   755,   232,  -381,
    -381,   233,  -381,  -381,  -381,  -381,  -381,   458,   817,  -381,
    -381,   817,  -381,  -381,  -381,  -381,  -381,   783,   817,   817,
    -381,  -381,  -381,   697,   238,   170,   170,   549,   937,  -381,
    -381,  -381,   170,  -381,   937,  -381,   995,   817,   817,   817,
     817,   817,   995,   225,   961,    55,  -381,   755,   239,   260,
     755,   755,   755,   755,  -381,  -381,  -381,  -381,   817,   817,
     817,   817,  -381,   261,   263,   817,   817,   817,   817,  -381,
     265,   266,   271,  -381,  -381,  -381,   275,   276,  -381,  -381,
    -381,   283,   284,  -381,   285,   286,   287,   288,   289,   290,
     200,  -381,  -381,  -381,  -381,   961,   639,   291,  -381,  -381,
     755,   755,   755,   755,  -381,   817,   817,   817,   817,  -381,
    -381,  -381,   817,   817,   817,   817,   580,  -381,  -381,  -381,
     903,   903,  -381,   937,  -381,  -381,  -381,  -381,  -381,  -381,
     879,  -381,  -381,  -381,  -381,   231,  -381,   292,   294,   295,
     304,   667,  -381,   305,   306,   307,   308,   608,   309,   310,
     311,   312,  -381,   313,   314,   316,   324,   725,   255,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,   322,   223,
    -381,  -381,  -381
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     4,     1,     3,     0,     5,     6,   256,     0,
       0,   257,   258,     0,     0,   288,   289,   290,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   291,   292,   293,   309,     0,     0,     7,
       0,     9,     0,     0,     0,    12,    15,    22,    47,    10,
      13,    16,    17,    18,    49,     0,     0,    70,    72,    77,
      75,     0,     8,     0,    48,     0,    11,     0,    14,     0,
      19,    27,    23,    24,     0,    20,    32,    28,    29,     0,
      21,    37,    33,    34,     0,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,     0,    51,   240,   240,     0,    81,    90,     0,
      25,    40,    38,     0,    26,    43,    41,    30,     0,    31,
      46,    44,    35,    36,   124,    50,    52,     0,   241,   244,
       0,     0,    74,    78,     0,     0,    82,    84,    92,    39,
      42,    45,   153,    71,   246,   242,   245,    73,   277,   278,
     279,   281,   282,   286,   285,   280,   283,   284,    79,    88,
     287,    76,    86,    81,    85,     0,    96,     0,   153,     0,
     127,   129,   153,   153,   131,   150,     0,   240,   234,   234,
       0,    83,   234,     0,     0,     0,    95,    96,    97,   132,
     135,     0,     0,     0,   126,   125,     0,   128,     0,   130,
     275,     0,   247,   248,   249,   243,   310,     0,   235,   238,
       0,   274,    87,   255,     0,     0,    98,   158,   184,     0,
      99,   100,   112,    91,    94,   240,     0,     0,   272,   273,
     156,   154,   276,     0,    80,   236,   239,    89,    93,   260,
     261,   262,   263,   264,   265,   163,   165,     0,   161,   167,
       0,   178,   171,     0,   174,   176,   182,   259,   266,   267,
     268,   269,   270,   271,   104,     0,   107,   102,     0,   114,
     116,   118,   120,   122,     0,     0,   134,   136,   137,   187,
     153,     0,     0,     0,   251,   252,   234,     0,     0,   194,
       0,   159,   205,   206,     0,     0,     0,     0,   180,     0,
       0,     0,   229,     0,     0,     0,     0,     0,   111,     0,
       0,     0,     0,     0,     0,     0,   133,     0,     0,   185,
       0,   149,   151,   219,   221,   212,     0,   222,     0,   217,
     228,     0,   250,   253,   254,   237,   188,     0,     0,   193,
     211,     0,   197,   195,   199,   201,   203,     0,     0,     0,
     169,   170,   179,     0,     0,   234,   234,     0,     0,   113,
     101,   108,   234,   106,     0,   109,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   232,     0,     0,     0,
       0,     0,     0,     0,   226,   223,   157,   155,     0,     0,
       0,     0,   209,     0,     0,     0,     0,     0,     0,   229,
       0,     0,     0,   172,   181,   173,     0,     0,   183,   230,
     231,     0,     0,   110,     0,     0,     0,     0,     0,     0,
       0,   140,   142,   146,   139,     0,     0,     0,   218,   220,
     224,     0,     0,     0,   232,   207,     0,     0,     0,   229,
     164,   166,   207,     0,     0,     0,     0,   160,   162,   168,
       0,     0,   105,     0,   115,   117,   119,   121,   123,   138,
       0,   144,   141,   186,   233,   153,   225,     0,     0,     0,
       0,     0,   208,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   204,     0,     0,     0,     0,     0,     0,   152,
     214,   213,   215,   216,   227,   190,   189,   191,   192,   210,
     198,   196,   200,   202,   175,   177,   103,   148,     0,     0,
     143,   145,   147
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -381,  -381,  -381,  -381,  -381,  -381,  -381,   339,   343,  -381,
    -381,  -381,   317,   318,   325,    21,   -31,   259,  -381,   319,
    -381,  -381,    49,  -381,   -30,    83,  -381,  -381,  -381,  -381,
     246,  -381,  -381,  -381,   274,  -381,  -381,  -381,  -381,  -381,
     224,  -381,  -381,  -297,  -381,  -381,  -381,  -381,  -294,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -381,   244,   242,
    -381,  -381,  -381,  -381,  -381,  -381,   -10,  -381,   -65,  -381,
     256,  -381,  -272,  -381,  -381,  -185,  -381,  -381,  -381,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -180,  -381,   208,  -381,
      -6,    89,  -381,  -381,  -381,  -381,  -381,    -7,   -75,  -381,
     107,  -264,  -381,  -307,  -381,  -380,    31,  -165,  -381,  -381,
     -94,  -381,  -381,   251,  -381,  -381,  -381,  -381,  -381,  -381,
    -381,  -381,   248,  -212,  -381,  -381,  -381,   249,   194,  -381,
    -381,  -381,  -381,  -381,  -381,  -381,  -381,  -213,  -381,  -164,
    -129,  -131,   -13,  -203
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     4,     6,    41,    44,    45,    46,    70,
      75,    80,   110,   114,   119,    47,    48,    49,   102,   103,
      50,   104,    51,   105,    52,    53,   107,   106,   133,   178,
     135,   136,   163,   180,   137,   179,    54,   138,   166,   185,
     186,   187,   188,   220,   307,   303,   306,   309,   310,   221,
     311,   312,   313,   314,   315,     7,   142,   169,   170,   171,
     172,   225,   173,   226,   277,   375,   421,   487,   422,   508,
     174,   321,   175,   282,   281,   216,   347,   294,   287,   288,
     295,   298,   300,   301,   297,   353,   217,   302,   423,   376,
     472,   291,   396,   395,   397,   398,   399,   473,   339,   439,
     327,   328,   467,   329,   434,   357,   426,   207,   286,   208,
     127,   177,   128,   145,   176,   202,   283,   212,     9,    13,
      14,   264,   250,   251,   252,   253,   254,   255,   268,   269,
     270,   271,   272,   273,   230,   231,   213,   203,   233,   157,
     218,   340,   160,   209
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      37,    38,   158,   159,   222,   236,   227,   266,   322,   361,
     373,   130,   204,   365,   210,   364,    64,   214,   331,   446,
     284,    72,    59,    83,   374,   148,   149,   150,   134,    56,
     162,    56,    57,    58,    59,    60,    59,    60,    11,    12,
      64,    61,    84,   116,   293,    64,    58,    59,   108,   206,
      64,    58,   116,    61,   144,    61,   189,   190,   191,   477,
     192,   411,     3,   385,     5,   296,    61,   413,   299,   285,
     333,    61,    71,    76,    81,   151,   152,    60,   200,   155,
      64,   425,   156,   205,    64,   305,   256,   332,     8,    64,
     256,   129,   129,   111,   115,    61,    10,   279,   111,   120,
      39,    77,    82,   115,   120,   191,    40,   192,    64,    64,
      64,    42,   352,   308,   354,   146,   363,   308,   358,   334,
     424,   335,    43,   148,   149,   150,   222,   121,   131,   132,
     222,   274,   121,   139,    73,    78,    55,   140,    58,    59,
      60,    58,   141,    60,   293,    62,   319,   183,   184,    61,
     330,   330,    63,   486,   410,   112,   485,    65,    61,   346,
     112,    61,   228,   229,   129,   148,   149,   150,   404,   190,
     191,   124,   192,   151,   152,   256,   200,   155,   222,    67,
     156,   189,   190,   191,   222,   192,   330,   377,   192,    69,
     406,   407,    74,   489,   143,   384,   330,   412,    79,    57,
      58,    59,    60,   148,   149,   150,   392,   109,   461,   258,
     113,   118,   129,   134,   165,   151,   152,   153,   154,   155,
      61,   147,   156,   161,   167,   182,   148,   149,   150,   317,
     193,   206,   195,   196,   292,   392,   367,   368,   369,   370,
     371,   384,   240,   410,   198,   279,   330,   223,   211,   330,
     330,   330,   330,   151,   152,   153,   154,   155,   184,   232,
     156,   234,   237,   238,   280,   483,   484,   460,   308,   316,
      15,    16,    17,   222,   410,   320,   151,   152,   153,   154,
     155,   338,   341,   156,   350,   355,   356,   419,   348,   349,
     460,   319,   192,   359,   362,   488,   279,   386,   387,   330,
     330,   330,   330,   405,   428,   378,   379,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,   429,   440,    36,   441,   330,
     447,   448,   393,   380,   292,   394,   449,   381,   382,   383,
     450,   451,   401,   402,   409,   148,   149,   150,   452,   453,
     454,   455,   456,   457,   458,   459,   465,   490,   279,   491,
     492,   414,   415,   416,   417,   418,   148,   149,   150,   493,
     495,   496,   497,   498,   500,   501,   502,   503,   504,   505,
     319,   506,   435,   436,   437,   438,   507,   512,    66,   442,
     443,   444,   445,    68,   117,   151,   152,   153,   154,   155,
     122,   168,   156,    15,    16,    17,   275,   276,   123,   181,
     164,   224,   194,   464,   197,   462,   151,   152,   153,   154,
     155,   126,   511,   156,   148,   149,   150,   215,   351,   199,
     474,   475,   476,   409,   278,   478,   400,   479,   480,   481,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,   464,   235,
      36,    15,    16,    17,   409,   471,   342,   265,   267,   318,
     343,   344,   345,     0,   151,   152,   153,   154,   155,     0,
       0,   156,     0,     0,   427,   219,   360,   430,   431,   432,
     433,     0,     0,   148,   149,   150,     0,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,     0,    36,     0,
       0,     0,     0,     0,   388,    15,    16,    17,   389,   390,
     391,   239,   240,   241,   242,   243,   244,   466,   468,   469,
     470,     0,     0,   151,   152,   153,   154,   155,   245,   246,
     156,     0,    15,    16,    17,     0,     0,   247,   248,   249,
       0,     0,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
       0,     0,    36,    15,    16,    17,     0,     0,   144,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
     206,    15,    16,    17,   408,     0,     0,     0,     0,     0,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,     0,     0,
      36,   206,    15,    16,    17,   482,     0,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,     0,    36,   206,
      15,    16,    17,   499,     0,     0,     0,     0,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
     148,   149,   150,     0,   463,     0,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,   148,   149,
     150,     0,   494,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     151,   152,   153,   154,   155,     0,     0,   156,    15,    16,
      17,   215,   403,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   151,   152,
     153,   154,   155,     0,     0,   156,    15,    16,    17,   509,
     510,     0,     0,   323,   324,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,     0,    36,     0,   325,     0,   326,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,     0,     0,    36,   206,   289,     0,   290,    15,    16,
      17,     0,     0,     0,     0,     0,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,     0,   336,
       0,   337,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,     0,    36,   148,   149,   150,   366,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
     148,   149,   150,   372,     0,     0,     0,     0,     0,     0,
       0,     0,   148,   149,   150,     0,   151,   152,   153,   154,
     155,     0,     0,   156,   148,   149,   150,   215,     0,   148,
     149,   150,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     151,   152,   153,   154,   155,     0,     0,   156,    15,    16,
      17,   219,   151,   152,   153,   154,   155,     0,     0,   156,
       0,     0,     0,   304,   151,   152,   153,   154,   155,   151,
     152,   156,   200,   155,     0,   420,   156,   148,   149,   150,
     201,     0,     0,     0,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,     0,    36,   148,   149,   150,     0,
       0,     0,     0,     0,   257,   239,   240,     0,     0,     0,
     244,   258,   259,   260,   261,   262,   263,   151,   152,   153,
     154,   155,     0,     0,   156,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,     0,     0,     0,     0,   151,   152,   153,   154,
     155,     0,     0,   156,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   125
};

static const yytype_int16 yycheck[] =
{
      13,    14,   131,   134,   184,   208,   191,   219,   280,   306,
     317,   105,   176,   310,   179,   309,    47,   182,   282,   399,
     233,    51,     9,    53,   318,     3,     4,     5,    64,     6,
      66,     6,     7,     8,     9,    10,     9,    10,     4,     5,
      71,    28,    55,    73,   247,    76,     8,     9,    61,    61,
      81,     8,    82,    28,    66,    28,    33,    34,    35,   439,
      37,   358,     0,   327,    64,   250,    28,   364,   253,   233,
     283,    28,    51,    52,    53,    53,    54,    10,    56,    57,
     111,   375,    60,   177,   115,   265,   215,    65,     3,   120,
     219,   104,   105,    72,    73,    28,    64,   226,    77,    78,
      65,    52,    53,    82,    83,    35,    65,    37,   139,   140,
     141,    64,   297,    62,   299,   128,    65,    62,   303,   283,
      65,   286,    64,     3,     4,     5,   306,    78,    64,    65,
     310,   225,    83,   112,    51,    52,    32,   116,     8,     9,
      10,     8,   121,    10,   347,    65,   275,    30,    31,    28,
     281,   282,    64,   460,   357,    72,   453,    64,    28,   290,
      77,    28,    53,    54,   177,     3,     4,     5,   353,    34,
      35,    65,    37,    53,    54,   304,    56,    57,   358,    64,
      60,    33,    34,    35,   364,    37,   317,    36,    37,    64,
     355,   356,    64,   465,    65,   326,   327,   362,    64,     7,
       8,     9,    10,     3,     4,     5,   337,    64,   420,    47,
      64,    64,   225,    64,    29,    53,    54,    55,    56,    57,
      28,    65,    60,    65,    64,    64,     3,     4,     5,    67,
      64,    61,    65,    64,   247,   366,   311,   312,   313,   314,
     315,   372,    42,   446,    64,   374,   377,    65,    55,   380,
     381,   382,   383,    53,    54,    55,    56,    57,    31,    57,
      60,    65,    65,    65,    65,   450,   451,    67,    62,    65,
       3,     4,     5,   453,   477,    64,    53,    54,    55,    56,
      57,   287,   288,    60,    65,    64,    64,    62,   294,   295,
      67,   420,    37,    65,    64,    64,   425,    65,    65,   430,
     431,   432,   433,    65,    65,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    65,    65,    60,    65,   460,
      65,    65,   338,    66,   347,   341,    65,    70,    71,    72,
      65,    65,   348,   349,   357,     3,     4,     5,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,   487,    65,
      65,   367,   368,   369,   370,   371,     3,     4,     5,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
     509,    65,   388,   389,   390,   391,    62,    65,    49,   395,
     396,   397,   398,    50,    77,    53,    54,    55,    56,    57,
      82,   142,    60,     3,     4,     5,    64,    65,    83,   163,
     136,   187,   168,   426,   172,   425,    53,    54,    55,    56,
      57,   102,   487,    60,     3,     4,     5,    64,    65,   173,
     436,   437,   438,   446,   226,   442,   347,   443,   444,   445,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   471,   208,
      60,     3,     4,     5,   477,   434,    66,   219,   219,   275,
      70,    71,    72,    -1,    53,    54,    55,    56,    57,    -1,
      -1,    60,    -1,    -1,   377,    64,    65,   380,   381,   382,
     383,    -1,    -1,     3,     4,     5,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    66,     3,     4,     5,    70,    71,
      72,    41,    42,    43,    44,    45,    46,   430,   431,   432,
     433,    -1,    -1,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,     3,     4,     5,    -1,    -1,    67,    68,    69,
      -1,    -1,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    -1,    60,     3,     4,     5,    -1,    -1,    66,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    -1,    60,
      61,     3,     4,     5,    65,    -1,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    -1,
      60,    61,     3,     4,     5,    65,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    -1,    60,    61,
       3,     4,     5,    65,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    -1,    60,
       3,     4,     5,    -1,    65,    -1,    -1,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    -1,    60,     3,     4,
       5,    -1,    65,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      53,    54,    55,    56,    57,    -1,    -1,    60,     3,     4,
       5,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,
      55,    56,    57,    -1,    -1,    60,     3,     4,     5,    64,
      65,    -1,    -1,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    -1,    60,    -1,    62,    -1,    64,
       3,     4,     5,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    -1,    60,    61,    62,    -1,    64,     3,     4,
       5,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    -1,    60,    -1,    62,
      -1,    64,     3,     4,     5,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    -1,    60,     3,     4,     5,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    -1,    60,
       3,     4,     5,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,    53,    54,    55,    56,
      57,    -1,    -1,    60,     3,     4,     5,    64,    -1,     3,
       4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,    54,    55,    56,    57,    -1,    -1,    60,     3,     4,
       5,    64,    53,    54,    55,    56,    57,    -1,    -1,    60,
      -1,    -1,    -1,    64,    53,    54,    55,    56,    57,    53,
      54,    60,    56,    57,    -1,    64,    60,     3,     4,     5,
      64,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    -1,    60,     3,     4,     5,    -1,
      -1,    -1,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    -1,    60,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    -1,    -1,    -1,    -1,    53,    54,    55,    56,
      57,    -1,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,    75,     0,    76,    64,    77,   128,     3,   191,
      64,     4,     5,   192,   193,     3,     4,     5,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    60,   215,   215,    65,
      65,    78,    64,    64,    79,    80,    81,    88,    89,    90,
      93,    95,    97,    98,   109,    32,     6,     7,     8,     9,
      10,    28,    65,    64,    89,    64,    80,    64,    81,    64,
      82,    88,    97,    98,    64,    83,    88,    95,    98,    64,
      84,    88,    95,    97,   215,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    91,    92,    94,    96,   100,    99,   215,    64,
      85,    88,    98,    64,    86,    88,    97,    85,    64,    87,
      88,    95,    86,    87,    65,    65,    92,   183,   185,   215,
     183,    64,    65,   101,    64,   103,   104,   107,   110,    88,
      88,    88,   129,    65,    66,   186,   215,    65,     3,     4,
       5,    53,    54,    55,    56,    57,    60,   212,   213,   214,
     215,    65,    66,   105,   107,    29,   111,    64,    90,   130,
     131,   132,   133,   135,   143,   145,   187,   184,   102,   108,
     106,   103,    64,    30,    31,   112,   113,   114,   115,    33,
      34,    35,    37,    64,   131,    65,    64,   132,    64,   143,
      56,    64,   188,   210,   212,   183,    61,   180,   182,   216,
     180,    55,   190,   209,   180,    64,   148,   159,   213,    64,
     116,   122,   159,    65,   113,   134,   136,   148,    53,    54,
     207,   208,    57,   211,    65,   186,   216,    65,    65,    41,
      42,    43,    44,    45,    46,    58,    59,    67,    68,    69,
     195,   196,   197,   198,   199,   200,   213,    40,    47,    48,
      49,    50,    51,    52,   194,   195,   196,   200,   201,   202,
     203,   204,   205,   206,   183,    64,    65,   137,   161,   213,
      65,   147,   146,   189,   210,   212,   181,   151,   152,    62,
      64,   164,   215,   216,   150,   153,   148,   157,   154,   148,
     155,   156,   160,   118,    64,   159,   119,   117,    62,   120,
     121,   123,   124,   125,   126,   127,    65,    67,   201,   213,
      64,   144,   145,    38,    39,    62,    64,   173,   174,   176,
     214,   174,    65,   210,   212,   180,    62,    64,   163,   171,
     214,   163,    66,    70,    71,    72,   214,   149,   163,   163,
      65,    65,   148,   158,   148,    64,    64,   178,   148,    65,
      65,   116,    64,    65,   121,   116,    64,   171,   171,   171,
     171,   171,    64,   176,   121,   138,   162,    36,    38,    39,
      66,    70,    71,    72,   214,   174,    65,    65,    66,    70,
      71,    72,   214,   163,   163,   166,   165,   167,   168,   169,
     164,   163,   163,    65,   148,    65,   180,   180,    65,   215,
     216,   116,   180,   116,   163,   163,   163,   163,   163,    62,
      64,   139,   141,   161,    65,   121,   179,   173,    65,    65,
     173,   173,   173,   173,   177,   163,   163,   163,   163,   172,
      65,    65,   163,   163,   163,   163,   178,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      67,   196,   139,    65,   215,    65,   173,   175,   173,   173,
     173,   179,   163,   170,   163,   163,   163,   178,   170,   163,
     163,   163,    65,   148,   148,   116,   176,   140,    64,   145,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    62,   142,    64,
      65,   141,    65
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    75,    74,    76,    76,    76,    78,    77,    79,
      79,    79,    79,    80,    80,    80,    81,    81,    81,    81,
      81,    81,    81,    82,    82,    82,    82,    82,    83,    83,
      83,    83,    83,    84,    84,    84,    84,    84,    85,    85,
      85,    86,    86,    86,    87,    87,    87,    88,    88,    89,
      90,    91,    91,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      94,    93,    96,    95,    97,    99,    98,   100,   100,   102,
     101,   103,   103,   103,   104,   104,   106,   105,   108,   107,
     110,   109,   111,   111,   112,   112,   113,   113,   114,   115,
     116,   116,   117,   116,   118,   116,   116,   119,   119,   120,
     120,   121,   122,   122,   123,   122,   124,   122,   125,   122,
     126,   122,   127,   122,   129,   128,   130,   130,   131,   131,
     132,   132,   134,   133,   135,   136,   136,   137,   137,   137,
     138,   138,   139,   139,   140,   140,   141,   141,   142,   143,
     143,   144,   144,   145,   146,   145,   147,   145,   148,   149,
     148,   150,   148,   151,   148,   152,   148,   153,   148,   148,
     148,   154,   148,   148,   155,   148,   156,   148,   157,   157,
     158,   158,   160,   159,   159,   162,   161,   161,   163,   163,
     163,   163,   163,   163,   164,   165,   164,   166,   164,   167,
     164,   168,   164,   169,   164,   164,   164,   170,   170,   172,
     171,   171,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   174,   174,   175,   175,   177,   176,   176,   178,
     178,   178,   179,   179,   180,   180,   181,   180,   182,   182,
     183,   183,   184,   183,   185,   185,   187,   186,   188,   188,
     188,   189,   189,   189,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   212,   212,
     212,   212,   212,   212,   213,   213,   213,   214,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     216
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     2,     0,     9,     0,
       1,     2,     1,     1,     2,     1,     1,     1,     1,     2,
       2,     2,     1,     1,     1,     2,     2,     1,     1,     1,
       2,     2,     1,     1,     1,     2,     2,     1,     1,     2,
       1,     1,     2,     1,     1,     2,     1,     1,     2,     1,
       4,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     5,     0,     5,     4,     0,     5,     0,     2,     0,
       5,     0,     1,     3,     1,     2,     0,     3,     0,     5,
       0,     7,     0,     4,     2,     1,     0,     1,     2,     2,
       1,     4,     0,     8,     0,     6,     4,     0,     2,     2,
       3,     1,     1,     4,     0,     6,     0,     6,     0,     6,
       0,     6,     0,     6,     0,    13,     2,     1,     2,     1,
       2,     1,     0,     5,     4,     0,     2,     1,     5,     4,
       2,     3,     1,     4,     0,     2,     1,     5,     1,     5,
       1,     1,     5,     0,     0,     6,     0,     6,     1,     0,
       6,     0,     6,     0,     6,     0,     6,     0,     6,     4,
       4,     0,     5,     5,     0,     8,     0,     8,     0,     2,
       0,     2,     0,     5,     1,     0,     5,     1,     1,     5,
       5,     5,     5,     1,     1,     0,     6,     0,     6,     0,
       6,     0,     6,     0,     5,     1,     1,     0,     1,     0,
       5,     1,     1,     5,     5,     5,     5,     1,     3,     1,
       3,     1,     1,     2,     0,     1,     0,     5,     1,     0,
       2,     2,     0,     2,     0,     1,     0,     4,     1,     2,
       0,     1,     0,     4,     1,     2,     0,     3,     1,     1,
       4,     1,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 305 "parser.yy"
       { success = true; line_number = 1; }
#line 2150 "parser.tab.cc"
    break;

  case 3:
#line 306 "parser.yy"
         { if (!success) YYERROR; }
#line 2156 "parser.tab.cc"
    break;

  case 7:
#line 318 "parser.yy"
                                            { make_domain((yyvsp[-1].str)); }
#line 2162 "parser.tab.cc"
    break;

  case 53:
#line 392 "parser.yy"
                     { requirements->strips = true; }
#line 2168 "parser.tab.cc"
    break;

  case 54:
#line 393 "parser.yy"
                     { requirements->typing = true; }
#line 2174 "parser.tab.cc"
    break;

  case 55:
#line 395 "parser.yy"
                { requirements->negative_preconditions = true; }
#line 2180 "parser.tab.cc"
    break;

  case 56:
#line 397 "parser.yy"
                { requirements->disjunctive_preconditions = true; }
#line 2186 "parser.tab.cc"
    break;

  case 57:
#line 398 "parser.yy"
                       { requirements->equality = true; }
#line 2192 "parser.tab.cc"
    break;

  case 58:
#line 400 "parser.yy"
                { requirements->existential_preconditions = true; }
#line 2198 "parser.tab.cc"
    break;

  case 59:
#line 402 "parser.yy"
                { requirements->universal_preconditions = true; }
#line 2204 "parser.tab.cc"
    break;

  case 60:
#line 404 "parser.yy"
                { requirements->quantified_preconditions(); }
#line 2210 "parser.tab.cc"
    break;

  case 61:
#line 405 "parser.yy"
                                  { requirements->conditional_effects = true; }
#line 2216 "parser.tab.cc"
    break;

  case 62:
#line 406 "parser.yy"
                      { requirements->fluents = true; }
#line 2222 "parser.tab.cc"
    break;

  case 63:
#line 407 "parser.yy"
                  { requirements->adl(); }
#line 2228 "parser.tab.cc"
    break;

  case 64:
#line 409 "parser.yy"
                {
                  throw std::runtime_error("`:durative-actions'"
                                           " not supported");
                }
#line 2237 "parser.tab.cc"
    break;

  case 65:
#line 414 "parser.yy"
                {
                  throw std::runtime_error("`:duration-inequalities'"
                                           " not supported");
                }
#line 2246 "parser.tab.cc"
    break;

  case 66:
#line 419 "parser.yy"
                {
                  throw std::runtime_error("`:continuous-effects'"
                                           " not supported");
                }
#line 2255 "parser.tab.cc"
    break;

  case 67:
#line 424 "parser.yy"
                {
                  requirements->probabilistic_effects = true;
                }
#line 2263 "parser.tab.cc"
    break;

  case 68:
#line 428 "parser.yy"
                {
                  requirements->rewards = true;
                  reward_function = domain->functions().add_function("reward");
                }
#line 2272 "parser.tab.cc"
    break;

  case 69:
#line 433 "parser.yy"
                {
                  requirements->mdp();
                  reward_function = domain->functions().add_function("reward");
                }
#line 2281 "parser.tab.cc"
    break;

  case 70:
#line 439 "parser.yy"
                      { require_typing(); name_kind = TYPE_KIND; }
#line 2287 "parser.tab.cc"
    break;

  case 71:
#line 440 "parser.yy"
                              { name_kind = VOID_KIND; }
#line 2293 "parser.tab.cc"
    break;

  case 72:
#line 443 "parser.yy"
                              { name_kind = CONSTANT_KIND; }
#line 2299 "parser.tab.cc"
    break;

  case 73:
#line 444 "parser.yy"
                  { name_kind = VOID_KIND; }
#line 2305 "parser.tab.cc"
    break;

  case 75:
#line 450 "parser.yy"
                              { require_fluents(); }
#line 2311 "parser.tab.cc"
    break;

  case 79:
#line 461 "parser.yy"
                               { make_predicate((yyvsp[0].str)); }
#line 2317 "parser.tab.cc"
    break;

  case 80:
#line 462 "parser.yy"
                   { predicate = 0; }
#line 2323 "parser.tab.cc"
    break;

  case 86:
#line 474 "parser.yy"
                         { require_typing(); }
#line 2329 "parser.tab.cc"
    break;

  case 88:
#line 477 "parser.yy"
                             { make_function((yyvsp[0].str)); }
#line 2335 "parser.tab.cc"
    break;

  case 89:
#line 478 "parser.yy"
                  { function = 0; }
#line 2341 "parser.tab.cc"
    break;

  case 90:
#line 485 "parser.yy"
                             { make_action((yyvsp[0].str)); }
#line 2347 "parser.tab.cc"
    break;

  case 91:
#line 486 "parser.yy"
                                          { add_action(); }
#line 2353 "parser.tab.cc"
    break;

  case 98:
#line 501 "parser.yy"
                                    { action->set_precondition(*(yyvsp[0].formula)); }
#line 2359 "parser.tab.cc"
    break;

  case 99:
#line 504 "parser.yy"
                            { action->set_effect(*(yyvsp[0].effect)); }
#line 2365 "parser.tab.cc"
    break;

  case 101:
#line 512 "parser.yy"
                                       { (yyval.effect) = (yyvsp[-1].effect); }
#line 2371 "parser.tab.cc"
    break;

  case 102:
#line 513 "parser.yy"
                         { prepare_forall_effect(); }
#line 2377 "parser.tab.cc"
    break;

  case 103:
#line 514 "parser.yy"
                                { (yyval.effect) = make_forall_effect(*(yyvsp[-1].effect)); }
#line 2383 "parser.tab.cc"
    break;

  case 104:
#line 515 "parser.yy"
                       { require_conditional_effects(); }
#line 2389 "parser.tab.cc"
    break;

  case 105:
#line 516 "parser.yy"
                                { (yyval.effect) = &ConditionalEffect::make(*(yyvsp[-2].formula), *(yyvsp[-1].effect)); }
#line 2395 "parser.tab.cc"
    break;

  case 106:
#line 518 "parser.yy"
                { (yyval.effect) = make_prob_effect((yyvsp[-1].outcomes)); }
#line 2401 "parser.tab.cc"
    break;

  case 107:
#line 521 "parser.yy"
                           { (yyval.effect) = &Effect::EMPTY; }
#line 2407 "parser.tab.cc"
    break;

  case 108:
#line 522 "parser.yy"
                                        { (yyval.effect) = &(*(yyvsp[-1].effect) && *(yyvsp[0].effect)); }
#line 2413 "parser.tab.cc"
    break;

  case 109:
#line 526 "parser.yy"
              {
                (yyval.outcomes) = new std::vector<std::pair<Rational, const Effect*> >();
                add_outcome(*(yyval.outcomes), (yyvsp[-1].num), *(yyvsp[0].effect));
              }
#line 2422 "parser.tab.cc"
    break;

  case 110:
#line 531 "parser.yy"
              { (yyval.outcomes) = (yyvsp[-2].outcomes); add_outcome(*(yyval.outcomes), (yyvsp[-1].num), *(yyvsp[0].effect)); }
#line 2428 "parser.tab.cc"
    break;

  case 112:
#line 537 "parser.yy"
                               { (yyval.effect) = make_add_effect(*(yyvsp[0].atom)); }
#line 2434 "parser.tab.cc"
    break;

  case 113:
#line 538 "parser.yy"
                                           { (yyval.effect) = make_delete_effect(*(yyvsp[-1].atom)); }
#line 2440 "parser.tab.cc"
    break;

  case 114:
#line 539 "parser.yy"
                      { effect_fluent = true; }
#line 2446 "parser.tab.cc"
    break;

  case 115:
#line 540 "parser.yy"
             { (yyval.effect) = make_assign_effect(*(yyvsp[-2].fluent), *(yyvsp[-1].expr)); }
#line 2452 "parser.tab.cc"
    break;

  case 116:
#line 541 "parser.yy"
                        { effect_fluent = true; }
#line 2458 "parser.tab.cc"
    break;

  case 117:
#line 542 "parser.yy"
             { (yyval.effect) = make_scale_up_effect(*(yyvsp[-2].fluent), *(yyvsp[-1].expr)); }
#line 2464 "parser.tab.cc"
    break;

  case 118:
#line 543 "parser.yy"
                          { effect_fluent = true; }
#line 2470 "parser.tab.cc"
    break;

  case 119:
#line 544 "parser.yy"
             { (yyval.effect) = make_scale_down_effect(*(yyvsp[-2].fluent), *(yyvsp[-1].expr)); }
#line 2476 "parser.tab.cc"
    break;

  case 120:
#line 545 "parser.yy"
                        { effect_fluent = true; }
#line 2482 "parser.tab.cc"
    break;

  case 121:
#line 546 "parser.yy"
             { (yyval.effect) = make_increase_effect(*(yyvsp[-2].fluent), *(yyvsp[-1].expr)); }
#line 2488 "parser.tab.cc"
    break;

  case 122:
#line 547 "parser.yy"
                        { effect_fluent = true; }
#line 2494 "parser.tab.cc"
    break;

  case 123:
#line 548 "parser.yy"
             { (yyval.effect) = make_decrease_effect(*(yyvsp[-2].fluent), *(yyvsp[-1].expr)); }
#line 2500 "parser.tab.cc"
    break;

  case 124:
#line 556 "parser.yy"
                { make_problem((yyvsp[-5].str), (yyvsp[-1].str)); }
#line 2506 "parser.tab.cc"
    break;

  case 125:
#line 557 "parser.yy"
                { problem->ground(); delete requirements; }
#line 2512 "parser.tab.cc"
    break;

  case 132:
#line 572 "parser.yy"
                          { name_kind = OBJECT_KIND; }
#line 2518 "parser.tab.cc"
    break;

  case 133:
#line 573 "parser.yy"
                { name_kind = VOID_KIND; }
#line 2524 "parser.tab.cc"
    break;

  case 137:
#line 583 "parser.yy"
                                   { problem->add_init_atom(*(yyvsp[0].atom)); }
#line 2530 "parser.tab.cc"
    break;

  case 138:
#line 585 "parser.yy"
                 { problem->add_init_value(*(yyvsp[-2].fluent), *(yyvsp[-1].num)); delete (yyvsp[-1].num); }
#line 2536 "parser.tab.cc"
    break;

  case 139:
#line 587 "parser.yy"
                 { problem->add_init_effect(*make_prob_effect((yyvsp[-1].outcomes))); }
#line 2542 "parser.tab.cc"
    break;

  case 140:
#line 591 "parser.yy"
               {
                 (yyval.outcomes) = new std::vector<std::pair<Rational, const Effect*> >();
                 add_outcome(*(yyval.outcomes), (yyvsp[-1].num), *(yyvsp[0].effect));
               }
#line 2551 "parser.tab.cc"
    break;

  case 141:
#line 596 "parser.yy"
               { (yyval.outcomes) = (yyvsp[-2].outcomes); add_outcome(*(yyval.outcomes), (yyvsp[-1].num), *(yyvsp[0].effect)); }
#line 2557 "parser.tab.cc"
    break;

  case 143:
#line 600 "parser.yy"
                                    { (yyval.effect) = (yyvsp[-1].effect); }
#line 2563 "parser.tab.cc"
    break;

  case 144:
#line 603 "parser.yy"
                        { (yyval.effect) = &Effect::EMPTY; }
#line 2569 "parser.tab.cc"
    break;

  case 145:
#line 604 "parser.yy"
                               { (yyval.effect) = &(*(yyvsp[-1].effect) && *(yyvsp[0].effect)); }
#line 2575 "parser.tab.cc"
    break;

  case 146:
#line 607 "parser.yy"
                               { (yyval.effect) = make_add_effect(*(yyvsp[0].atom)); }
#line 2581 "parser.tab.cc"
    break;

  case 147:
#line 609 "parser.yy"
             { (yyval.effect) = make_assign_effect(*(yyvsp[-2].fluent), *(yyvsp[-1].expr)); }
#line 2587 "parser.tab.cc"
    break;

  case 148:
#line 612 "parser.yy"
               { (yyval.expr) = new Value(*(yyvsp[0].num)); delete (yyvsp[0].num); }
#line 2593 "parser.tab.cc"
    break;

  case 149:
#line 615 "parser.yy"
                                             { problem->set_goal(*(yyvsp[-2].formula)); }
#line 2599 "parser.tab.cc"
    break;

  case 152:
#line 621 "parser.yy"
                { set_goal_reward(*(yyvsp[-2].expr)); }
#line 2605 "parser.tab.cc"
    break;

  case 153:
#line 624 "parser.yy"
                          { set_default_metric(); }
#line 2611 "parser.tab.cc"
    break;

  case 154:
#line 625 "parser.yy"
                                  { metric_fluent = true; }
#line 2617 "parser.tab.cc"
    break;

  case 155:
#line 626 "parser.yy"
                { problem->set_metrics(*(yyvsp[-1].vec_expr), false); metric_fluent = false; }
#line 2623 "parser.tab.cc"
    break;

  case 156:
#line 627 "parser.yy"
                                  { metric_fluent = true; }
#line 2629 "parser.tab.cc"
    break;

  case 157:
#line 628 "parser.yy"
                { problem->set_metrics(*(yyvsp[-1].vec_expr), true); metric_fluent = false; }
#line 2635 "parser.tab.cc"
    break;

  case 158:
#line 634 "parser.yy"
                              { (yyval.formula) = (yyvsp[0].atom); }
#line 2641 "parser.tab.cc"
    break;

  case 159:
#line 636 "parser.yy"
            { first_eq_term = eq_term; first_eq_expr = eq_expr; }
#line 2647 "parser.tab.cc"
    break;

  case 160:
#line 637 "parser.yy"
                              { (yyval.formula) = make_equality(); }
#line 2653 "parser.tab.cc"
    break;

  case 161:
#line 638 "parser.yy"
                  { require_fluents(); }
#line 2659 "parser.tab.cc"
    break;

  case 162:
#line 639 "parser.yy"
            { (yyval.formula) = &LessThan::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2665 "parser.tab.cc"
    break;

  case 163:
#line 640 "parser.yy"
                 { require_fluents(); }
#line 2671 "parser.tab.cc"
    break;

  case 164:
#line 641 "parser.yy"
            { (yyval.formula) = &LessThanOrEqualTo::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2677 "parser.tab.cc"
    break;

  case 165:
#line 642 "parser.yy"
                 { require_fluents(); }
#line 2683 "parser.tab.cc"
    break;

  case 166:
#line 643 "parser.yy"
            { (yyval.formula) = &GreaterThanOrEqualTo::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2689 "parser.tab.cc"
    break;

  case 167:
#line 644 "parser.yy"
                  { require_fluents(); }
#line 2695 "parser.tab.cc"
    break;

  case 168:
#line 645 "parser.yy"
            { (yyval.formula) = &GreaterThan::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2701 "parser.tab.cc"
    break;

  case 169:
#line 646 "parser.yy"
                              { (yyval.formula) = make_negation(*(yyvsp[-1].formula)); }
#line 2707 "parser.tab.cc"
    break;

  case 170:
#line 647 "parser.yy"
                                { (yyval.formula) = (yyvsp[-1].formula); }
#line 2713 "parser.tab.cc"
    break;

  case 171:
#line 648 "parser.yy"
                 { require_disjunction(); }
#line 2719 "parser.tab.cc"
    break;

  case 172:
#line 648 "parser.yy"
                                                          { (yyval.formula) = (yyvsp[-1].formula); }
#line 2725 "parser.tab.cc"
    break;

  case 173:
#line 649 "parser.yy"
                                        { (yyval.formula) = make_implication(*(yyvsp[-2].formula), *(yyvsp[-1].formula)); }
#line 2731 "parser.tab.cc"
    break;

  case 174:
#line 650 "parser.yy"
                     { prepare_exists(); }
#line 2737 "parser.tab.cc"
    break;

  case 175:
#line 651 "parser.yy"
            { (yyval.formula) = make_exists(*(yyvsp[-1].formula)); }
#line 2743 "parser.tab.cc"
    break;

  case 176:
#line 652 "parser.yy"
                     { prepare_forall(); }
#line 2749 "parser.tab.cc"
    break;

  case 177:
#line 653 "parser.yy"
            { (yyval.formula) = make_forall(*(yyvsp[-1].formula)); }
#line 2755 "parser.tab.cc"
    break;

  case 178:
#line 656 "parser.yy"
                        { (yyval.formula) = &StateFormula::TRUE; }
#line 2761 "parser.tab.cc"
    break;

  case 179:
#line 657 "parser.yy"
                              { (yyval.formula) = &(*(yyvsp[-1].formula) && *(yyvsp[0].formula)); }
#line 2767 "parser.tab.cc"
    break;

  case 180:
#line 660 "parser.yy"
                        { (yyval.formula) = &StateFormula::FALSE; }
#line 2773 "parser.tab.cc"
    break;

  case 181:
#line 661 "parser.yy"
                              { (yyval.formula) = &(*(yyvsp[-1].formula) || *(yyvsp[0].formula)); }
#line 2779 "parser.tab.cc"
    break;

  case 182:
#line 664 "parser.yy"
                                    { prepare_atom((yyvsp[0].str)); }
#line 2785 "parser.tab.cc"
    break;

  case 183:
#line 665 "parser.yy"
                        { (yyval.atom) = make_atom(); }
#line 2791 "parser.tab.cc"
    break;

  case 184:
#line 666 "parser.yy"
                                { prepare_atom((yyvsp[0].str)); (yyval.atom) = make_atom(); }
#line 2797 "parser.tab.cc"
    break;

  case 185:
#line 669 "parser.yy"
                                    { prepare_atom((yyvsp[0].str)); }
#line 2803 "parser.tab.cc"
    break;

  case 186:
#line 670 "parser.yy"
                        { (yyval.atom) = make_atom(); }
#line 2809 "parser.tab.cc"
    break;

  case 187:
#line 671 "parser.yy"
                                { prepare_atom((yyvsp[0].str)); (yyval.atom) = make_atom(); }
#line 2815 "parser.tab.cc"
    break;

  case 188:
#line 678 "parser.yy"
               { (yyval.expr) = new Value(*(yyvsp[0].num)); delete (yyvsp[0].num); }
#line 2821 "parser.tab.cc"
    break;

  case 189:
#line 679 "parser.yy"
                                { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2827 "parser.tab.cc"
    break;

  case 190:
#line 680 "parser.yy"
                                    { (yyval.expr) = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
#line 2833 "parser.tab.cc"
    break;

  case 191:
#line 681 "parser.yy"
                                { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2839 "parser.tab.cc"
    break;

  case 192:
#line 682 "parser.yy"
                                { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2845 "parser.tab.cc"
    break;

  case 193:
#line 683 "parser.yy"
               { (yyval.expr) = (yyvsp[0].fluent); }
#line 2851 "parser.tab.cc"
    break;

  case 194:
#line 687 "parser.yy"
                  { require_fluents(); eq_expr = new Value(*(yyvsp[0].num)); delete (yyvsp[0].num); }
#line 2857 "parser.tab.cc"
    break;

  case 195:
#line 688 "parser.yy"
                        { require_fluents(); }
#line 2863 "parser.tab.cc"
    break;

  case 196:
#line 689 "parser.yy"
                  { eq_expr = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2869 "parser.tab.cc"
    break;

  case 197:
#line 690 "parser.yy"
                        { require_fluents(); }
#line 2875 "parser.tab.cc"
    break;

  case 198:
#line 691 "parser.yy"
                  { eq_expr = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
#line 2881 "parser.tab.cc"
    break;

  case 199:
#line 692 "parser.yy"
                        { require_fluents(); }
#line 2887 "parser.tab.cc"
    break;

  case 200:
#line 693 "parser.yy"
                  { eq_expr = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2893 "parser.tab.cc"
    break;

  case 201:
#line 694 "parser.yy"
                        { require_fluents(); }
#line 2899 "parser.tab.cc"
    break;

  case 202:
#line 695 "parser.yy"
                  { eq_expr = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2905 "parser.tab.cc"
    break;

  case 203:
#line 696 "parser.yy"
                             { require_fluents(); prepare_fluent((yyvsp[0].str)); }
#line 2911 "parser.tab.cc"
    break;

  case 204:
#line 697 "parser.yy"
                            { eq_expr = make_fluent(); }
#line 2917 "parser.tab.cc"
    break;

  case 205:
#line 698 "parser.yy"
                     { make_eq_name((yyvsp[0].str)); }
#line 2923 "parser.tab.cc"
    break;

  case 206:
#line 699 "parser.yy"
                         { eq_term = make_term((yyvsp[0].str)); eq_expr = 0; }
#line 2929 "parser.tab.cc"
    break;

  case 207:
#line 702 "parser.yy"
                        { (yyval.expr) = 0; }
#line 2935 "parser.tab.cc"
    break;

  case 209:
#line 706 "parser.yy"
                      { prepare_fluent((yyvsp[0].str)); }
#line 2941 "parser.tab.cc"
    break;

  case 210:
#line 706 "parser.yy"
                                                        { (yyval.fluent) = make_fluent(); }
#line 2947 "parser.tab.cc"
    break;

  case 211:
#line 707 "parser.yy"
                  { prepare_fluent((yyvsp[0].str)); (yyval.fluent) = make_fluent(); }
#line 2953 "parser.tab.cc"
    break;

  case 212:
#line 710 "parser.yy"
                      { (yyval.expr) = new Value(*(yyvsp[0].num)); delete (yyvsp[0].num); }
#line 2959 "parser.tab.cc"
    break;

  case 213:
#line 712 "parser.yy"
                 { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2965 "parser.tab.cc"
    break;

  case 214:
#line 714 "parser.yy"
                 { (yyval.expr) = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
#line 2971 "parser.tab.cc"
    break;

  case 215:
#line 716 "parser.yy"
                 { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2977 "parser.tab.cc"
    break;

  case 216:
#line 718 "parser.yy"
                 { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2983 "parser.tab.cc"
    break;

  case 217:
#line 719 "parser.yy"
                             { (yyval.expr) = (yyvsp[0].fluent); }
#line 2989 "parser.tab.cc"
    break;

  case 218:
#line 721 "parser.yy"
                 { (yyval.expr) = &Fluent::make(domain->total_time(), TermList()); }
#line 2995 "parser.tab.cc"
    break;

  case 219:
#line 723 "parser.yy"
                 { (yyval.expr) = &Fluent::make(domain->total_time(), TermList()); }
#line 3001 "parser.tab.cc"
    break;

  case 220:
#line 725 "parser.yy"
                 { (yyval.expr) = &Fluent::make(domain->goal_achieved(), TermList()); }
#line 3007 "parser.tab.cc"
    break;

  case 221:
#line 727 "parser.yy"
                 { (yyval.expr) = &Fluent::make(domain->goal_achieved(), TermList()); }
#line 3013 "parser.tab.cc"
    break;

  case 222:
#line 730 "parser.yy"
                                 { (yyval.vec_expr) = new VecExpression{(yyvsp[0].expr)}; }
#line 3019 "parser.tab.cc"
    break;

  case 223:
#line 731 "parser.yy"
                                                   { (yyvsp[0].vec_expr)->push_back((yyvsp[-1].expr)); (yyval.vec_expr) = (yyvsp[0].vec_expr); }
#line 3025 "parser.tab.cc"
    break;

  case 224:
#line 733 "parser.yy"
                               { (yyval.expr) = 0; }
#line 3031 "parser.tab.cc"
    break;

  case 226:
#line 737 "parser.yy"
                             { prepare_fluent((yyvsp[0].str)); }
#line 3037 "parser.tab.cc"
    break;

  case 227:
#line 738 "parser.yy"
                  { (yyval.fluent) = make_fluent(); }
#line 3043 "parser.tab.cc"
    break;

  case 228:
#line 739 "parser.yy"
                         { prepare_fluent((yyvsp[0].str)); (yyval.fluent) = make_fluent(); }
#line 3049 "parser.tab.cc"
    break;

  case 230:
#line 747 "parser.yy"
                   { add_term((yyvsp[0].str)); }
#line 3055 "parser.tab.cc"
    break;

  case 231:
#line 748 "parser.yy"
                       { add_term((yyvsp[0].str)); }
#line 3061 "parser.tab.cc"
    break;

  case 233:
#line 752 "parser.yy"
                   { add_term((yyvsp[0].str)); }
#line 3067 "parser.tab.cc"
    break;

  case 235:
#line 756 "parser.yy"
                         { add_variables((yyvsp[0].strs), TypeTable::OBJECT); }
#line 3073 "parser.tab.cc"
    break;

  case 236:
#line 757 "parser.yy"
                                   { add_variables((yyvsp[-1].strs), *(yyvsp[0].type)); delete (yyvsp[0].type); }
#line 3079 "parser.tab.cc"
    break;

  case 238:
#line 761 "parser.yy"
                        { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[0].str)); }
#line 3085 "parser.tab.cc"
    break;

  case 239:
#line 762 "parser.yy"
                                     { (yyval.strs) = (yyvsp[-1].strs); (yyval.strs)->push_back((yyvsp[0].str)); }
#line 3091 "parser.tab.cc"
    break;

  case 241:
#line 766 "parser.yy"
                       { add_names((yyvsp[0].strs), TypeTable::OBJECT); }
#line 3097 "parser.tab.cc"
    break;

  case 242:
#line 767 "parser.yy"
                                 { add_names((yyvsp[-1].strs), *(yyvsp[0].type)); delete (yyvsp[0].type); }
#line 3103 "parser.tab.cc"
    break;

  case 244:
#line 770 "parser.yy"
                { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[0].str)); }
#line 3109 "parser.tab.cc"
    break;

  case 245:
#line 771 "parser.yy"
                         { (yyval.strs) = (yyvsp[-1].strs); (yyval.strs)->push_back((yyvsp[0].str)); }
#line 3115 "parser.tab.cc"
    break;

  case 246:
#line 774 "parser.yy"
                { require_typing(); }
#line 3121 "parser.tab.cc"
    break;

  case 247:
#line 774 "parser.yy"
                                           { (yyval.type) = (yyvsp[0].type); }
#line 3127 "parser.tab.cc"
    break;

  case 248:
#line 777 "parser.yy"
              { (yyval.type) = new Type(TypeTable::OBJECT); }
#line 3133 "parser.tab.cc"
    break;

  case 249:
#line 778 "parser.yy"
                 { (yyval.type) = new Type(make_type((yyvsp[0].str))); }
#line 3139 "parser.tab.cc"
    break;

  case 250:
#line 779 "parser.yy"
                            { (yyval.type) = new Type(make_type(*(yyvsp[-1].types))); delete (yyvsp[-1].types); }
#line 3145 "parser.tab.cc"
    break;

  case 251:
#line 782 "parser.yy"
               { (yyval.types) = new TypeSet(); }
#line 3151 "parser.tab.cc"
    break;

  case 252:
#line 783 "parser.yy"
                  { (yyval.types) = new TypeSet(); (yyval.types)->insert(make_type((yyvsp[0].str))); }
#line 3157 "parser.tab.cc"
    break;

  case 253:
#line 784 "parser.yy"
                     { (yyval.types) = (yyvsp[-1].types); }
#line 3163 "parser.tab.cc"
    break;

  case 254:
#line 785 "parser.yy"
                        { (yyval.types) = (yyvsp[-1].types); (yyval.types)->insert(make_type((yyvsp[0].str))); }
#line 3169 "parser.tab.cc"
    break;

  case 256:
#line 795 "parser.yy"
                { delete (yyvsp[0].str); }
#line 3175 "parser.tab.cc"
    break;

  case 257:
#line 798 "parser.yy"
                      { delete (yyvsp[0].str); }
#line 3181 "parser.tab.cc"
    break;

  case 258:
#line 801 "parser.yy"
                  { delete (yyvsp[0].str); }
#line 3187 "parser.tab.cc"
    break;

  case 259:
#line 804 "parser.yy"
            { delete (yyvsp[0].str); }
#line 3193 "parser.tab.cc"
    break;

  case 260:
#line 807 "parser.yy"
          { delete (yyvsp[0].str); }
#line 3199 "parser.tab.cc"
    break;

  case 261:
#line 810 "parser.yy"
          { delete (yyvsp[0].str); }
#line 3205 "parser.tab.cc"
    break;

  case 262:
#line 813 "parser.yy"
        { delete (yyvsp[0].str); }
#line 3211 "parser.tab.cc"
    break;

  case 263:
#line 816 "parser.yy"
              { delete (yyvsp[0].str); }
#line 3217 "parser.tab.cc"
    break;

  case 264:
#line 819 "parser.yy"
                { delete (yyvsp[0].str); }
#line 3223 "parser.tab.cc"
    break;

  case 265:
#line 822 "parser.yy"
                { delete (yyvsp[0].str); }
#line 3229 "parser.tab.cc"
    break;

  case 266:
#line 825 "parser.yy"
                              { delete (yyvsp[0].str); }
#line 3235 "parser.tab.cc"
    break;

  case 267:
#line 828 "parser.yy"
                { delete (yyvsp[0].str); }
#line 3241 "parser.tab.cc"
    break;

  case 268:
#line 831 "parser.yy"
                    { delete (yyvsp[0].str); }
#line 3247 "parser.tab.cc"
    break;

  case 269:
#line 834 "parser.yy"
                        { delete (yyvsp[0].str); }
#line 3253 "parser.tab.cc"
    break;

  case 270:
#line 837 "parser.yy"
                    { delete (yyvsp[0].str); }
#line 3259 "parser.tab.cc"
    break;

  case 271:
#line 840 "parser.yy"
                    { delete (yyvsp[0].str); }
#line 3265 "parser.tab.cc"
    break;

  case 272:
#line 843 "parser.yy"
                    { delete (yyvsp[0].str); }
#line 3271 "parser.tab.cc"
    break;

  case 273:
#line 846 "parser.yy"
                    { delete (yyvsp[0].str); }
#line 3277 "parser.tab.cc"
    break;

  case 274:
#line 849 "parser.yy"
                      { delete (yyvsp[0].str); }
#line 3283 "parser.tab.cc"
    break;

  case 275:
#line 852 "parser.yy"
                      { delete (yyvsp[0].str); }
#line 3289 "parser.tab.cc"
    break;

  case 276:
#line 855 "parser.yy"
                { delete (yyvsp[0].str); }
#line 3295 "parser.tab.cc"
    break;


#line 3299 "parser.tab.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 882 "parser.yy"


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
