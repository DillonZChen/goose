#ifndef ADDITIVE_INTERVAL_BASED_RELAXATION_H
#define ADDITIVE_INTERVAL_BASED_RELAXATION_H

#include "../heuristic.h"
#include "interval.h"
#include "interval_relaxation_heuristic.h"

class FactProxy;
class GlobalState;
class OperatorProxy;

using interval_relaxation_heuristic::NumericState;
using interval_relaxation_heuristic::UnaryOperator;
using interval_relaxation_heuristic::Proposition;
using interval_relaxation_heuristic::UnaryEffect;

namespace additive_interval_based_relaxation {
struct Supporter;

class AdditiveIntervalBasedRelaxation : public Heuristic {
	void build_unary_operators(const OperatorProxy &op);
	void build_unary_axioms(const OperatorProxy &ax, int operator_no);
	void build_unary_comparison_axioms(const ComparisonAxiomProxy &ax);
	void build_unary_assignment_axioms(const AssignmentAxiomProxy &ax);

protected:
	int num_propositions;
	std::vector<UnaryOperator> unary_operators;
	std::vector<UnaryOperator> numeric_axioms; // ordered by layer
	std::vector<UnaryOperator> unary_axioms;
	std::vector<std::vector<Proposition>> propositions;
	std::vector<Proposition *> goal_propositions;
	std::vector<NumericState> planning_graph;
	std::list<UnaryOperator *> applicable_operators;
	std::list<UnaryOperator *> applicable_axioms;
	bool trigger_supporter(UnaryOperator *op, const NumericState &old_state, NumericState &new_state);
	void setup_exploration(const State &state);
	bool relaxed_exploration(bool reachability = false);
	Proposition *get_proposition(const FactProxy &fact);
	UnaryEffect get_effect(EffectProxy effect);
	UnaryEffect get_effect(AssEffectProxy num_effect);
	void handle_prop(Proposition * prop, ap_float distance, int layer, UnaryOperator* achiever, int &missinggoals);
	virtual void initialize();
	virtual ap_float compute_heuristic(const GlobalState &global_state) = 0;
	virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) = 0;

public:
	AdditiveIntervalBasedRelaxation(const options::Options &options);
	virtual ~AdditiveIntervalBasedRelaxation();
	virtual bool dead_ends_are_reliable() const { return !has_logic_axioms; }
};

}


#endif // ADDITIVE_INTERVAL_BASED_RELAXATION_H