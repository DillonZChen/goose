#include "pybind_goal_count.h"

#include <iostream>
#include <fstream>
#include <vector>

namespace py = pybind11;


namespace goal_count_heuristic {
PybindGoalCountHeuristic::PybindGoalCountHeuristic(const StripsProblem& problem) {
  // Add python_src submodule to the python path
  auto env_var = std::getenv("PYTHON_GOAL_COUNT");
  if (!env_var) {
      std::cout << "PYTHON_GOAL_COUNT env variable not found. Aborting." << std::endl;
      exit(-1);
  }
  std::string path(env_var);
  std::cout << "PYTHON_GOAL_COUNT path is " << path << std::endl;
  if (access(path.c_str(), F_OK) == -1) {
      std::cout << "PYTHON_GOAL_COUNT points to non-existent path. Aborting." << std::endl;
      exit(-1);
  }

  // Append python module directory to the path
  py::module sys = py::module::import("sys");
  sys.attr("path").attr("append")(path);

  // Force all output being printed to stdout. Otherwise INFO logging from
  // python will be printed to stderr, even if it is not an error.
  sys.attr("stderr") = sys.attr("stdout");

  // Load module and create python object
  py::module main_module = py::module::import("python_goal_count.main");
  StripsState goal_set = problem.goal_set();
  model = main_module.attr("construct_goal_count_heuristic")(state_to_python_list(goal_set));
}

py::list PybindGoalCountHeuristic::state_to_python_list(const StripsState &state) {
  py::list python_list;
  size_t index = state.find_first();
  while (index != boost::dynamic_bitset<>::npos) {
    python_list.append(index);
    index = state.find_next(index);
  }
  return python_list;
}

double PybindGoalCountHeuristic::compute_heuristic(const StripsState &state) {
  py::list python_list = state_to_python_list(state);
  int h = model.attr("h")(python_list).cast<int>();
  return h;
}

std::vector<double> PybindGoalCountHeuristic::compute_heuristic_batch(
  const std::vector<StripsState> &states
) {
  py::list py_states;
  for (const auto& state : states) {
    py_states.append(state_to_python_list(state));
  }
  py::object heuristics = model.attr("h_batch")(py_states);
  std::vector<double> ret = heuristics.cast<std::vector<double>>();
  return ret;
}

}  // namespace goal_count_heuristic
