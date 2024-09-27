#ifndef INTERVAL_FF_HEURISTIC_H
#define INTERVAL_FF_HEURISTIC_H

#include "interval_relaxation_heuristic.h"

namespace interval_FF_heuristic {
using interval_relaxation_heuristic::Proposition;
using interval_relaxation_heuristic::UnaryOperator;
using interval_relaxation_heuristic::NumericState;

struct RelaxedPlan {
	std::vector<int> prop_ops; // operator has to be applied latest in layer i (-1 = not needed)
	std::vector<std::vector<bool>> num_ops; // vecor[#ops][#layers]
	RelaxedPlan(int ops, int layers);
};

class IntervalFFHeuristic : public interval_relaxation_heuristic::IntervalRelaxationHeuristic {
    void mark_preferred_operators_and_relaxed_plan(
        const State &state, Proposition *goal);
    void mark_preferred_operators_and_relaxed_plan(
        const State &state, size_t num_var, ap_float target_value, size_t layer);
    UnaryOperator* determine_achiever(int var_index, size_t layer, ap_float target);
    RelaxedPlan plan;
    // the "marked" flags of numeric "propositions"
    std::vector<std::vector<bool>> numeric_markings; // vector[#variables][#layers]

protected:
	virtual void initialize();
	virtual ap_float compute_heuristic(const GlobalState &global_state);
	virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {return old_cost + new_cost;}
public:
	IntervalFFHeuristic(const options::Options &options);
	~IntervalFFHeuristic();
};
}

#endif
