#ifndef GOOSE_XGBOOST_H
#define GOOSE_XGBOOST_H

#include <xgboost/c_api.h>

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../goose/coloured_graph.h"
#include "goose_wl_heuristic.h"


namespace goose_xgboost {

class GooseXGBoost : public goose_wl::WLGooseHeuristic {
  /* Heuristic computation consists of three steps */

  // 1. convert state to CGraph
  // see goose_wl::WLGooseHeuristic
  // 2. perform WL on CGraph
  // see goose_wl::WLGooseHeuristic
  // 3. make a prediction with explicit feature
  int predict(const std::vector<int> &feature);

  BoosterHandle booster;

 protected:
  int compute_heuristic(const State &ancestor_state) override;

 public:
  explicit GooseXGBoost(const plugins::Options &opts);
};

}  // namespace goose_xgboost

#endif  // GOOSE_XGBOOST_H
