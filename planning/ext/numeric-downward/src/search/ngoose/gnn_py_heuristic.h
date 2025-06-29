#ifndef GNN_PY_HEURISTIC_H
#define GNN_PY_HEURISTIC_H

#include "pybind_heuristic.h"

namespace gnn_py_heuristic {
class GnnPyHeuristic : public pybind_heuristic::PybindHeuristic {

 protected:
  virtual ap_float compute_heuristic(const GlobalState &global_state) override;
  virtual std::vector<ap_float> compute_heuristic_batch(
    const std::vector<GlobalState> &states) override;

  virtual void print_statistics() override;

 public:
  GnnPyHeuristic(const options::Options &options);
  virtual ~GnnPyHeuristic();
};

}  // namespace gnn_heuristic

#endif
