#ifndef WLF_PY_HEURISTIC_H
#define WLF_PY_HEURISTIC_H

#include "pybind_heuristic.h"

namespace wlf_py_heuristic {
class WlfPyHeuristic : public pybind_heuristic::PybindHeuristic {

 protected:
  virtual ap_float compute_heuristic(const GlobalState &global_state) override;
  virtual std::vector<ap_float> compute_heuristic_batch(
    const std::vector<GlobalState> &states) override;

 public:
  WlfPyHeuristic(const options::Options &options);
  virtual ~WlfPyHeuristic();
};

}  // namespace wlf_py_heuristic

#endif
