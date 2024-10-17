#ifndef INTERVAL_MAX_HEURISTIC_H
#define INTERVAL_MAX_HEURISTIC_H

#include "interval_relaxation_heuristic.h"

namespace interval_max_heuristic {
using interval_relaxation_heuristic::Proposition;
using interval_relaxation_heuristic::UnaryOperator;
using interval_relaxation_heuristic::NumericState;

class IntervalMaxHeuristic : public interval_relaxation_heuristic::IntervalRelaxationHeuristic {
protected:
	virtual void initialize();
	virtual ap_float compute_heuristic(const GlobalState &global_state);
	virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {return std::max(old_cost, new_cost);}
public:
	IntervalMaxHeuristic(const options::Options &options);
	~IntervalMaxHeuristic();
};
}

#endif
