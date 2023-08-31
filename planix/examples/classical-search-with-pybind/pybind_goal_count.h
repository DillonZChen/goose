#ifndef PYBIND_GOAL_COUNT_H_
#define PYBIND_GOAL_COUNT_H_

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include <vector>
#include <map>
#include <utility>
#include <string>

#include "representations/strips_problems.h"

using STRIPS::StripsState;
using STRIPS::StripsProblem;

namespace goal_count_heuristic {
class PybindGoalCountHeuristic {
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

  pybind11::list state_to_python_list(const StripsState &state);

 public:
  explicit PybindGoalCountHeuristic(const StripsProblem& problem);
  double compute_heuristic(const StripsState &state);
  std::vector<double> compute_heuristic_batch(const std::vector<StripsState> &states);
};

}  // namespace goal_count_heuristic

#endif  // PYBIND_GOAL_COUNT_H_
