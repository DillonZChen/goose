#ifndef COLOURED_GRAPH_H
#define COLOURED_GRAPH_H

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include <iostream>

class CGraph {  // mainly assumes GOOSE LLG
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

  // // hard code colours
  // static const int TRUE_FACT_ = 1;
  // static const int TRUE_POS_GOAL_ = 2;
  // static const int TRUE_NEG_GOAL_ = 3;
  // static const int GROUND_EDGE_LABEL_ = 1;

  // hard code colours
  static const int TRUE_FACT_ = 0;
  static const int TRUE_POS_GOAL_ = 1;
  static const int TRUE_NEG_GOAL_ = 2;
  static const int GROUND_EDGE_LABEL_ = -1;

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

#endif  // COLOURED_GRAPH_H
