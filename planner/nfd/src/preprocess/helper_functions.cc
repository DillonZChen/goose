#include <cstdlib>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
using namespace std;

#include "helper_functions.h"
#include "state.h"
#include "mutex_group.h"
#include "operator.h"
#include "axiom.h"
#include "variable.h"
#include "successor_generator.h"
#include "domain_transition_graph.h"


static const int SAS_FILE_VERSION = 4;
static const int PRE_FILE_VERSION = SAS_FILE_VERSION;


void check_magic(istream &in, string magic) {
    string word;
    in >> word;
    if (word != magic) {
        cerr << "Failed to match magic word '" << magic << "'." << endl;
        cerr << "Got '" << word << "'." << endl;
        if (magic == "begin_version") {
            cerr << "Possible cause: you are running the preprocessor "
                 << "on a translator file from an " << endl
                 << "older version." << endl;
        }
        exit(1);
    }
}

void read_and_verify_version(istream &in) {
    int version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != SAS_FILE_VERSION) {
        cerr << "Expected translator file version " << SAS_FILE_VERSION
             << ", got " << version << "." << endl;
        cerr << "Exiting." << endl;
        exit(1);
    }
}

void read_metric(istream &in, Metric &metric) {
    check_magic(in, "begin_metric");
    in >> metric.optimization_criterion;
    in >> metric.index;
    assert(metric.index >= 0); // for unit-cost tasks a dummy variable "total-cost" is created during translate
    check_magic(in, "end_metric");
}

void read_variables(istream &in, vector<Variable> &internal_variables,
                    vector<Variable *> &variables) {
    int count;
    in >> count;
    internal_variables.reserve(count);
    // Important so that the iterators stored in variables are valid.
    for (int i = 0; i < count; i++) {
        internal_variables.push_back(Variable(in));
        variables.push_back(&internal_variables.back());
    }
}

void read_numeric_variables(istream &in, vector<NumericVariable> &internal_numeric_variables,
                    vector<NumericVariable *> &numeric_variables) {
    int count;
    in >> count;
    internal_numeric_variables.reserve(count);
    check_magic(in, "begin_numeric_variables");
    // Important so that the iterators stored in variables are valid.
    for (int i = 0; i < count; i++) {
    	NumericVariable next_var = NumericVariable(in);
        internal_numeric_variables.push_back(next_var);
        numeric_variables.push_back(&internal_numeric_variables.back());
    }
    check_magic(in, "end_numeric_variables");
}

void read_mutexes(istream &in, vector<MutexGroup> &mutexes,
                  const vector<Variable *> &variables) {
    size_t count;
    in >> count;
    for (size_t i = 0; i < count; ++i)
        mutexes.push_back(MutexGroup(in, variables));
}

void read_global_constraint(istream &in, const vector<Variable *> &variables, GlobalConstraint &gc) {
	check_magic(in, "begin_global_constraint");
	int index;
	int val;
	in >> index >> val;
	gc.var = variables[index];
	gc.val = val;
	cout << "read global constraint at var " << index << " value " << val << endl;
	gc.var->dump();
	check_magic(in, "end_global_constraint");
}

void read_goal(istream &in, const vector<Variable *> &variables,
               vector<pair<Variable *, int>> &goals) {
    check_magic(in, "begin_goal");
    int count;
    in >> count;
    for (int i = 0; i < count; i++) {
        int varNo, val;
        in >> varNo >> val;
        goals.push_back(make_pair(variables[varNo], val));
    }
    check_magic(in, "end_goal");
}

void dump_goal(const vector<pair<Variable *, int>> &goals) {
    cout << "Goal Conditions:" << endl;
    for (const auto &goal : goals)
        cout << "  " << goal.first->get_name() << ": "
             << goal.second << endl;
}

void read_operators(istream &in, const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables,
                    vector<Operator> &operators) {
    int count;
    in >> count;
    for (int i = 0; i < count; i++)
        operators.push_back(Operator(in, variables, numeric_variables));
}

struct axiom_rel_comparator
{
    bool operator()(const Axiom_relational& key1, const Axiom_relational& key2) const
    {
        return key1.get_effect_var()->get_layer() < key2.get_effect_var()->get_layer();
    }
};

void read_axioms_rel(istream &in, const vector<Variable *> &variables,
        vector<Axiom_relational> &axioms_rel) {
    int count;
    in >> count;
    for (int i = 0; i < count; i++) {
        axioms_rel.push_back(Axiom_relational(in, variables));
    }
//    if (DEBUG) {
//    	cout << "reading axioms on the following variables:" << endl;
//    	for(const Axiom_relational &axiom: axioms_rel) {
//    		cout << "axiom effect in layer " << axiom.get_effect_var()->get_layer() << endl;
//    	}
//    }
    std::sort(axioms_rel.begin(), axioms_rel.end(), axiom_rel_comparator());
//    if (DEBUG) {
//    	cout << "after sorting:" << endl;
//    	for(const Axiom_relational axiom: axioms_rel) {
//    	    		cout << axiom.get_effect_var()->get_layer() << endl;
//    	}
//    }

}

struct axiom_fc_comparator
{
    bool operator()(const Axiom_functional_comparison& key1, const Axiom_functional_comparison& key2) const
    {
        return key1.get_effect_var()->get_layer() < key2.get_effect_var()->get_layer();
    }
};


void read_axioms_func_comp(istream &in, const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables,
        vector<Axiom_functional_comparison> &axioms_func_comp) {
    int count;
    in >> count;
    check_magic(in, "begin_comparison_axioms");
    for(int i = 0; i < count; i++) {
        axioms_func_comp.push_back(Axiom_functional_comparison(in, variables, numeric_variables));
    }
    check_magic(in, "end_comparison_axioms");
//    if (DEBUG) {
//    	cout << "reading axioms on the following variables:" << endl;
//    	for(const Axiom_functional_comparison &axiom : axioms_func_comp) {
//    		cout << "read comparison axiom with index " << axiom.get_effect_var()->get_level() << " ("<< axiom.get_effect_var()->get_name() << ")"<<endl;
//    	}
//    }
    std::sort(axioms_func_comp.begin(), axioms_func_comp.end(), axiom_fc_comparator());
}

struct axiom_na_comparator
{
    bool operator()(const Axiom_numeric_computation& key1, const Axiom_numeric_computation& key2) const
    {
        return key1.get_effect_var()->get_layer() < key2.get_effect_var()->get_layer();
    }
};


void read_axioms_numeric(istream &in, const vector<NumericVariable *> &numeric_variables,
        vector<Axiom_numeric_computation> &axioms_numeric) {
    int count;
    in >> count;
    check_magic(in, "begin_numeric_axioms");
    for(int i = 0; i < count; i++)
        axioms_numeric.push_back(Axiom_numeric_computation(in, numeric_variables));
    check_magic(in, "end_numeric_axioms");
//    if (DEBUG) {
//    	cout << "reading numeric axioms on the following variables:" << endl;
//    	for(Axiom_numeric_computation &axiom : axioms_numeric) {
//    		cout << axiom.get_effect_var()->get_level() << " ("<< axiom.get_effect_var()->get_name() << ")"<<endl;
//    	}
//    }
    std::sort(axioms_numeric.begin(), axioms_numeric.end(), axiom_na_comparator());
}

void read_preprocessed_problem_description(istream &in,
                                           Metric &metric,
                                           vector<Variable> &internal_variables,
                                           vector<Variable *> &variables,
                                           vector<NumericVariable> &internal_numeric_variables,
                                           vector<NumericVariable *> &numeric_variables,
                                           vector<MutexGroup> &mutexes,
                                           State &initial_state,
                                           vector<pair<Variable *, int>> &goals,
                                           vector<Operator> &operators,
                                           vector<Axiom_relational> &axioms_rel,
                                           vector<Axiom_numeric_computation> &axioms_func_ass,
                                           vector<Axiom_functional_comparison> &axioms_func_comp,
										   GlobalConstraint &gconstraint) {
	if (DEBUG) cout << "reading version..." << endl;
    read_and_verify_version(in);
    if (DEBUG) cout << "reading metric..." << endl;
    read_metric(in, metric);
    if (DEBUG) cout << "reading variables..." << endl;
    read_variables(in, internal_variables, variables);
    if (DEBUG) cout << "reading numeric variables..." << endl;
    read_numeric_variables(in, internal_numeric_variables, numeric_variables);
    if (DEBUG) cout << "reading mutexes..." << endl;
    read_mutexes(in, mutexes, variables);
    if (DEBUG) cout << "reading initial state..." << endl;
    initial_state = State(in, variables, numeric_variables);
    read_goal(in, variables, goals);
    if (DEBUG) cout << "reading operators..." << endl;
    read_operators(in, variables, numeric_variables, operators);
    if (DEBUG) cout << "reading propositional axioms..." << endl;
    read_axioms_rel(in, variables, axioms_rel);
    if (DEBUG) cout << "reading functional comparison axioms..." << endl;
    read_axioms_func_comp(in, variables, numeric_variables, axioms_func_comp);
    if (DEBUG) cout << "reading functional assignment axioms..." << endl;
    read_axioms_numeric(in, numeric_variables, axioms_func_ass);
    if (DEBUG) cout << "reading global constraint" << endl;
    read_global_constraint(in, variables, gconstraint);

//    if (DEBUG) {
//    	cout << "Dumping Variables" << endl;
//    	for (Variable* var : variables) {
//    		var->dump();
//    		for (int i=0; i < var->get_range(); ++i) {
//    			cout << "  " << i << " = " << var->get_fact_name(i) << endl;
//    		}
//    		cout << "Initial value: " << initial_state[var] << endl;
//    	}
//    }


}

void dump_preprocessed_problem_description(const vector<Variable *> &variables,
                                           const State &initial_state,
                                           const vector<pair<Variable *, int>> &goals,
                                           const vector<Operator> &operators,
                                           const vector<Axiom_relational> &axioms_rel,
                                           const vector<Axiom_numeric_computation> &axioms_func_ass,
                                           const vector<Axiom_functional_comparison> &axioms_func_comp) {
    cout << "Variables (" << variables.size() << "):" << endl;
    for (Variable *var : variables)
        var->dump();

    cout << "Initial State:" << endl;
    initial_state.dump();
    dump_goal(goals);

    for (const Operator &op : operators)
        op.dump();
    for (const Axiom_relational &axiom_rel : axioms_rel)
        axiom_rel.dump();
    for (const Axiom_numeric_computation &axiom_ass : axioms_func_ass)
        axiom_ass.dump();
    for (const Axiom_functional_comparison &axiom_comp : axioms_func_comp)
        axiom_comp.dump();
}

void dump_DTGs(const vector<Variable *> &ordering,
               vector<DomainTransitionGraph> &transition_graphs) {
    int num_graphs = transition_graphs.size();
    for (int i = 0; i < num_graphs; i++) {
        cout << "Domain transition graph for " << ordering[i]->get_name() << ":" << endl;
        transition_graphs[i].dump();
    }
}

void generate_cpp_input(const string in_file,
                        bool /*solvable_in_poly_time*/,
                        const vector<Variable *> &ordered_vars,
                        const vector<NumericVariable *> &numeric_vars,
                        const Metric &metric,
                        const vector<MutexGroup> &mutexes,
                        const State &initial_state,
                        const vector<pair<Variable *, int>> &goals,
                        const vector<Operator> &operators,
                        const vector<Axiom_relational> &axioms_rel,
                        const vector<Axiom_numeric_computation> &axioms_func_ass,
                        const vector<Axiom_functional_comparison> &axioms_func_comp,
						const GlobalConstraint &constraint,
                        const SuccessorGenerator &sg,
                        const vector<DomainTransitionGraph> transition_graphs,
                        const CausalGraph &cg) {
    /* NOTE: solvable_in_poly_time flag is no longer included in output,
       since the planner doesn't handle it specially any more anyway. */

    ofstream outfile;
    outfile.open(in_file + ".num", ios::out);

    outfile << in_file + ".num" << endl;
    outfile << "begin_version" << endl;
    outfile << PRE_FILE_VERSION << endl;
    outfile << "end_version" << endl;

    outfile << "begin_metric" << endl;
    outfile << metric.optimization_criterion << " " << metric.index << endl;
    outfile << "end_metric" << endl;

    int num_vars = ordered_vars.size();
    outfile << num_vars << endl;
    if (DEBUG) cout << "Variables in output are: " << endl;
    for (Variable *var : ordered_vars) {
        var->generate_cpp_input(outfile);
        if(DEBUG) {
        	cout << var->get_name() << " {";
        	for (int i=0; i<var->get_range(); ++i) {
        		cout << var->get_fact_name(i) << ", ";
        	}
        	cout << "}" << endl;
        	cout << "Initial value = " <<initial_state[var] << endl;
        }
    }
    if (DEBUG) cout << "Numeric Variables in output are: " << endl;
    outfile << numeric_vars.size() << endl;
    outfile << "begin_numeric_variables" << endl;
    for (const NumericVariable *numeric_var : numeric_vars) {
    	if (DEBUG) numeric_var->dump();
    	numeric_var->generate_cpp_input(outfile);
    }
    outfile << "end_numeric_variables" << endl;

    outfile << mutexes.size() << endl;
    for (const MutexGroup &mutex : mutexes)
        mutex.generate_cpp_input(outfile);

    //int var_count = ordered_vars.size();
    outfile << "begin_state" << endl;
    for (Variable *var : ordered_vars)
        outfile << initial_state[var] << endl;  // for axioms default value
    outfile << "end_state" << endl;
    outfile << "begin_numeric_state" << endl;
    for (auto numvar : numeric_vars) {
//    	if (DEBUG) cout << "writing initial state of variable " << numvar->get_name() << " -> ";
//    	if (DEBUG) cout <<  initial_state.get_nv(numvar) << endl;
        outfile << initial_state.get_nv(numvar) << endl;
    }
    outfile << "end_numeric_state" << endl;


    vector<int> ordered_goal_values;
    ordered_goal_values.resize(num_vars, -1);
    for (const auto &goal : goals) {
        int var_index = goal.first->get_level();
        ordered_goal_values[var_index] = goal.second;
    }
    outfile << "begin_goal" << endl;
    outfile << goals.size() << endl;
    for (int i = 0; i < num_vars; i++)
        if (ordered_goal_values[i] != -1)
            outfile << i << " " << ordered_goal_values[i] << endl;
    outfile << "end_goal" << endl;

    outfile << operators.size() << endl;
    for (const Operator &op : operators)
        op.generate_cpp_input(outfile);

    outfile << axioms_rel.size() << endl;
    for (const Axiom_relational &axiom : axioms_rel)
        axiom.generate_cpp_input(outfile);

    outfile << axioms_func_comp.size() << endl;
    outfile << "begin_comparison_axioms" << endl;
    for (const Axiom_functional_comparison &axiom : axioms_func_comp)
        axiom.generate_cpp_input(outfile);
    outfile << "end_comparison_axioms" << endl;

    outfile << axioms_func_ass.size() << endl;
    outfile << "begin_numeric_axioms" << endl;
    for (const Axiom_numeric_computation &axiom : axioms_func_ass)
        axiom.generate_cpp_input(outfile);
    outfile << "end_numeric_axioms" << endl;

    outfile << "begin_global_constraint" << endl;
    outfile << constraint.var->get_level() << " " << constraint.val << endl;
    outfile << "end_global_constraint" << endl;

    outfile << "begin_SG" << endl;
    sg.generate_cpp_input(outfile);
    outfile << "end_SG" << endl;

    for (const auto &dtg : transition_graphs) {
        outfile << "begin_DTG" << endl;
        dtg.generate_cpp_input(outfile);
        outfile << "end_DTG" << endl;
    }

    outfile << "begin_CG" << endl;
    cg.generate_cpp_input(outfile, ordered_vars);
    outfile << "end_CG" << endl;

    outfile.close();
}

istream& operator>>(istream &is, foperator &fop) {
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

ostream& operator<<(ostream &os, const foperator &fop) {
    switch (fop) {
        case assign:
            os << "=";
            break;
        case scale_up:
            os << "*";
            break;
        case scale_down:
            os << "/";
            break;
        case increase:
            os << "+";
            break;
        case decrease:
            os << "-";
            break;
        default:
            cout << (int)fop << " was read" << endl;
            assert(false);
    }
    return os;
}

istream& operator>>(istream &is, compoperator &cop) {
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

ostream& operator<<(ostream &os, const compoperator &cop) {
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
    }
    return os;
}

void stringify(compoperator cop, string& op_string, string& reverse_op_string) {
	switch (cop) {
	        case lt:
	            op_string = "<";
	            reverse_op_string = ">=";
	            return;
	        case le:
	            op_string = "<=";
	            reverse_op_string = ">";
	            return;
	        case eq:
	            op_string = "=";
	            reverse_op_string = "!=";
	            return;
	        case ge:
	            op_string = ">=";
	            reverse_op_string = "<";
	            return;
	        case gt:
	            op_string = ">";
	            reverse_op_string = "<=";
	            return;
	        case ue:
	            op_string = "!=";
	            reverse_op_string = "=";
	            return;
	        default:
	            cout << cop << " WAS READ" << endl;
	            assert(false);
	    }
	    return;
}

void check_and_repair_empty_axiom_layers(
		const vector<NumericVariable*>& numeric_variables,
		const vector<Variable*>& variables) {
	int max_num_index_before = -1;
	int max_num_index_after = -1;
	for (auto nvar : numeric_variables) {
		max_num_index_before = max(max_num_index_before, nvar->get_layer());
		if (nvar->is_necessary()) {
			max_num_index_after = max(max_num_index_after, nvar->get_layer());
		}
	}
	if (max_num_index_before != max_num_index_after) {
		if (DEBUG) cout << "index before = " << max_num_index_before << " after = " << max_num_index_after << endl;
		for (auto var : variables) {
			int decrement = max_num_index_before - max_num_index_after;
			var->decrement_layer(decrement);
			assert(var->get_layer()==-1 || var->get_layer() > max_num_index_after);
		}
	}
}
