#ifndef GLOBALS_H
#define GLOBALS_H

#define _unused(x) ((void)x) // unused Macro for unused assert() verification variables

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>
#include <limits>
#include <list>
#include <set>

class AbstractTask;
class Axiom;
class PropositionalAxiom;
class ComparisonAxiom;
class AssignmentAxiom;
class AxiomEvaluator;
class CausalGraph;
struct Fact;
class StateRegistry;
class GlobalOperator;
class GlobalState;
class IntPacker;
class SuccessorGenerator;

class GraphCreator;

namespace utils {
struct Log;
class PlanVisLogger;
class RandomNumberGenerator;
}

template<class Entry> class PerStateInformation;

// TODO: the encoding size of floats has to be determined by command line
// a container_int has to be an integer variable with the same encoding size as ap_float
typedef unsigned long long container_int;
//typedef unsigned int container_int;
typedef double ap_float;
// typedef float ap_float;

enum planVisLog {no_plan_vis_log = 0, plan_vis_log = 1, latex_only = 2};

static const bool DEBUG = false;
static const planVisLog PLAN_VIS_LOG = no_plan_vis_log; // no_plan_vis_log; // needed for Benedict Wright's Plan Visualizer

// already declared somewhere else, right? static const int PRE_FILE_VERSION = 4;
extern std::string g_plan_vis_filename;
static const ap_float EPSILON = 0.000001; // needed for comparison axioms (currently only in interval max heuristic)
static constexpr ap_float INF = std::numeric_limits<ap_float>::infinity();

bool test_goal(const GlobalState &state);
/*
  Set generates_multiple_plan_files to true if the planner can find more than
  one plan and should number the plans as FILENAME.1, ..., FILENAME.n.
*/
void save_plan(const std::vector<const GlobalOperator *> &plan,
               bool generates_multiple_plan_files = false);
ap_float calculate_plan_cost(const std::vector<const GlobalOperator *> &plan);


void read_everything(std::istream &in);
void dump_variable(size_t variable_id);
void dump_everything();


// The following functions are deprecated. Use task_tools.h instead.
bool is_unit_cost();
bool has_axioms();
bool has_logic_axioms();
bool has_numeric_axioms();
void verify_no_axioms();
bool has_conditional_effects();
void verify_no_conditional_effects();
void verify_no_axioms_no_conditional_effects();

void check_magic(std::istream &in, std::string magic);

bool are_mutex(const Fact &a, const Fact &b);
std::list<std::set<Fact>> get_mutex_group();

enum f_operator // used by numeric operator effects
{
    assign = 0, scale_up = 1, scale_down = 2, increase = 3, decrease = 4
};
enum cal_operator { // used by numeric assignment axioms
	sum = 0, diff = 1, mult = 2, divi = 3
};
enum comp_operator // used by numeric comparison axioms
{
    lt = 0, le = 1, eq = 2, ge = 3, gt = 4, ue = 5
};

enum numType
{
	unknown = 0,
	constant = 1,
	derived = 2,
	instrumentation = 3,
	regular = 4
};

std::istream& operator>>(std::istream &is, cal_operator &cop);
std::ostream& operator<<(std::ostream &os, const cal_operator &cop);
std::istream& operator>>(std::istream &is, f_operator &fop);
std::ostream& operator<<(std::ostream &os, const f_operator &fop);
std::istream& operator>>(std::istream &is, comp_operator &cop);
std::ostream& operator<<(std::ostream &os, const comp_operator &cop);
std::istream& operator>>(std::istream &is, numType &nt);
std::ostream& operator<<(std::ostream &os, const numType &nt);


comp_operator get_inverse_op(comp_operator op);
comp_operator get_mirror_op(comp_operator op);
cal_operator get_inverse_op(cal_operator cop);

extern bool g_use_metric;
extern bool g_metric_minimizes;
extern int g_metric_fluent_id;

extern ap_float g_min_action_cost;
extern ap_float g_max_action_cost;

// TODO: The following five belong into a new Variable class.
extern std::vector<std::string> g_variable_name;
extern std::vector<container_int> g_variable_domain;
extern std::vector<std::vector<std::string> > g_fact_names;
extern std::vector<int> g_axiom_layers;
extern std::vector<container_int> g_default_axiom_values;
// TODO: as do the numeric variables
extern std::vector<int> g_numeric_axiom_layers;
extern std::vector<std::string> g_numeric_var_names;
extern std::vector<numType> g_numeric_var_types;

extern IntPacker *g_state_packer;
// This vector holds the initial values *before* the axioms have been evaluated.
// Use the state registry to obtain the real initial state.
extern std::vector<container_int> g_initial_state_data;
extern std::vector<ap_float> g_initial_state_numeric;


// TODO The following function returns the initial state that is registered
//      in g_state_registry. This is only a short-term solution. In the
//      medium term, we should get rid of the global registry.
extern const GlobalState &g_initial_state();
extern std::vector<std::pair<int, container_int> > g_goal; // var-value pairs of the goals

extern std::vector<GlobalOperator> g_operators;
extern std::vector<PropositionalAxiom> g_logic_axioms;
extern std::vector<GlobalOperator> g_axioms_as_operator;  // backwards compatibility for heuristics
													// that assume that g_axioms contains Operators (it did before
												    // the numeric extension of Fast Downward NFD)
extern std::vector<ComparisonAxiom> g_comp_axioms;
extern std::vector<AssignmentAxiom> g_ass_axioms;
extern int g_global_constraint_var_id;
extern int g_global_constraint_val;
extern AxiomEvaluator *g_axiom_evaluator;
extern SuccessorGenerator *g_successor_generator;
extern std::string g_plan_filename;
extern int g_num_previously_generated_plans;
extern bool g_is_part_of_anytime_portfolio;
extern std::shared_ptr<utils::RandomNumberGenerator> g_rng();
// Only one global object for now. Could later be changed to use one instance
// for each problem in this case the method GlobalState::get_id would also have to be
// changed.
extern StateRegistry *g_state_registry;
extern PerStateInformation<std::vector<ap_float>> g_cost_information;
extern utils::PlanVisLogger *g_plan_logger;
extern int g_last_arithmetic_axiom_layer;
extern int g_comparison_axiom_layer;
extern int g_first_logic_axiom_layer;
extern int g_last_logic_axiom_layer;

extern const std::shared_ptr<AbstractTask> g_root_task();

extern utils::Log g_log;

extern GraphCreator *g_symmetry_graph;

#endif
