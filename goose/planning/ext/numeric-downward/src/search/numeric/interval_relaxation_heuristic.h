#ifndef INTERVAL_RELAXATION_HEURISTIC_H
#define INTERVAL_RELAXATION_HEURISTIC_H

#include "../heuristic.h"
#include "interval.h"
#include <vector>
#include <list>

class FactProxy;
class GlobalState;
class OperatorProxy;

namespace interval_relaxation_heuristic {
struct NumericState;
struct UnaryOperator;
struct Proposition;
struct UnaryEffect;

struct UnaryEffect {
	int aff_variable_index;
	f_operator assign_type;
	int val_or_ass_var_index;
	bool numeric;
	std::string str();
	UnaryEffect(int variable, int value)
		: aff_variable_index(variable), assign_type(assign), val_or_ass_var_index(value), numeric(false) {}
	UnaryEffect(int aff_var, f_operator assignment_type, int ass_var)
		: aff_variable_index(aff_var), assign_type(assignment_type), val_or_ass_var_index(ass_var), numeric(true) {}
};

struct UnaryOperator {
    int operator_no; // -1 for axioms; index into g_operators otherwise
    cal_operator ass_ax_op; // only for assignment axioms
    comp_operator comp_ax_op; // only for comparison axioms
    std::vector<Proposition *> precondition;
    int axiom_left_var; // "precondition" for axioms
    int axiom_right_var; // "precondition" for axioms
    UnaryEffect effect;
    bool is_numeric_axiom;
    ap_float base_cost;
    int unsatisfied_preconditions;
    ap_float precondition_cost; // Used for h^max cost or h^add cost;
              // DOES NOT include operator cost (base_cost)
    std::string str();
    ap_float cost() {return base_cost + precondition_cost;}
    bool is_numeric_operator() {return !is_numeric_axiom && effect.numeric;}
    bool is_assignment_axiom() {return is_numeric_axiom && effect.numeric;}
    bool is_comparison_axiom() {return is_numeric_axiom && !effect.numeric;}
    UnaryOperator(int operator_no_, const std::vector<Proposition *> &pre, UnaryEffect eff,
                  ap_float base) // regular operator constructor
        : operator_no(operator_no_), ass_ax_op(sum), comp_ax_op(ue), precondition(pre),
		  axiom_left_var(-1), axiom_right_var(-1), effect(eff), is_numeric_axiom(false),
          base_cost(base), unsatisfied_preconditions(precondition.size()), precondition_cost(0) {}
    UnaryOperator(int l, int r, cal_operator ao, int eff): // assignment axiom constructor
    	operator_no(-1), ass_ax_op(ao), comp_ax_op(ue), precondition(std::vector<Proposition *>()),
    	axiom_left_var(l), axiom_right_var(r), effect(UnaryEffect(eff, assign, eff)), is_numeric_axiom(true),
		base_cost(0), unsatisfied_preconditions(0), precondition_cost(0) {}
    UnaryOperator(int l, int r, comp_operator co, int eff_var, int eff_val): // comparison axiom constructor
    	operator_no(-1), ass_ax_op(sum), comp_ax_op(co), precondition(std::vector<Proposition *>()),
		axiom_left_var(l), axiom_right_var(r), effect(UnaryEffect(eff_var, eff_val)), is_numeric_axiom(true),
		base_cost(0), unsatisfied_preconditions(0), precondition_cost(0) {}
};

struct Proposition {
    bool is_goal;
    int id;
    std::vector<UnaryOperator *> precondition_of;
    ap_float cost; // Used for h^max cost or h^add cost
    UnaryOperator *reached_by;
    int reached_in_layer;
    bool marked; // used when computing preferred operators for h^add and h^FF
    Proposition(int id_) {
        id = id_;
        is_goal = false;
        cost = INF;
        reached_by = 0;
        reached_in_layer =  -1;
        marked = false;
    }
};

class NumericState {
	std::vector<Interval> vals;
	std::vector<std::vector<UnaryOperator *> > achievers; // all operators that extended the interval in the last step
	std::vector<ap_float> costs;
public:
	void new_val_for(size_t index, Interval new_val, UnaryOperator *achiever, ap_float cost);
	Interval get_val(size_t index) const {return vals[index];}
	ap_float get_cost(size_t index) const {return costs[index];}
	std::vector<UnaryOperator *> &get_achievers(size_t index) {return achievers[index];}
	NumericState(const State &state);
	NumericState(std::vector<Interval> _vals,
			std::vector<std::vector<UnaryOperator *> > _achievers,
			std::vector<ap_float> _costs) : vals(_vals), achievers(_achievers), costs(_costs) {}
	NumericState duplicate();
	size_t size() {return vals.size();}
	void dump();
};

class IntervalRelaxationHeuristic : public Heuristic {
    void build_unary_operators(const OperatorProxy &op, int operator_no);
    void build_unary_axioms(const OperatorProxy &ax, int operator_no);
	void build_unary_comparison_axioms(const ComparisonAxiomProxy &ax);
	void build_unary_assignment_axioms(const AssignmentAxiomProxy &ax);
protected:
    std::vector<UnaryOperator> unary_operators;
    std::vector<UnaryOperator> numeric_axioms; // ordered by layer
    std::vector<UnaryOperator> unary_axioms;
    std::vector<std::vector<Proposition>> propositions;
    std::vector<Proposition *> goal_propositions;
    std::vector<NumericState> planning_graph;
    std::list<UnaryOperator *> applicable_operators;
    std::list<UnaryOperator *> applicable_axioms;
    std::vector<std::string> debug_fact_names;
	void setup_exploration(const State &state);
	void relaxed_exploration();
    Proposition *get_proposition(const FactProxy &fact);
	UnaryEffect get_effect(EffectProxy effect);
	UnaryEffect get_effect(AssEffectProxy num_effect);
	void handle_prop(Proposition * prop, ap_float distance, int layer, UnaryOperator* achiever, int &missinggoals);
    virtual void initialize();
    virtual ap_float compute_heuristic(const GlobalState &state) = 0;
    virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) = 0;
public:
	IntervalRelaxationHeuristic(const options::Options &options);
	virtual ~IntervalRelaxationHeuristic();
	virtual bool dead_ends_are_reliable() const;
};
}

#endif
