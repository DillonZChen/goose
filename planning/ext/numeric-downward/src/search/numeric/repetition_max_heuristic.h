#ifndef REPETITION_MAX_HEURISTIC_H
#define REPETITION_MAX_HEURISTIC_H

#include "repetition_relaxation_heuristic.h"

namespace repetition_max_heuristic{
using repetition_relaxation_heuristic::UnaryOperator;
using repetition_relaxation_heuristic::IProposition;
using repetition_relaxation_heuristic::NumericAchiever;
using repetition_relaxation_heuristic::UnaryEffect;
using repetition_relaxation_heuristic::NumericStateVariable;
using repetition_relaxation_heuristic::operatorType;

class RepetitionMaxHeuristic : public repetition_relaxation_heuristic::RepetitionRelaxationHeuristic {
protected:
    virtual void initialize();
    virtual ap_float compute_heuristic(const GlobalState &global_state);
    virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {return std::max(old_cost, new_cost);}
public:
    RepetitionMaxHeuristic(const options::Options &options);
    ~RepetitionMaxHeuristic();
};

}
#endif
