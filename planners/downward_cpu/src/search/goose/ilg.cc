#include "ilg.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <regex>
#include <string>
#include <vector>

InstanceLearningGraph::InstanceLearningGraph(const Edges &edges,
                                             const std::vector<int> &colour)
    : CGraph(edges, colour) {}

InstanceLearningGraph::InstanceLearningGraph(const std::string &path) {
  std::string line;
  std::vector<std::string> toks;
  std::ifstream infile(path);
  int node = 0;
  int n_nodes = -1;
  int n_pos_goal_nodes = -1;
  int n_neg_goal_nodes = -1;
  int pos_goal_cnt = 0;
  int neg_goal_cnt = 0;
  int n_predicates = -1;
  int predicate_cnt = 0;

  // collect graph information
  while (std::getline(infile, line)) {
    toks = tokenise(line);

    // collect graph structure and colours
    // line number = node
    // <node_name> <node_colour> [<neighbour_node> <edge_label>]
    if (n_nodes == -1) {
      n_nodes = stoi(toks[0]);
      continue;
    }
    if (node < n_nodes) {
      std::string node_name = toks[0];
      node_index_[node_name] = node;

      int colour = stoi(toks[1]);
      colour_.push_back(colour);

      std::vector<std::pair<int, int>> neighbours;

      for (size_t i = 2; i < toks.size(); i += 2) {
        int neighbour_node = stoi(toks[i]);
        int edge_label = stoi(toks[i + 1]);
        neighbours.push_back({neighbour_node, edge_label});
      }

      edges_.push_back(neighbours);

      node++;
      continue;
    }

    // collect positive goal nodes
    if (n_pos_goal_nodes == -1) {
      n_pos_goal_nodes = stoi(toks[0]);
      continue;
    }
    if (pos_goal_cnt < n_pos_goal_nodes) {
      std::string node_name = toks[0];
      pos_goal_nodes_.insert(node_name);
      pos_goal_cnt++;
      continue;
    }

    // collect negative goal nodes
    if (n_neg_goal_nodes == -1) {
      n_neg_goal_nodes = stoi(toks[0]);
      continue;
    }
    if (neg_goal_cnt < n_neg_goal_nodes) {
      std::string node_name = toks[0];
      neg_goal_nodes_.insert(node_name);
      neg_goal_cnt++;
      continue;
    }

    // collect predicates and indices
    if (n_predicates == -1) {
      n_predicates = stoi(toks[0]);
      continue;
    }
    if (predicate_cnt < n_predicates) {
      pred_to_idx[toks[0]] = stoi(toks[1]);
      predicate_cnt++;
      continue;
    }
  }

  // remove file
  char *char_array = new char[path.length() + 1];
  strcpy(char_array, path.c_str());
  remove(char_array);
}

std::shared_ptr<CGraph> InstanceLearningGraph::predicate_arguments_to_graph(
    const PredicateArguments pred_args) {
  std::vector<std::vector<std::pair<int, int>>> edges = get_edges();
  std::vector<int> colours = get_colours();
  int cur_node_fact;
  int new_idx = n_nodes();

  std::string pred, node_name;
  std::vector<std::string> args;
  for (const PredicateArgument &pred_args : pred_args) {
    pred = pred_args.first;
    args = pred_args.second;
    if (pred.size() == 0) {
      continue;
    }

    node_name = pred;
    for (const std::string &arg : args) {
      node_name += ',' + arg;
    }

    if (is_pos_goal_node(node_name)) {
      // std::cout<<node_name<<std::endl;
      colours[get_node_index(node_name)] =
          1 + 3 * pred_to_idx[pred] + T_POS_GOAL;
      continue;
    }
    // if (is_neg_goal_node(node_name)) {
    //   colours[get_node_index(node_name)] = TRUE_NEG_GOAL_;
    //   continue;
    // }

    // add new node
    cur_node_fact = new_idx;
    new_idx++;
    colours.push_back(1 + 3 * pred_to_idx[pred] + T_NON_GOAL);  // TRUE_FACT
    std::vector<std::pair<int, int>> new_edges_fact;
    edges.push_back(new_edges_fact);

    // // connect fact to predicate
    // int pred_node = get_node_index(pred);
    // edges[cur_node_fact].push_back(std::make_pair(pred_node,
    // GROUND_EDGE_LABEL_));
    // edges[pred_node].push_back(std::make_pair(cur_node_fact,
    // GROUND_EDGE_LABEL_));

    for (size_t k = 0; k < args.size(); k++) {
      // connect fact to object
      int object_node = get_node_index(args[k]);
      edges[object_node].push_back(std::make_pair(cur_node_fact, k));
      edges[cur_node_fact].push_back(std::make_pair(object_node, k));
    }
  }
  // std::cout<<std::endl;

  return std::make_shared<InstanceLearningGraph>(
      InstanceLearningGraph(edges, colours));
}
