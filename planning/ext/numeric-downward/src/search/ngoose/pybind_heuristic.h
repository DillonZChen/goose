#ifndef PYBIND_HEURISTIC_H
#define PYBIND_HEURISTIC_H

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "../heuristic.h"

#include <string>
#include <unordered_set>
#include <vector>

#define get_time() std::chrono::high_resolution_clock::now().time_since_epoch();

namespace pybind_heuristic {
class PybindHeuristic : public Heuristic {
 protected:
  // Required for pybind. Once this goes out of scope python interaction is no
  // longer possible.
  //
  // IMPORTANT: since member variables are destroyed in reverse order of
  // construction it is important that the scoped_interpreter_guard is listed
  // as first member variable (therefore destroyed last), otherwise calling the
  // destructor of this class will lead to a segmentation fault.
  pybind11::scoped_interpreter guard;
  pybind11::object model;

  std::string model_path;
  std::string domain_path;
  std::string problem_path;

  std::vector<std::vector<std::string>> fdr_pair_to_name;
  std::vector<std::vector<bool>> fdr_pair_to_is_true;
  std::vector<int> fluent_indices;
  std::vector<std::string> fluent_names;
  std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>
      fact_to_pred_objects;

  void load_and_init_model(std::string py_model_class);

  std::unordered_set<std::string> get_fact_names();

 public:
  PybindHeuristic(const options::Options &options, const std::string py_model_class);
  virtual ~PybindHeuristic();
};

}  // namespace pybind_heuristic

#endif
