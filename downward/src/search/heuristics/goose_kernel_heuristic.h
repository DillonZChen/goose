#ifndef GOOSE_KERNEL_HEURISTIC_H
#define GOOSE_KERNEL_HEURISTIC_H

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


namespace goose_kernel_heuristic {

class CGraph {  // LLG
 public: 
  CGraph();

  // construct graph from file
  explicit CGraph(const std::string &path);

  // construct graph from edges and colours;
  CGraph(const std::vector<std::vector<std::pair<int, int>>> &edges, const std::vector<int> &colour);

  int colour(const int node) const {
    return colour_[node];
  }

  size_t n_nodes() const {
    return edges_.size();
  }

  bool is_pos_goal_node(const std::string &node_name) const {
    return pos_goal_nodes_.count(node_name);
  }

  bool is_neg_goal_node(const std::string &node_name) const {
    return neg_goal_nodes_.count(node_name);
  }

  int get_node_index(const std::string &node_name) {
    return node_index_[node_name];
  }

  const std::vector<std::vector<std::pair<int, int>>> get_edges() const {
    return edges_;
  }

  std::vector<int> get_colours() const {
    return colour_;
  }

  void dump() {
    for (size_t u = 0; u < edges_.size(); u++) {
      std::cout << u << " " << colour_[u];
      for (auto edge : edges_[u]) {
        std::cout << " " << edge.first << " " << edge.second;
      }
      std::cout  <<  std::endl;
    }
  }

  // hard code colours
  static const int TRUE_FACT_ = 1;
  static const int TRUE_POS_GOAL_ = 2;
  static const int TRUE_NEG_GOAL_ = 3;
  static const int GROUND_EDGE_LABEL_ = 1;

 private:
  // represent edge labeled graph by linked list
  // edges_[u] = [{v_1, e_1}, ..., {v_m, e_m}]
  // u points to nodes v_1 to v_m with edge labels e_1 to e_m respectively
  std::vector<std::vector<std::pair<int, int>>> edges_;

  // map node names to node index
  std::map<std::string, int> node_index_;

  // map node index to colour
  std::vector<int> colour_;

  // positive and negative goal nodes
  std::set<std::string> pos_goal_nodes_;
  std::set<std::string> neg_goal_nodes_;
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
  explicit GooseKernelHeuristic(const plugins::Options &opts);

 private:
  CGraph graph_;
  std::map<std::string, int> hash_;
  std::vector<double> weights_;
  double bias_;
  int feature_size_;
  size_t iterations_;
};

}  // namespace goose_kernel_heuristic

#endif  // GOOSE_KERNEL_HEURISTIC_H
