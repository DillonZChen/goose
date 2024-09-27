#ifndef AIBR_HEURISTIC_H
#define AIBR_HEURISTIC_H

#include <list>
#include <vector>

#include "additive_interval_based_relaxation.h"
#include "interval_relaxation_heuristic.h"
#include "../global_state.h"
#include "../task_proxy.h"

using interval_relaxation_heuristic::NumericState;

namespace aibr_heuristic {

class AIBRHeuristic : public additive_interval_based_relaxation::AdditiveIntervalBasedRelaxation {
  std::vector<std::list<UnaryOperator*>> applicable_operator_to_unary_operator;
protected:
  ap_float compute_aibr_estimate();
  virtual void initialize() override;
	virtual ap_float compute_heuristic(const GlobalState &global_state) override;
  virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) override { return old_cost + new_cost; }

public:
  std::vector<NumericState> get_relaxed_reachable_states(const GlobalState &global_state);
	AIBRHeuristic(const options::Options &options);
	virtual ~AIBRHeuristic();
};

}

#endif // AIBR_HEURISTIC_H