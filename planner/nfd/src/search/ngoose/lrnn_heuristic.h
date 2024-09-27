#ifndef LRNN_HEURISTIC_H
#define LRNN_HEURISTIC_H

#include "pybind_heuristic.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace lrnn_heuristic {
class LrnnHeuristic : public pybind_heuristic::PybindHeuristic {
 protected:
  virtual ap_float compute_heuristic(const GlobalState &global_state) override;
  virtual std::vector<ap_float>
  compute_heuristic_batch(const std::vector<GlobalState> &states) override;

  virtual void print_statistics() override;

 public:
  LrnnHeuristic(const options::Options &options);
  virtual ~LrnnHeuristic();
};

}  // namespace lrnn_heuristic

#endif
