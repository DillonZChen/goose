#ifndef GOOSE_LINEAR_H
#define GOOSE_LINEAR_H

#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../goose/coloured_graph.h"
#include "goose_wl_heuristic.h"

/* Optimised linear regression model all in c++ with no pybind */

namespace goose_linear {

class GooseLinear : public goose_wl::WLGooseHeuristic {
  bool compute_std_;

  double compute_std(const State &ancestor_state);
  
 protected:
  /* Heuristic computation consists of three steps */

  // 1. convert state to CGraph (IG representation)
  // see goose_wl::WLGooseHeuristic
  // 2. perform WL on CGraph
  // see goose_wl::WLGooseHeuristic
  // 3. make a prediction with explicit feature
  int predict(const std::vector<int> &feature);

  int compute_heuristic(const State &ancestor_state) override;

 public:
  explicit GooseLinear(const plugins::Options &opts);

  int compute_heuristic_from_feature(const std::vector<int> &feature, int model);
};

}  // namespace goose_linear

#endif  // GOOSE_LINEAR_H
