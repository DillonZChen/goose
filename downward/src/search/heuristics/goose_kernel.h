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

#include "../heuristic.h"
#include "../goose/coloured_graph.h"


/* Optimised kernel evaluation. 
  TODO: use OOP to reduce copied code with goose_linear_regression
*/

namespace goose_kernel {

class GooseKernel : public Heuristic {
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

  // Python object which computes the heuristic
  pybind11::object model;

  // map facts to a better data structure for heuristic computation
  std::map<FactPair, std::pair<std::string, std::vector<std::string>>> fact_to_lifted_input;

  /* Heuristic computation consists of three steps */

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
  explicit GooseKernel(const plugins::Options &opts);

 private:
  CGraph graph_;
  std::map<std::string, int> hash_;
  int feature_size_;
  size_t iterations_;
};

}  // namespace goose_kernel

#endif  // GOOSE_KERNEL_H
