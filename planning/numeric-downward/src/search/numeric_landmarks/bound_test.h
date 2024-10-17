#ifndef BOUND_TEST_H
#define BOUND_TEST_H

#include "numeric_bound.h"

#include "../heuristic.h"

#include <memory>

class GlobalState;

namespace options {
  class Options;
}

namespace bound_test {
  class BoundTestHeuristic : public Heuristic {
      numeric_helper::NumericTaskProxy numeric_task_proxy;
      numeric_bound::NumericBound bound;

      virtual void initialize() override {}
      virtual ap_float compute_heuristic(const GlobalState &global_state) override;

    public:
      explicit BoundTestHeuristic(const options::Options &opts);
      virtual ~BoundTestHeuristic() override;
  };
}

#endif

