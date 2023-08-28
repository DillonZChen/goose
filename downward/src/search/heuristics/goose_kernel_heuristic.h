#ifndef GOOSE_KERNEL_HEURISTIC_H
#define GOOSE_KERNEL_HEURISTIC_H

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include "../heuristic.h"


namespace goose_kernel_heuristic {

class CGraph {
 public: 
  CGraph();
  explicit CGraph(const std::string path);

 private:
  // represent edge labeled graph by linked list
  std::vector<std::vector<std::pair<int, int>>> edges_;

  // map node names to node index
  std::map<std::string, int> node_index_;

  // map node index to colour
  std::vector<std::string> colour_;
};

class GooseKernelHeuristic : public Heuristic {
  void initialise_model(const plugins::Options &opts);
  void initialise_facts();

  // Required for pybind. Once this goes out of scope python interaction is no
  // longer possible.
  //
  // IMPORTANT: since member variables are destroyed in reverse order of
  // construction it is important that the scoped_interpreter_guard is listed as
  // first member variable (therefore destroyed last), otherwise calling the
  // destructor of this class will lead to a segmentation fault.
  pybind11::scoped_interpreter guard;

  // map facts to a better data structure for heuristic computation
  std::map<FactPair, std::pair<std::string, std::vector<std::string>>> fact_to_lifted_input;

  // 1. convert state to CGraph
  CGraph state_to_graph(const State &state);

  // 2. perform WL on CGraph
  std::vector<int> wl_feature(const CGraph &graph);

  // 3. make a prediction with explicit feature
  int predict(const std::vector<int> &feature);

 protected:
  int compute_heuristic(const State &ancestor_state) override;
  std::vector<int> compute_heuristic_batch(const std::vector<State> &ancestor_states) override;
  
 public:
  explicit GooseKernelHeuristic(const plugins::Options &opts);

 private:
  CGraph graph_;
  std::map<std::string, int> hash_;
  std::vector<double> weights_;
  double bias_;
  int feature_size_;
};

}  // namespace goose_kernel_heuristic

#endif  // GOOSE_KERNEL_HEURISTIC_H
