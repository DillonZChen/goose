#ifndef DLPLAN_HEURISTIC_H
#define DLPLAN_HEURISTIC_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "../ext/dlplan/include/dlplan/core.h"
#include "../ext/dlplan/include/dlplan/state_space.h"

#include "../heuristic.h"

namespace dlplan_heuristic {
class DLPlanHeuristic : public Heuristic {
  // Required for pybind. Once this goes out of scope python interaction is no
  // longer possible.
  //
  // IMPORTANT: since member variables are destroyed in reverse order of
  // construction it is important that the scoped_interpreter_guard is listed as
  // first member variable (therefore destroyed last), otherwise calling the
  // destructor of this class will lead to a segmentation fault.
  pybind11::scoped_interpreter guard;

  // Python object which computes the heuristic
  pybind11::object model;

  std::map<FactPair, int> _fact_pair_to_atom_idx;

  std::shared_ptr<dlplan::core::InstanceInfo> _instance_info;
  std::shared_ptr<dlplan::core::SyntacticElementFactory> _factory_info;

  std::vector<std::shared_ptr<const dlplan::core::Boolean>> _b_features;
  std::vector<double> _b_weights;
  std::vector<std::shared_ptr<const dlplan::core::Numerical>> _n_features;
  std::vector<double> _n_weights;

 protected:
  int compute_heuristic(const State &ancestor_state) override;

 public:
  explicit DLPlanHeuristic(const plugins::Options &opts);
};

}  // namespace dlplan_heuristic

#endif
