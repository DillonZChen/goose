#ifndef COLOURED_GRAPH_H
#define COLOURED_GRAPH_H

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

typedef std::pair<std::string, std::vector<std::string>> PredicateArgument;
typedef std::vector<PredicateArgument> PredicateArguments;

typedef std::vector<std::vector<std::pair<int, int>>> Edges;

class CGraph {
 public:
  CGraph();

  // construct graph from edges and colours;
  CGraph(const Edges &edges, const std::vector<int> &colour);

  virtual std::shared_ptr<CGraph>
  predicate_arguments_to_graph(const PredicateArguments pred_args) = 0;

  int colour(const int node) const { return colour_[node]; }

  size_t n_nodes() const { return edges_.size(); }

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

  std::vector<int> get_colours() const { return colour_; }

  void dump() {
    for (size_t u = 0; u < edges_.size(); u++) {
      std::cout << u << " " << colour_[u];
      for (auto edge : edges_[u]) {
        std::cout << " " << edge.first << " " << edge.second;
      }
      std::cout << std::endl;
    }
  }

 protected:
  std::vector<std::string> tokenise(const std::string str) {
    std::istringstream iss(str);
    std::string s;
    std::vector<std::string> ret;
    while (std::getline(iss, s, ' ')) {
      ret.push_back(s);
    }
    return ret;
  }

  // represent edge labeled graph by linked list
  // edges_[u] = [{v_1, e_1}, ..., {v_m, e_m}]
  // u points to nodes v_1 to v_m with edge labels e_1 to e_m respectively
  Edges edges_;

  // map node names to node index
  std::unordered_map<std::string, int> node_index_;

  // map node index to colour
  std::vector<int> colour_;

  // positive and negative goal nodes
  std::set<std::string> pos_goal_nodes_;
  std::set<std::string> neg_goal_nodes_;
};

#endif  // COLOURED_GRAPH_H
