#ifndef REPETITION_FF_HEURISTIC_H
#define REPETITION_FF_HEURISTIC_H

#include "repetition_relaxation_heuristic.h"

namespace repetition_ff_heuristic{
using repetition_relaxation_heuristic::UnaryOperator;
using repetition_relaxation_heuristic::IProposition;
using repetition_relaxation_heuristic::NumericAchiever;
using repetition_relaxation_heuristic::UnaryEffect;
using repetition_relaxation_heuristic::NumericStateVariable;
using repetition_relaxation_heuristic::operatorType;
using repetition_relaxation_heuristic::global_achiever_ordering_rank;
static constexpr ap_float MAX_COST_VALUE = 100000000;

class RepetitionFFHeuristic : public repetition_relaxation_heuristic::RepetitionRelaxationHeuristic {
    bool did_write_overflow_warning;
    static const int FAIRNESS_REPETITIONS = 5;
    void mark_preferred_operators_and_determine_repetitions(const State &state, IProposition *subgoal);
    void mark_preferred_operators_and_determine_repetitions(const State &state, NumericStateVariable *subgoal, ap_float target_value, ap_float parent_cost, int parent_ordinal_number);
protected:
    virtual void initialize();
    virtual ap_float compute_heuristic(const GlobalState &global_state);
    virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {
    	if (old_cost + new_cost > MAX_COST_VALUE) {
    		if(!did_write_overflow_warning) {
    			std::cerr << "WARNING i_h_FF overflow prevented" << std::endl;
    			did_write_overflow_warning = true;
    		}
    	}
    	return std::min(MAX_COST_VALUE, old_cost + new_cost);}
public:
    RepetitionFFHeuristic(const options::Options &options);
    ~RepetitionFFHeuristic();
};

}
#endif
