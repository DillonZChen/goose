#include "opg.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <regex>
#include <string>
#include <vector>

ObjectPairGraph::ObjectPairGraph(const Edges &edges,
                                 const std::vector<int> &colour)
    : CGraph(edges, colour) {}

ObjectPairGraph::ObjectPairGraph(const std::string &path) {
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

    if (line.find("largest_predicate_size") != std::string::npos) {
      largest_predicate_size = stoi(toks[0]);
      continue;
    }

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

std::shared_ptr<CGraph> ObjectPairGraph::predicate_arguments_to_graph(
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

    // activated proposition overlaps with a goal Atom
    if (is_pos_goal_node(node_name)) {
      colours[get_node_index(node_name)] =
          2 + 3 * pred_to_idx[pred] + T_POS_GOAL;
      continue;
    }

    cur_node_fact = new_idx;
    new_idx++;

    // else add node and corresponding edges to graph
    colours.push_back(2 + 3 * pred_to_idx[pred] + T_NON_GOAL);
    std::vector<std::pair<int, int>> new_edges_fact;  // add new node
    edges.push_back(new_edges_fact);

    std::map<std::string, int> obj_to_index;
    int n = largest_predicate_size;
    for (size_t k = 0; k < args.size(); k++) {
      obj_to_index[args[k]] = k;
      // connect fact to object
      int object_node = get_node_index(args[k]);
      edges[object_node].push_back(std::make_pair(cur_node_fact, k));
      edges[cur_node_fact].push_back(std::make_pair(object_node, k));
    }

    for (size_t i = 0; i < args.size(); i++) {
      for (size_t j = i + 1; j < args.size(); j++) {
        std::string obj1 = args[i];
        std::string obj2 = args[j];
        if (obj2 < obj1) {
          std::string objtmp = obj1;
          obj1 = obj2;
          obj2 = objtmp;
        }
        int object_pair_node = get_node_index(obj1 + "," + obj2);
        int k1 = obj_to_index[obj1];
        int k2 = obj_to_index[obj2];
        int k = n + k2 - k1 - 1 + (k1 * n) - (k1 * (k1 + 1)) / 2;
        edges[object_pair_node].push_back(std::make_pair(cur_node_fact, k));
        edges[cur_node_fact].push_back(std::make_pair(object_pair_node, k));
      }
    }
  }

  return std::make_shared<ObjectPairGraph>(ObjectPairGraph(edges, colours));
}
