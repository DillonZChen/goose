#ifndef GOOSE_KERNEL_H
#define GOOSE_KERNEL_H

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include "goose_wl_heuristic.h"
#include "../goose/coloured_graph.h"


/* Kernel model which calls python sklearn for evaluation */

namespace goose_kernel {

class GooseKernel : public goose_wl::WLGooseHeuristic {
  /* Heuristic computation consists of three steps */

  // 1. convert state to CGraph (IG representation)
  // see goose_wl::WLGooseHeuristic
  // 2. perform WL on CGraph
  // see goose_wl::WLGooseHeuristic
  // 3. make a prediction with explicit feature
  int predict(const std::vector<int> &feature);

 protected:
  int compute_heuristic(const State &ancestor_state) override;
  
 public:
  explicit GooseKernel(const plugins::Options &opts);
};

}  // namespace goose_kernel

#endif  // GOOSE_KERNEL_H
