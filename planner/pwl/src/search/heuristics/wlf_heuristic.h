#ifndef GOOSE_HEURISTICS_WLF_HEURISTIC_H_
#define GOOSE_HEURISTICS_WLF_HEURISTIC_H_

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "heuristic.h"
#include "wlf_graph.h"
#include "datalog_transformation_options.h"

#include "../action.h"
#include "../task.h"
#include "../options.h"

#include "../datalog/grounder/weighted_grounder.h"

#include <chrono>

#define get_time() std::chrono::high_resolution_clock::now().time_since_epoch();


class WlfHeuristic : public Heuristic{
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

  void load_and_init_model(std::string py_model_class);

  // WLF specific
  int iterations_;
  std::vector<double> weights_;
  std::unordered_map<std::string, int> hash_;
  int n_init_features_;
  int n_features_;

  std::chrono::duration<double> start_time, end_time;
  double graph_time;
  double wl_time;
  double linear_time;

  std::shared_ptr<ngoose_wlf_graph::WlfGraph> graph;

  const ngoose_wlf_graph::WlfGraph state_to_graph(const DBState &s, const Task &task);
  std::vector<int> compute_features(const ngoose_wlf_graph::WlfGraph &state_graph);
  int estimate(const std::vector<int> x);

public:
  WlfHeuristic(const Options &opts, const Task &task);

  void print_statistics() override;

  int compute_heuristic(const DBState &s, const Task &task) override;
};

#endif //GOOSE_HEURISTICS_WLF_HEURISTIC_H_
