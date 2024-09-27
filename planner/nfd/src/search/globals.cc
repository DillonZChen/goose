#include "globals.h"

#include "axioms.h"
#include "causal_graph.h"
#include "global_operator.h"
#include "global_state.h"
#include "heuristic.h"
#include "int_packer.h"
#include "state_registry.h"
#include "successor_generator.h"
#include "per_state_information.h" // instrumentation variables are stored as PSI

#include "tasks/root_task.h"

#include "utils/rng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include "utils/logging.h"
#include "utils/planvis.h"


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "transformation/resource_transformation.h"

using namespace std;
using utils::ExitCode;

static const int PRE_FILE_VERSION = 4;

// TODO: This needs a proper type and should be moved to a separate
//       mutexes.cc file or similar, accessed via something called
//       g_mutexes. (Right now, the interface is via global function
//       are_mutex, which is at least better than exposing the data
//       structure globally.)

static vector<vector<set<Fact> > > g_inconsistent_facts;
static list<set<Fact> >  g_mutex_group;

bool test_goal(const GlobalState &state) {
    // for (size_t i = 0; i < g_goal.size(); ++i) {
    //   int j = g_goal[i].first;
    //   std::cout << j << " " << state[j] << " " << g_goal[i].second << std::endl;
    // }
    // exit(-1);
    for (size_t i = 0; i < g_goal.size(); ++i) {
        if (state[g_goal[i].first] != g_goal[i].second) {
            return false;
        }
    }
    return true;
}

ap_float calculate_plan_cost(const vector<const GlobalOperator *> &plan) {
    // TODO: Refactor: this is only used by save_plan (see below)
    //       and the SearchEngine classes and hence should maybe
    //       be moved into the SearchEngine (along with save_plan).
    ap_float plan_cost = 0;
    for (size_t i = 0; i < plan.size(); ++i) {
        plan_cost += plan[i]->get_cost();
    }
    return plan_cost;
}

void save_plan(const vector<const GlobalOperator *> &plan,
               bool generates_multiple_plan_files) {
    // TODO: Refactor: this is only used by the SearchEngine classes
    //       and hence should maybe be moved into the SearchEngine.
    ostringstream filename;
    filename << g_plan_filename;
    int plan_number = g_num_previously_generated_plans + 1;
    if (generates_multiple_plan_files || g_is_part_of_anytime_portfolio) {
        filename << "." << plan_number;
    } else {
        assert(plan_number == 1);
    }
    ofstream outfile(filename.str());
    for (size_t i = 0; i < plan.size(); ++i) {
        cout << plan[i]->get_name() << " (" << plan[i]->get_cost() << ")" << endl;
        outfile << "(" << plan[i]->get_name() << ")" << endl;
    }
    ap_float plan_cost = calculate_plan_cost(plan);
    outfile << "; cost = " << plan_cost << " ("
            << (is_unit_cost() ? "unit cost" : "general cost") << ")" << endl;
    outfile.close();
    cout << "Plan length: " << plan.size() << " step(s)." << endl;
    cout << "Plan cost: " << plan_cost << endl;
    ++g_num_previously_generated_plans;
}

void check_magic(istream &in, string magic) {
    string word;
    in >> word;
    if (word != magic) {
        cout << "Failed to match magic word '" << magic << "'." << endl;
        cout << "Got '" << word << "'." << endl;
        if (magic == "begin_version") {
            cerr << "Possible cause: you are running the planner "
                 << "on a preprocessor file from " << endl
                 << "an older version." << endl;
        }
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
}

void read_and_verify_version(istream &in) {
    int version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != PRE_FILE_VERSION) {
        cerr << "Expected preprocessor file version " << PRE_FILE_VERSION
             << ", got " << version << "." << endl;
        cerr << "Exiting." << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
}

void read_metric(istream &in) {
    check_magic(in, "begin_metric");
    char optimization;
    in >> optimization;
    if (optimization == '<') {
    	g_metric_minimizes = true;
    } else {
    	assert(optimization == '>');
    	g_metric_minimizes = false;
    }
    if(DEBUG) cout << "The metric " << (g_metric_minimizes?"minimizes":"maximizes") << endl;
    in >> g_metric_fluent_id;
    g_use_metric = (g_metric_fluent_id != -1);
    check_magic(in, "end_metric");
}

void read_variables(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i) {
        check_magic(in, "begin_variable");
        string name;
        in >> name;
        g_variable_name.push_back(name);
        int layer;
        in >> layer;
        g_axiom_layers.push_back(layer);
        int range;
        in >> range;
        g_variable_domain.push_back(range);
        in >> ws;
        vector<string> fact_names(range);
        for (size_t j = 0; j < fact_names.size(); ++j)
            getline(in, fact_names[j]);
        g_fact_names.push_back(fact_names);
        check_magic(in, "end_variable");
    }
}

void read_numeric_variables(istream &in) {
    int count;
    in >> count;
    if (DEBUG) cout << "Domain has " << count << " numeric variables " << endl;
    check_magic(in, "begin_numeric_variables");
    g_last_arithmetic_axiom_layer = -1;
    for (int i = 0; i < count; i++) {
    	numType type;
        int layer;
        in >> type >> layer >> ws;
//        if (DEBUG) cout << "numvar in layer " << layer << endl;
        g_last_arithmetic_axiom_layer = max(layer, g_last_arithmetic_axiom_layer);
        string name;
        getline(in,name);
//        if(DEBUG) cout << "Reading numeric variable index : " << i << " name " << name << " type " << type << endl;
        g_numeric_axiom_layers.push_back(layer);
        g_numeric_var_names.push_back(name);
        g_numeric_var_types.push_back(type);
    }
    check_magic(in, "end_numeric_variables");
}


void read_mutexes(istream &in) {
    g_inconsistent_facts.resize(g_variable_domain.size());
    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        g_inconsistent_facts[i].resize(g_variable_domain[i]);

    int num_mutex_groups;
    in >> num_mutex_groups;

    /* NOTE: Mutex groups can overlap, in which case the same mutex
       should not be represented multiple times. The current
       representation takes care of that automatically by using sets.
       If we ever change this representation, this is something to be
       aware of. */

    for (int i = 0; i < num_mutex_groups; ++i) {
        check_magic(in, "begin_mutex_group");
        int num_facts;
        in >> num_facts;
        vector<Fact> invariant_group;
        invariant_group.reserve(num_facts);
        set<Fact> mutex_group;
        for (int j = 0; j < num_facts; ++j) {
            int var;
            int value;
            in >> var >> value;
            invariant_group.emplace_back(var, value);
            mutex_group.insert(Fact(var,value));
        }
        g_mutex_group.push_back(mutex_group);
        check_magic(in, "end_mutex_group");
        for (const Fact &fact1 : invariant_group) {
            for (const Fact &fact2 : invariant_group) {
                if (fact1.var != fact2.var) {
                    /* The "different variable" test makes sure we
                       don't mark a fact as mutex with itself
                       (important for correctness) and don't include
                       redundant mutexes (important to conserve
                       memory). Note that the preprocessor removes
                       mutex groups that contain *only* redundant
                       mutexes, but it can of course generate mutex
                       groups which lead to *some* redundant mutexes,
                       where some but not all facts talk about the
                       same variable. */
                    g_inconsistent_facts[fact1.var][fact1.value].insert(fact2);
                }
            }
        }
    }
}

void read_goal(istream &in) {
    check_magic(in, "begin_goal");
    int count;
    in >> count;
    if (count < 1) {
        cerr << "Task has no goal condition!" << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
    for (int i = 0; i < count; ++i) {
        int var, val;
        in >> var >> val;
        g_goal.push_back(make_pair(var, val));
    }
    check_magic(in, "end_goal");
}

void read_global_constraint(istream &in) {
    check_magic(in, "begin_global_constraint");
    in >> g_global_constraint_var_id >> g_global_constraint_val;
    check_magic(in, "end_global_constraint");
}

void dump_goal() {
    cout << "Goal Conditions:" << endl;
    for (size_t i = 0; i < g_goal.size(); ++i)
        cout << "  " << g_variable_name[g_goal[i].first] << ": "
             << g_goal[i].second << endl;
}

void read_operators(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i)
        g_operators.push_back(GlobalOperator(in, false));
}

/**
 * Provides a Method to convert Axioms to Operator.
 * This can be used in heuristics that do not support the new LogicAxiom strutcture
 * and still treat Axioms as Zero-Cost Operators
 */
//GlobalOperator axiom_to_operator(LogicAxiom axiom) {
//	vector<PrePost> prepost;
//	vector<Prevail> conditional_effect;
//	vector<AssignEffect> ass_eff;
//	vector<AssignEffect> inst_eff;
//	PrePost axiom_change = PrePost(axiom.affected_variable, axiom.old_value, axiom.new_value, conditional_effect);
//	prepost.push_back(axiom_change);
//	stringstream ax_name_s;
//	ax_name_s << "axiom_" << axiom.affected_variable << "_" << axiom.old_value << "_" << axiom.new_value;
//	string ax_name = ax_name_s.str();
//	if (DEBUG) {
//		cout << "Convert Axiom " << ax_name << " into Operator" << endl;
//	}
//	return GlobalOperator(true, axiom.prevail, prepost, ass_eff, inst_eff, ax_name, 0, false);
//}


void read_axioms(istream &in) {
    int count;
    in >> count;
    if(DEBUG) cout << "Reading " << count << " propositional axioms " << endl;
    for (int i = 0; i < count; ++i) {
    	PropositionalAxiom nextAxiom = PropositionalAxiom(in);
        g_logic_axioms.push_back(nextAxiom);
        g_axioms_as_operator.push_back(GlobalOperator(nextAxiom));
    }
    in >> count;
    if(DEBUG) cout << "Reading " << count << " comparison axioms " << endl;
    check_magic(in, "begin_comparison_axioms");
    for (int i = 0; i < count; i++) {
    	g_comp_axioms.push_back(ComparisonAxiom(in));
    }
    check_magic(in, "end_comparison_axioms");
    in >> count;
    if(DEBUG) cout << "Reading " << count << " assignment axioms " << endl;
    check_magic(in, "begin_numeric_axioms");
    for (int i = 0; i < count; i++) {
    	AssignmentAxiom next = AssignmentAxiom(in);
//    	if (DEBUG) cout << "Assignment Axiom #" << i << ": "<< g_numeric_var_names[next.affected_variable] << " = ("
//    		        << g_numeric_var_names[next.var_lhs] << " " << next.op << " " <<
//    		        g_numeric_var_names[next.var_rhs] << ") is in layer " << g_numeric_axiom_layers[next.affected_variable] << endl;
//    	next.dump();
    	g_ass_axioms.push_back(next);
    }
    check_magic(in, "end_numeric_axioms");
    if (DEBUG) {
    	assert(count == (int) g_ass_axioms.size());
    	cout << "Verifying Axiom structure... " << endl;
    	int layer = -1;
    	for (const auto ax : g_ass_axioms) {
//    			ax.dump();
//    			cout << "Axiom layer " << g_numeric_axiom_layers[ax.affected_variable]
//    			     << " >= last layer " << layer << endl;
    		assert(g_numeric_axiom_layers[ax.affected_variable] >= layer);
    		layer = g_numeric_axiom_layers[ax.affected_variable];
    	}
    	_unused(layer); // disable compiler warning that layer is only used to check assertion
    }
    g_axiom_evaluator = new AxiomEvaluator; // requires the sizes of all axioms, do not call earlier
}

void read_everything(istream &in) {
    cout << "reading input... [t=" << utils::g_timer << "]" << endl;
    string input_file_name;
    in >> input_file_name;
    read_and_verify_version(in);
    read_metric(in);
    read_variables(in);
    read_numeric_variables(in);
    read_mutexes(in);
    g_initial_state_data.resize(g_variable_domain.size());
    check_magic(in, "begin_state");
    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        in >> g_initial_state_data[i];
    }
    check_magic(in, "end_state");
    if (DEBUG) cout << "Reading numeric state..." <<endl;
    check_magic(in, "begin_numeric_state");
    g_initial_state_numeric.resize(g_numeric_var_names.size());
    for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    	in >> g_initial_state_numeric[i];
    }
    check_magic(in, "end_numeric_state");
    g_default_axiom_values = g_initial_state_data;

    read_goal(in);
    read_operators(in);
    if (DEBUG) cout << " Operators before reading axioms: " << g_operators.size() << endl;
    read_axioms(in);
//    if (DEBUG) {cout << " read " <<g_logic_axioms.size() <<" axioms... " << endl;
//    	for (auto axiom : g_logic_axioms) {
//    		axiom.dump();
//    		for (auto cond : axiom.get_preconditions()) {
//    			cout << "["<<cond.var<<"]["<<cond.val<<"]= "<< g_fact_names[cond.var][cond.val] << ", ";
//    		}
//    		cout << "--> ";
//    		for (auto eff : axiom.get_effects()) {
//    			cout << "["<<eff.var<<"]["<<eff.val<<"]= " << g_fact_names[eff.var][eff.val] << ", ";
//    		}
//    		cout << endl;
//    	}
//    }

    read_global_constraint(in);

    // Ignore successor generator from preprocessor output.
    check_magic(in, "begin_SG");
    string dummy_string = "";
    while (dummy_string != "end_SG") {
        getline(in, dummy_string);
    }

    check_magic(in, "begin_DTG"); // ignore everything from here

    cout << "done reading input! [t=" << utils::g_timer << "]" << endl;

    cout << "packing state variables..." << flush;
    assert(!g_variable_domain.empty());
    vector<container_int> statespace_sizes = g_variable_domain;
    int numeric_constants = 0;
    int instrumentation_vars = 0;
    for (auto& type : g_numeric_var_types) {
    	if (type == constant) ++numeric_constants;
    	if (type == instrumentation) ++instrumentation_vars;
    }
    if (DEBUG) cout << "Task has " << numeric_constants << " numeric constants and " << instrumentation_vars << " instrumentation variables." << endl;
    /*
	 The state packer's bin size is either 32 bit or 64 bit.
	 Note that in order to have 64 bit precision floats a.k.a "double"
	 the container_int type has to be an unsigned 64 bit int (unsigned long long)

	 we only reserve space for non-constant non-instrumentation vars
	 TODO: currently derived variables are stored in the state. If this is changed for regular variables, numeric variables should change as well
	 */
    int regular_variables = g_numeric_var_names.size() - numeric_constants - instrumentation_vars;
    if (DEBUG) cout << "Task has " << regular_variables << " regular (or derived)  variables." << endl;
    assert(regular_variables >= 0);
    statespace_sizes.resize(statespace_sizes.size()+regular_variables, numeric_limits<container_int>::max());
//    cout << "initializing state packer ..." << endl;
    g_state_packer = new IntPacker(statespace_sizes);
    cout << "done! [t=" << utils::g_timer << "]" << endl;

    // NOTE: state registry stores the sizes of the state, so must be
    // built after the problem has been read in.
    g_state_registry = new StateRegistry(numeric_constants);

    int num_vars = g_variable_domain.size();
    int num_facts = 0;
    for (int var = 0; var < num_vars; ++var)
        num_facts += g_variable_domain[var];

    if(DEBUG) cout << "Evaluate metric in initial state to determine true cost of all Operators"<< endl;
    ap_float init_cost = g_state_registry->evaluate_metric(g_initial_state().get_numeric_vars());
    if(DEBUG) cout << "Cost of initial state is " << init_cost << endl; // usually 0.0

    for (auto &op : g_operators) {
    	op.set_cost(init_cost); // has to be done after the g_state_registry has been created
    	g_min_action_cost = min(g_min_action_cost, op.get_cost());
    	g_max_action_cost = max(g_max_action_cost, op.get_cost());
    }

    cout << "Variables: " << num_vars << endl;
    cout << "Facts: " << num_facts << endl;
    cout << "Bytes per state: "
         << g_state_packer->get_num_bins() *
        g_state_packer->get_bin_size_in_bytes() << endl;

    cout << "Building successor generator..." << flush;
    g_successor_generator = new SuccessorGenerator(g_root_task());
    cout << "done! [t=" << utils::g_timer << "]" << endl;

    cout << "done initalizing global data [t=" << utils::g_timer << "]" << endl;
    if (PLAN_VIS_LOG != 0) g_plan_logger = new utils::PlanVisLogger();

    std::remove(input_file_name.c_str());
}


void dump_variable(size_t variable_id) {
	cout << g_variable_name[variable_id] << " {" << g_fact_names[variable_id][0];
	for (size_t i = 1; i < g_fact_names[variable_id].size(); ++i)
		cout << ", " << g_fact_names[variable_id][i];
	cout << "}" << endl;
}

void dump_everything() {
    cout << "Use metric? " << g_use_metric << endl;
    cout << "Min Action Cost: " << g_min_action_cost << endl;
    cout << "Max Action Cost: " << g_max_action_cost << endl;
    // TODO: Dump the actual fact names.
    cout << "Variables (" << g_variable_name.size() << "):" << endl;
    for (size_t i = 0; i < g_variable_name.size(); ++i)
        cout << "  " << g_variable_name[i]
             << " (range " << g_variable_domain[i] << ")" << endl;
    GlobalState initial_state = g_initial_state();
    cout << "Initial State (PDDL):" << endl;
    initial_state.dump_pddl();
    cout << "Initial State (FDR):" << endl;
    initial_state.dump_fdr();
    dump_goal();
    /*
    if(DEBUG) { cout << "Successor Generator:" << endl;
	    g_successor_generator->dump();
	}
    for(int i = 0; i < g_variable_domain.size(); ++i)
      g_transition_graphs[i]->dump();
    */
}

bool is_unit_cost() {
    return g_min_action_cost == 1 && g_max_action_cost == 1;
}

bool has_axioms() {
	return has_logic_axioms() || has_numeric_axioms();
}

bool has_logic_axioms() {
	return !g_logic_axioms.empty();
}

bool has_numeric_axioms() {
	return (!g_ass_axioms.empty() || !g_comp_axioms.empty());
}

void verify_no_axioms() {
    if (has_axioms()) {
        cerr << "Heuristic does not support axioms!" << endl << "Terminating."
             << endl;
        utils::exit_with(ExitCode::UNSUPPORTED);
    }
}

static int get_first_conditional_effects_op_id() {
    for (size_t i = 0; i < g_operators.size(); ++i) {
        const vector<GlobalEffect> &effects = g_operators[i].get_effects();
        for (size_t j = 0; j < effects.size(); ++j) {
            const vector<GlobalCondition> &cond = effects[j].conditions;
            if (!cond.empty())
                return i;
        }
    }
    return -1;
}

bool has_conditional_effects() {
    return get_first_conditional_effects_op_id() != -1;
}

void verify_no_conditional_effects() {
    int op_id = get_first_conditional_effects_op_id();
    if (op_id != -1) {
        cerr << "Heuristic does not support conditional effects "
             << "(operator " << g_operators[op_id].get_name() << ")" << endl
             << "Terminating." << endl;
        utils::exit_with(ExitCode::UNSUPPORTED);
    }
}

void verify_no_axioms_no_conditional_effects() {
    verify_no_axioms();
    verify_no_conditional_effects();
}

bool are_mutex(const Fact &a, const Fact &b) {
    if (a.var == b.var) {
        // Same variable: mutex iff different value.
        return a.value != b.value;
    }
    return bool(g_inconsistent_facts[a.var][a.value].count(b));
}

list<set<Fact>> get_mutex_group(){
    return g_mutex_group;
}
const GlobalState &g_initial_state() {
    return g_state_registry->get_initial_state();
}

const shared_ptr<AbstractTask> g_root_task() {
    const shared_ptr<AbstractTask> root_task = make_shared<tasks::RootTask>();
    return root_task;//extra_tasks::create_resource_task(root_task);
}

shared_ptr<utils::RandomNumberGenerator> g_rng() {
    // Use an arbitrary default seed.
    static shared_ptr<utils::RandomNumberGenerator> rng =
        make_shared<utils::RandomNumberGenerator>(2011);
    return rng;
}

bool g_use_metric;
bool g_metric_minimizes;
int g_metric_fluent_id;

ap_float g_min_action_cost = numeric_limits<ap_float>::max();
ap_float g_max_action_cost = 0;
vector<string> g_variable_name;
vector<container_int> g_variable_domain;
vector<vector<string> > g_fact_names;
vector<string> g_numeric_var_names;
vector<numType> g_numeric_var_types;
vector<int> g_axiom_layers; // [i] stores the axiom layer of i-th variable
vector<int> g_numeric_axiom_layers; // [i] stores the axiom layer of i-th numeric variables
vector<container_int> g_default_axiom_values;
IntPacker *g_state_packer;
vector<container_int> g_initial_state_data;
vector<ap_float> g_initial_state_numeric;
vector<pair<int, container_int> > g_goal;
vector<GlobalOperator> g_operators;
vector<GlobalOperator> g_axioms_as_operator;
vector<PropositionalAxiom> g_logic_axioms;
vector<ComparisonAxiom> g_comp_axioms;
vector<AssignmentAxiom> g_ass_axioms; // ordered by layer !!!
int g_global_constraint_var_id;
int g_global_constraint_val;

AxiomEvaluator *g_axiom_evaluator;
SuccessorGenerator *g_successor_generator;
int g_last_arithmetic_axiom_layer;
int g_comparison_axiom_layer;
int g_first_logic_axiom_layer;
int g_last_logic_axiom_layer;

string g_plan_filename = "sas_plan";
int g_num_previously_generated_plans = 0;
bool g_is_part_of_anytime_portfolio = false;
StateRegistry *g_state_registry = 0;
PerStateInformation<std::vector<ap_float> > g_cost_information;

//TODO: the loggers should be managed in the same class
utils::Log g_log;
utils::PlanVisLogger *g_plan_logger = 0;


istream& operator>>(istream &is, cal_operator &cop) {
	string strVal;
	is >> strVal;
	if(!strVal.compare("+"))
		cop = sum;
	else if(!strVal.compare("-"))
		cop = diff;
	else if(!strVal.compare("*"))
		cop = mult;
	else if(!strVal.compare("/"))
		cop = divi;
	else {
    	cerr << "Unknown operator : '" << strVal << "'" << endl;
        assert(false);
    }
    return is;
}

ostream& operator<<(ostream &os, const cal_operator &cop) {
	switch (cop) {
	case sum:
		os << "+";
		break;
	case diff:
		os << "-";
		break;
	case mult:
		os << "*";
		break;
	case divi:
		os << "/";
		break;
	default:
		cout << (int)cop << " was read" << endl;
		assert(false);
		break;
	}
	return os;
}

istream& operator>>(istream &is, f_operator &fop) {
    string strVal;
    is >> strVal;
    if(!strVal.compare("="))
        fop = assign;
    else if(!strVal.compare("+"))
        fop = increase;
    else if(!strVal.compare("-"))
        fop = decrease;
    else if(!strVal.compare("*"))
        fop = scale_up;
    else if(!strVal.compare("/"))
        fop = scale_down;
    else {
    	cerr << "Unknown assignment operator : '" << strVal << "'" << endl;
        assert(false);
    }
    return is;
}

ostream& operator<<(ostream &os, const f_operator &fop) {
    switch (fop) {
        case assign:
            os << ":=";
            break;
        case scale_up:
            os << "*=";
            break;
        case scale_down:
            os << "/=";
            break;
        case increase:
            os << "+=";
            break;
        case decrease:
            os << "-=";
            break;
        default:
            cout << (int)fop << " was read" << endl;
            assert(false);
            break;
    }
    return os;
}

istream& operator>>(istream &is, comp_operator &cop) {
    string strVal;
    is >> strVal;
    if(!strVal.compare("<"))
        cop = lt;
    else if(!strVal.compare("<="))
        cop = le;
    else if(!strVal.compare("="))
        cop = eq;
    else if(!strVal.compare(">="))
        cop = ge;
    else if(!strVal.compare(">"))
        cop = gt;
    else if(!strVal.compare("!="))
        cop = ue;
    else
        assert(false);
    return is;
}

ostream& operator<<(ostream &os, const comp_operator &cop) {
    switch (cop) {
        case lt:
            os << "<";
            break;
        case le:
            os << "<=";
            break;
        case eq:
            os << "=";
            break;
        case ge:
            os << ">=";
            break;
        case gt:
            os << ">";
            break;
        case ue:
            os << "!=";
            break;
        default:
            cout << cop << " WAS READ" << endl;
            assert(false);
            break;
    }
    return os;
}

istream& operator>>(istream &is, numType &nt) {
	string strVal;
	is >> strVal;
	if(!strVal.compare("R"))
		nt = regular;
	else if(!strVal.compare("D"))
		nt = derived;
	else if(!strVal.compare("I"))
		nt = instrumentation;
	else if(!strVal.compare("C"))
		nt = constant;
	else if(!strVal.compare("U"))
		nt = unknown;
	else {
		cout << "unknown variable type read: '" << strVal << "'" << endl;
		assert(false);
	}
	return is;
}

ostream& operator<<(ostream &os, const numType &nt) {
    switch (nt) {
        case unknown:
            os << "U";
            break;
        case constant:
            os << "C";
            break;
        case derived:
            os << "D";
            break;
        case instrumentation:
            os << "I";
            break;
        case regular:
            os << "R";
            break;
        default:
            cout << nt << " WAS READ" << endl;
            assert(false);
            break;
    }
    return os;
}

comp_operator get_inverse_op(comp_operator op) {
    switch (op) {
        case lt:
            return ge;
        case le:
            return gt;
        case eq:
            return ue;
        case ge:
            return lt;
        case gt:
            return le;
        case ue:
            return eq;
        default:
            cout << op << " WAS READ" << endl;
            assert(false);
            break;
    }
    assert(false);
    return eq;
}

comp_operator get_mirror_op(comp_operator op) {
    switch (op) {
        case lt:
            return gt;
        case le:
            return ge;
        case eq:
            return eq;
        case ge:
            return le;
        case gt:
            return lt;
        case ue:
            return ue;
        default:
            cout << op << " WAS READ" << endl;
            assert(false);
            break;
    }
    assert(false);
    return eq;
}

cal_operator get_inverse_op(cal_operator cop) {
	switch (cop) {
	case sum:
		return diff;
	case diff:
		return sum;
	case mult:
		return divi;
	case divi:
		return mult;
	default:
		cout << cop << " WAS READ" << endl;
		assert(false);
		break;
	}
	assert(false);
	return sum;
}

// Globals for symmetries
GraphCreator* g_symmetry_graph = nullptr;