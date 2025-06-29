#ifndef CAUSAL_GRAPH_H
#define CAUSAL_GRAPH_H

#include <iosfwd>
#include <vector>
#include <map>
using namespace std;

class Operator;
class Axiom_relational;
class Axiom_numeric_computation;
class Axiom_functional_comparison;
class Variable;
class NumericVariable;
class GlobalConstraint;

struct CGVar { // slightly more sophisticated than the Variable pointers used in the original
			   // version. The CGVar handles the pointers to either a FiniteDomain or a Numeric variable
			   // and offers some helper functions to easily access often used features
	Variable* var;
	NumericVariable* nvar;
	bool numeric;
	CGVar(Variable * var): var(var), nvar(0), numeric(false) {};
	CGVar(NumericVariable * nvar): var(0), nvar(nvar), numeric(true) {};
	string get_name() const;
	void set_necessary();
	bool is_necessary() const;
	bool operator==(const CGVar &other) const;
	bool operator!=(const CGVar &other) const {return !((*this)==other);};
	bool operator<(const CGVar &other) const;
};


class CausalGraph {
    const vector<Variable *> &variables;
    const vector<NumericVariable *> &numeric_variables;
    const vector<Operator> &operators;
    const vector<Axiom_relational> &axioms;
    const vector<Axiom_numeric_computation> &ass_axioms;
    const vector<Axiom_functional_comparison> &comp_axioms;
    const vector<pair<Variable *, int> > &goals;
    const GlobalConstraint &global_constraint;
    NumericVariable * metric_var;

    typedef map<CGVar, int> WeightedSuccessors;
    typedef map<CGVar, WeightedSuccessors> WeightedGraph;
    WeightedGraph weighted_graph;
    typedef map<CGVar, int> Predecessors;
    typedef map<CGVar, Predecessors> PredecessorGraph;
    // predecessor_graph is weighted_graph with edges turned around
    PredecessorGraph predecessor_graph;

    typedef vector<vector<CGVar> > Partition;
    typedef vector<CGVar> Ordering;
    Ordering ordering;
    vector<Variable *> propositional_ordering;
    vector<NumericVariable *> numeric_ordering;
    bool acyclic;

    void weigh_graph_from_ops(const vector<Variable *> &variables,
    				          const vector<NumericVariable *> &numeric_variables,
                              const vector<Operator> &operators,
                              const vector<pair<Variable *, int> > &goals);
    void weigh_graph_from_axioms(const vector<Variable *> &variables,
                                 const vector<Axiom_relational> &axioms,
                                 const vector<pair<Variable *, int> > &goals);
    void weigh_graph_from_comp_axioms(const vector<Axiom_functional_comparison> &comp_axioms);
    void weigh_graph_from_ass_axioms(const vector<Axiom_numeric_computation> &ass_axioms);

    void get_strongly_connected_components(Partition &sccs);
    void calculate_topological_pseudo_sort(const Partition &sccs);
    void calculate_important_vars();
    void dfs(CGVar from);
    void set_variable_instrumentation_necessary(NumericVariable* inst_var);
public:
    CausalGraph(const vector<Variable *> &the_variables,
    			const vector<NumericVariable *> &the_numeric_variables,
                const vector<Operator> &the_operators,
                const vector<Axiom_relational> &the_axioms,
                const vector<Axiom_numeric_computation> &the_ass_axioms,
                const vector<Axiom_functional_comparison> &the_comp_axioms,
                const vector<pair<Variable *, int> > &the_goals,
				const GlobalConstraint &the_global_constraint,
                NumericVariable * metric_var);
    ~CausalGraph() {}
    int get_metric_index() const;
    const vector<Variable *> &get_variable_ordering() const;
    const vector<NumericVariable *> &get_numeric_variable_ordering() const;
    bool is_acyclic() const;
    void dump() const;
    void generate_cpp_input(ofstream &outfile,
                            const vector<Variable *> &ordered_vars) const;
};

extern bool g_do_not_prune_variables;

#endif
