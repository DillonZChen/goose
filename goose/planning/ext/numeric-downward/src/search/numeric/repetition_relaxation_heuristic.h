#ifndef REPETITION_RELAXATION_HEURISTIC_H
#define REPETITION_RELAXATION_HEURISTIC_H

#include <map>
#include <limits>
#include "../heuristic.h"
#include "interval.h"
#include "../globals.h" // f_operator
#include "../priority_queue.h"
//#include "axioms.h"

namespace repetition_relaxation_heuristic {

class UnaryOperator;

enum operatorType {
	logic_op = 0, numeric_op = 1, comp_axiom = 2, ass_axiom = 3, dummy = 4
};

extern int global_achiever_ordering_rank;

//bool interval_contains(const Interval& interval, ap_float target);

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

struct IProposition {
    bool is_goal;
    int id;
    std::vector<UnaryOperator *> precondition_of;
    ap_float cost; // Used for h^max cost or h^add cost
    UnaryOperator *reached_by;
    int exploration_index; // exploration index at enqueue time that makes comparison axiom true
    bool marked; // used when computing preferred operators for h^add and h^FF
    IProposition(int id_) {
        id = id_;
        is_goal = false;
        cost = INF;
        reached_by = 0;
		exploration_index = -1;
        marked = false;
//        std::cout << "constructed proposition with id " << id << std::endl;
    }
};

struct NumericAchiever {
	UnaryOperator *reached_by;
	Interval reached_interval;
	int enque_pos; // the "timepoint" at which the Achiever was enqueue
	int order_pos; // only used for debugging to secure termination of FF
	int repetitions;
	ap_float cost;

	NumericAchiever(UnaryOperator *the_reached_by, Interval the_reached_interval, int the_enqueue_id,
				int the_repetitions, ap_float the_cost) :
				reached_by(the_reached_by), // Every time the NSV gets a new value, the enabling Operator is stored here.
				reached_interval(the_reached_interval),  // the interval after applying the operator in reached_by
				enque_pos(the_enqueue_id),
				order_pos(++global_achiever_ordering_rank),
				repetitions(the_repetitions), // corresponds to the "marked" attribute of proposition. The number of repetitions that should be
			    // applied for the corresponding operator in reached_by
				cost(the_cost)
				{
//		if (the_reached_from_interval.left_open) std::cout << "creating numeric achiever LEFTOPEN" << std::endl;
		assert(order_pos < std::numeric_limits<int>::max()); // simple overflow check
	} // the cost to achieve the interval (required by h_max)
};

class NumericStateVariable {
	int id; // corresponds to the global variable index
	std::set<NumericStateVariable *> topology_parents; // variables that this depends on
	std::set<NumericStateVariable *> topology_children; // variables that are dependant from this
	int topology_level;
	bool blocked; // this variable must wait for topologically higher operators to finish their work
	int missing_depending_applicable_op_count; // number of operators that are currently working on this +
	// number of blocked topology_parents
	std::vector<NumericAchiever> achievers;
	Interval init;
	Interval max_iv; // h_max stores the maximally obtainable interval here to break topology dependencies in the second iteration
	void block();
	void free();
public:
	std::vector<UnaryOperator *> precondition_of;  //  those Operators are either Axioms or have this variable in its assignment
	size_t top_parent_size() {return topology_parents.size();};
	std::set<NumericStateVariable *> &get_depending_variables(); // variables that are dependant from this
	std::set<NumericStateVariable *> &get_topology_parents();
	int get_id();
	void tblock(); // topologically block this variable
	bool tfree(); // topologically unblock this variable
	bool is_blocked() {return blocked;};
	void set_topology(int level);
	int get_topology();
	void cleanup_and_init(ap_float value); // clear achievers and reset depending operators
	void achieved_by(NumericAchiever achiever);
	void depends_on(NumericStateVariable* other);
	void set_max_to_current_val() {max_iv = val();}
//	std::vector<NumericAchiever>& get_achievers();
	void dump();
	ap_float cost();
	Interval val();
	Interval max_val();
	Interval val_at_cost(ap_float cost);
	Interval val_at_id(int id);
	int index_at_id(int id);
	NumericAchiever* get_achiever(int index);
	NumericAchiever* get_best_achiever(ap_float target);
	std::string str();
	NumericStateVariable(int id_) : id (id_) {
		assert((int) g_initial_state_numeric.size() > id);
		init = Interval(g_initial_state_numeric[id]);
		max_iv = init;
		missing_depending_applicable_op_count = 0;
		topology_parents = std::set<NumericStateVariable *>();
		topology_children = std::set<NumericStateVariable *>();
		topology_level = -1;
		blocked = false;
		achievers = std::vector<NumericAchiever>();
	};
};

// Operator action only on a single variable
struct UnaryOperator { // comparison axiom, assignment axiom, classic axiom, classic operator or numeric operator
    int operator_no; // index in g_operators, g_logic_axioms, g_ass_axioms or g_comp_axioms
    std::vector<IProposition *> precondition; // only propositional variables allowed here (e.g. result of a comparison axiom)
    std::vector<NumericStateVariable *> axiom_left_right; // "preconditions" of numeric axioms
    cal_operator ass_ax_op; // only for assignment axioms
    comp_operator comp_ax_op; // only for comparison axioms
    UnaryEffect effect;
    ap_float base_cost;
    ap_float precondition_cost; // preconditions costs without base_cost
    ap_float assignment_cost;
    operatorType op_type;
    int unsatisfied_preconditions;
    int repetitions; // sum of all repetitions from the achievers
    bool is_axiom() const;
//    int topology_level; // 0 for logic operators and numeric operators depending on no other numeric variables
    std::string str();
    ap_float cost() {return base_cost + precondition_cost + assignment_cost;}
    // regular operator or logic axiom constructor
    UnaryOperator(int operator_no_,
    			const std::vector<IProposition *> &pre,
    			UnaryEffect eff,
    			ap_float base_cost_,
    			operatorType type_)
        : operator_no(operator_no_),
          precondition(pre),
          axiom_left_right(std::vector<NumericStateVariable *>()),
    	  ass_ax_op(sum), // does not matter
    	  comp_ax_op(lt), // does not matter
          effect(eff),
		  base_cost(base_cost_),
          precondition_cost(0),
          assignment_cost(0),
          op_type(type_),
          unsatisfied_preconditions((int) pre.size()),
          repetitions(0)
    {
    	assert(op_type == logic_op || op_type == numeric_op);
    };
    // assignment axiom constructor
    UnaryOperator(int axiom_no,
    		std::vector<NumericStateVariable *> left_right,
    		cal_operator calop,
    		UnaryEffect eff)
    	: operator_no(axiom_no),
    	  precondition(std::vector<IProposition *>()),
    	  axiom_left_right(left_right),
    	  ass_ax_op(calop),
    	  comp_ax_op(lt), // does not matter
    	  effect(eff),
		  base_cost(0),
    	  precondition_cost(0),
		  assignment_cost(0),
    	  op_type(ass_axiom),
    	  unsatisfied_preconditions((int) axiom_left_right.size()),
          repetitions(0)
    {	assert(axiom_left_right.size()==2);
    	assert(effect.numeric);
    };
    // comparison axiom constructor
    UnaryOperator(int axiom_no,
    		std::vector<NumericStateVariable *> left_right,
    		comp_operator cop,
    		UnaryEffect eff)
    	: operator_no(axiom_no),
    	  precondition(std::vector<IProposition *>()),
    	  axiom_left_right(left_right),
    	  ass_ax_op(sum), // does not matter
    	  comp_ax_op(cop),
    	  effect(eff),
		  base_cost(0),
		  precondition_cost(0),
		  assignment_cost(0),
    	  op_type(comp_axiom),
    	  unsatisfied_preconditions((int) axiom_left_right.size()),
          repetitions(0)
    {	assert(axiom_left_right.size()==2);
    	assert(!effect.numeric);
    };
    UnaryOperator(UnaryEffect eff):
    	operator_no(-2),
    	precondition(std::vector<IProposition *>()),
        axiom_left_right(std::vector<NumericStateVariable *>()),
        ass_ax_op(sum), // does not matter
        comp_ax_op(lt), // does not matter
        effect(eff),
        base_cost(0),
		precondition_cost(0),
        assignment_cost(0),
        op_type(dummy),
        unsatisfied_preconditions(0),
        repetitions(0)
    {};
};

struct ExploredOperator {
	UnaryOperator* op;
	int exploration_id;
	ExploredOperator(UnaryOperator* _op, int _id) : op(_op), exploration_id(_id) {};
};

class RepetitionRelaxationHeuristic: public Heuristic {
	void build_unary_operators(const OperatorProxy &op, int op_no);
	//    void simplify();
	void build_unary_comparison_axioms(const ComparisonAxiomProxy &ax);
	void build_unary_assignment_axioms(const AssignmentAxiomProxy &ax);
	void build_dummy_operators();

protected:
    HeapQueue<ExploredOperator> queue; // cannot use adaptive queue when costs are non integer
    std::vector<NumericStateVariable> numeric_variables;
    std::vector<NumericStateVariable> numeric_cycle_breaker_vars;
    std::map<int, int> cycle_breaker_index;
	std::vector<std::vector<IProposition> > prop_variables; // first index: fdr-variable number second index: value
	std::vector<IProposition *> linearized_prop_variables;
	std::vector<std::string> linearized_fact_names; // required only for better understandable debug output
    std::vector<IProposition *> goal_propositions;
    std::vector<UnaryOperator> classic_operators;
    std::vector<UnaryOperator> num_operators;
	std::vector<UnaryOperator> comparison_axioms;
	std::vector<UnaryOperator> ass_axioms;
	std::vector<UnaryOperator> cycle_breakers;
	std::vector<UnaryOperator> prop_var_dummies; // "initial state" operators that set certain propositions
	std::vector<UnaryOperator *> all_operators;
	bool enqueue_if_necessary(ap_float cost, UnaryOperator *op, bool max_instead_of_val = false);
    void setup_exploration_queue(const State &state);
    void relaxed_exploration(bool reachability_only = false, bool ignore_topology = false);
	IProposition *get_proposition(const FactProxy &fact);
	UnaryEffect get_effect(EffectProxy effect);
	UnaryEffect get_effect(AssEffectProxy num_effect);
	Interval repetition_result(UnaryOperator *op, int exploration_time, bool max_instead_of_val = false); // returns the result of repeatedly applying the operator
	virtual void initialize();
	void initialize_topology();
	virtual ap_float compute_heuristic(const GlobalState &global_state) = 0;
	virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) = 0;
//	size_t get_best_achiever_index(const std::vector<NumericAchiever> &achievers, int achiever_ordering_bound, ap_float target_val);
public:
	RepetitionRelaxationHeuristic(const options::Options &options);
	virtual ~RepetitionRelaxationHeuristic();
	virtual bool dead_ends_are_reliable() const;
};

}

#endif
