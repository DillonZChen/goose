#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "state.h"
#include "variable.h"
#include "successor_generator.h"
#include "causal_graph.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class State;
class MutexGroup;
class Operator;
class Axiom_relational;
class Axiom_functional_comparison;
class Axiom_numeric_computation;
class DomainTransitionGraph;

static const bool DEBUG = false;

struct Metric {
	char optimization_criterion; // '<' : minimize, '>' : maximize
	int index; // index of the numeric fluent (possible a derived variable) to be optimized
};

struct GlobalConstraint {
	Variable* var; // pointer to the variable storing the derived global constraint axiom result
	int val; // index of the value that the variable has to attain
};

//void read_everything
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
										   GlobalConstraint &gconstraint);

//void dump_everything
void dump_preprocessed_problem_description(const vector<Variable *> &variables,
                                           const State &initial_state,
                                           const vector<pair<Variable *, int>> &goals,
                                           const vector<Operator> &operators,
                                           const vector<Axiom_relational> &axioms_rel,
                                           const vector<Axiom_numeric_computation> &axioms_func_ass,
                                           const vector<Axiom_functional_comparison> &axioms_func_comp);

void dump_DTGs(const vector<Variable *> &ordering,
               vector<DomainTransitionGraph> &transition_graphs);
void generate_cpp_input(const string in_file,
                        bool causal_graph_acyclic,
                        const vector<Variable *> &ordered_var,
                        const vector<NumericVariable *> &numeric_var,
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
                        const CausalGraph &cg);
void check_magic(istream &in, string magic);

enum foperator
{
    assign = 0, scale_up = 1, scale_down = 2, increase = 3, decrease = 4
};
enum compoperator
{
    lt = 0, le = 1, eq = 2, ge = 3, gt = 4, ue = 5
};

istream& operator>>(istream &is, foperator &fop);

ostream& operator<<(ostream &os, const foperator &fop);

istream& operator>>(istream &is, compoperator &cop);

ostream& operator<<(ostream &os, const compoperator &cop);

void stringify(compoperator cop, string &op_string, string &reverse_op_string);

void check_and_repair_empty_axiom_layers(const vector<NumericVariable *> &numeric_variables, const vector<Variable *> &variables);

#endif
