#ifndef REPETITION_ADD_HEURISTIC_H
#define REPETITION_ADD_HEURISTIC_H

#include "repetition_relaxation_heuristic.h"

/**
 * The heuristic estimate of the repetition relaxed additive heuristic
 * corresponds to Bonet's additive heuristic h_add for classical planning
 * domains.
 *
 * The heuristic is computed with the general Dijkstra algorithm.
 *
 * The repetition relaxation accounts for umlimited applications of each operator only ONCE
 * as such it can underestimates the actual relaxed plan cost severly.
 *
 * Example 1:
 * I(x): 0
 * o_1 (cost 1): <x += 1>
 * G : x >= 5
 * h_add is 1 because o_1 has to be applied once in the repetition relaxation
 *
 * Example 2:
 * I(x) = 0; I(y) = 1
 * o_1 (cost 1): <x += y>; o_2 (cost 1000): <y += 1>
 * G: x >= 1
 * h_add is 1001 because x depends on y topologically, and o_1 is only checked after y reaches its maximal value
 */

namespace repetition_add_heuristic{
using repetition_relaxation_heuristic::UnaryOperator;
using repetition_relaxation_heuristic::IProposition;
using repetition_relaxation_heuristic::NumericAchiever;
using repetition_relaxation_heuristic::UnaryEffect;
using repetition_relaxation_heuristic::NumericStateVariable;
using repetition_relaxation_heuristic::operatorType;
static constexpr ap_float MAX_COST_VALUE = 100000000;

class RepetitionAddHeuristic : public repetition_relaxation_heuristic::RepetitionRelaxationHeuristic {
    bool did_write_overflow_warning;
protected:
    virtual void initialize();
    virtual ap_float compute_heuristic(const GlobalState &global_state);
    virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {
    	if (old_cost + new_cost > MAX_COST_VALUE) {
    		if(!did_write_overflow_warning) {
    			std::cerr << "WARNING i_h_add overflow prevented" << std::endl;
    			did_write_overflow_warning = true;
    		}
    	}
    	return std::min(MAX_COST_VALUE, old_cost + new_cost);}
public:
    RepetitionAddHeuristic(const options::Options &options);
    ~RepetitionAddHeuristic();
};

}

#endif
