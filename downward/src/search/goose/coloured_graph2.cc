#include "coloured_graph.h"

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <cstdio>
#include <algorithm>


std::vector<std::string> tokenise(const std::string str) {
  std::istringstream iss(str);
  std::string s;
  std::vector<std::string> ret;
  while (std::getline(iss, s, ' ')) {
    ret.push_back(s);
  }
  return ret;
}

CGraph::CGraph() 
{ }

CGraph::CGraph(const std::string &path) {
  std::string line;
  std::vector<std::string> toks;
  std::ifstream infile(path);
  int node = 0;
  int n_nodes = -1;
  int n_pos_goal_nodes = -1;
  int n_neg_goal_nodes = -1;
  int pos_goal_cnt = 0;
  int neg_goal_cnt = 0;

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

      for (size_t i = 2; i < toks.size(); i+=2) {
        int neighbour_node = stoi(toks[i]);
        int edge_label = stoi(toks[i+1]);
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
  }

  // remove file
  char* char_array = new char[path.length() + 1];
  strcpy(char_array, path.c_str());
  remove(char_array);
}

CGraph::CGraph(const std::vector<std::vector<std::pair<int, int>>> &edges, 
  const std::vector<int> &colour) : edges_(edges), colour_(colour) 
{ }
