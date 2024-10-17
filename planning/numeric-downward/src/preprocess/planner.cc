/* Main file, keeps all important variables.
 * Calls functions from "helper_functions" to read in input (variables, operators,
 * goals, initial state),
 * then calls functions to build causal graph, domain_transition_graphs and
 * successor generator
 * finally prints output to file "output"
 */

#include "helper_functions.h"
#include "successor_generator.h"
#include "causal_graph.h"
#include "domain_transition_graph.h"
#include "state.h"
#include "mutex_group.h"
#include "operator.h"
#include "axiom.h"
#include "variable.h"
#include <iostream>
#include <sstream>
#include <cassert>
using namespace std;

int main(int argc, const char **argv) {
    Metric  metric;
    vector<Variable *> variables; // variables ordered beneficially (according to causal graph) without pruned variables
    vector<Variable> internal_variables; // includes variables that will be pruned away, ordered according to input file
    vector<NumericVariable *> numeric_variables;
    vector<NumericVariable> internal_numeric_variables;
    State initial_state;
    vector<pair<Variable *, int>> goals;
    vector<MutexGroup> mutexes;
    vector<Operator> operators;
    vector<Axiom_relational> axioms_rel;
    vector<Axiom_numeric_computation> axioms_numeric;
    vector<Axiom_functional_comparison> axioms_func_comp;
    vector<DomainTransitionGraph> transition_graphs;
    GlobalConstraint global_constraint;

    /**
     * The following block generates a stream that will be further processed.
     * preprocess can either be executed with a file name as argument (e.g. ./preprocess output.sas)
     * or the content of the file can be piped over stdin (e.g. ./preprocess < output.sas)
     * In both cases the stream "result" will contain the relevant informations.
     */
	string line;
	stringstream result;
  string in_file = "_none_";
    if (argc == 2) {
        ifstream file_content;
        in_file = argv[1];
    	cout << "opening file " << in_file << endl;
    	file_content.open(in_file);
    	while(getline(file_content, line)) {
        	result << line << endl;
        }
    	argc--;
    } else {
    	while(getline(cin, line)) {
        	result << line << endl;
        }
    }

    if (argc != 1) {
        cout << "*** do not perform relevance analysis ***" << endl;
        g_do_not_prune_variables = true;
    }

    read_preprocessed_problem_description(result, metric, internal_variables, variables,
        		internal_numeric_variables, numeric_variables, mutexes, initial_state, goals,
        		operators, axioms_rel, axioms_numeric, axioms_func_comp, global_constraint);
    //dump_preprocessed_problem_description
    //  (variables, initial_state, goals, operators, axioms);

    cout << "Building causal graph..." << endl;
    CausalGraph causal_graph(variables, numeric_variables, operators, axioms_rel, axioms_numeric, axioms_func_comp, goals, global_constraint, numeric_variables[metric.index]);
    const vector<Variable *> &ordering = causal_graph.get_variable_ordering();
    const vector<NumericVariable *> &numeric_ordering = causal_graph.get_numeric_variable_ordering();

    check_and_repair_empty_axiom_layers(numeric_variables, ordering);

    int old_metric_index = metric.index;
    metric.index = causal_graph.get_metric_index();
    if(DEBUG) cout << "Metric index changed from " << old_metric_index << " to " << metric.index << endl;

    bool cg_acyclic = causal_graph.is_acyclic();

    // Remove unnecessary effects from operators and axioms, then remove
    // operators and axioms without effects.
    strip_mutexes(mutexes);
    strip_operators(operators);
    strip_axiom_relationals(axioms_rel);
    strip_axiom_functional_comparisons(axioms_func_comp);
    strip_axiom_functional_assignment(axioms_numeric);

    cout << "Building domain transition graphs..." << endl;
    build_DTGs(ordering, operators, axioms_rel, transition_graphs);
    //dump_DTGs(ordering, transition_graphs);
    bool solveable_in_poly_time = false;
    if (cg_acyclic)
        solveable_in_poly_time = are_DTGs_strongly_connected(transition_graphs);
    /*
      TODO: The above test doesn't seem to be quite ok because it
      ignores axioms and it ignores non-unary operators. (Note that the
      causal graph computed here does *not* contain arcs between
      effects, only arcs from preconditions to effects.)

      So solveable_in_poly_time [sic] should also be set to false if
      there are any derived variables or non-unary operators.
     */

    //TODO: genauer machen? (highest level var muss nicht scc sein...gemacht)
    //nur Werte, die wichtig sind fuer drunterliegende vars muessen in scc sein
    cout << "solveable in poly time " << solveable_in_poly_time << endl;
    cout << "Building successor generator..." << endl;
    SuccessorGenerator successor_generator(ordering, operators);
    //successor_generator.dump();

    // Output some task statistics
    int facts = 0;
    int derived_vars = 0;
    for (Variable *var : ordering) {
        facts += var->get_range();
        if (var->is_derived())
            derived_vars++;
    }
    cout << "Preprocessor facts: " << facts << endl;
    cout << "Preprocessor derived variables: " << derived_vars << endl;

    // Calculate the problem size
    int task_size = ordering.size() + facts + goals.size();

    for (const MutexGroup &mutex : mutexes)
        task_size += mutex.get_encoding_size();

    for (const Operator &op : operators)
        task_size += op.get_encoding_size();

    for (const Axiom_relational &axiom : axioms_rel)
        task_size += axiom.get_encoding_size();

    for (const Axiom_numeric_computation &axiom : axioms_numeric)
        task_size += axiom.get_encoding_size();

    for (const Axiom_functional_comparison &axiom : axioms_func_comp)
        task_size += axiom.get_encoding_size();

    cout << "Preprocessor task size: " << task_size << endl;

    cout << "Writing output..." << endl;
    generate_cpp_input(in_file, solveable_in_poly_time, ordering, numeric_ordering, 
             metric, mutexes, initial_state, goals, operators, axioms_rel,
					   axioms_numeric, axioms_func_comp, global_constraint,
					   successor_generator, transition_graphs, causal_graph);
    cout << "done" << endl;

    std::remove(in_file.c_str());
    if (!std::ifstream{in_file}){ // uses operator! of temporary stream object
        cout << "Removed tmp file " << in_file << endl;
    }

//    cout << "-----------------------------------------------\n Eliminated Variables : \n--------------------------------------------------" << endl;
//    int vcount = 0;
//    for (const auto &var : internal_variables) {
//    	if (!var.is_necessary()) {
//    		cout << "Variable eliminated (" << ++vcount << ") ";
//    		var.dump();
//    	}
//    }
//    cout << " ------------------- numeric : -----------------" << endl;
//    vcount = 0;
//    for (const auto &var : internal_numeric_variables) {
//    	if (!var.is_necessary()) {
//    		cout << "NumericVariable eliminated (" << ++vcount << ") ";
//    		var.dump();
//    	}
//    }

}
