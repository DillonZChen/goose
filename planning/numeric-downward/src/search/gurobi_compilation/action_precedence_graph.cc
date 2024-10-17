#include "action_precedence_graph.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>

using namespace gurobi_ip_compilation;

void ActionPrecedenceGraph::add_edge(int a, int b) {
  if (!edges[a][b]) {
    edges[a][b] = true;
    ++n_edges;
  }
}

int ActionPrecedenceGraph::get_n_edges() const {
  return n_edges;
}

bool ActionPrecedenceGraph::has_no_cycle() {
  if (n_edges <= 1) return true;

  nodes.clear();
  int n_nodes = edges.size();

  for (int a = 0; a < n_nodes; ++a) {
    bool has_edge = false;
    for (int b = 0; b < n_nodes; ++b) {
      if (a == b) continue;
      if (edges[a][b] || edges[b][a]) {
        has_edge = true;
        break;
      }
    }
    if (has_edge) nodes.push_back(a);
  }

  auto sorted = topological_sort(nodes);

  return sorted.size() == nodes.size();
}

void ActionPrecedenceGraph::find_cycle(const std::vector<double> &x_values,
                                       std::vector<std::vector<int>> &cycles) {
  nodes.clear();
  for (int i = 0, n = x_values.size(); i < n; ++i)
    if (x_values[i] > 0.0) nodes.push_back(i);
  if (nodes.size() <= 1) {
    cycles.clear();
    return;
  }

  floyd_warshall(x_values);
  int num_cuts = 0;

  for (auto a : nodes) {
    for (auto b : nodes) {
      if (a == b || next[a][b] == -1 || !edges[b][a]) continue;
      if (d[a][b] - x_values[b] - x_values[a] + 1 < 0) {
        ++num_cuts;
        if (cycles.size() < num_cuts) {
          cycles.emplace_back(std::vector<int>());
        } else {
          cycles[num_cuts - 1].clear();
        }
        construct_shortest_path(x_values, a, b, cycles[num_cuts - 1]);
        if (num_cuts >= max_num_cuts) {
          cycles.resize(num_cuts);
          return;
        }
      }
    }
  }

  cycles.resize(num_cuts);
  return;
}

void ActionPrecedenceGraph::floyd_warshall(
    const std::vector<double> &x_values) {
  for (auto i : nodes) {
    std::fill(d[i].begin(), d[i].end(), std::numeric_limits<double>::max());
    std::fill(next[i].begin(), next[i].end(), -1);
    for (auto j : nodes) {
      if (i != j && edges[i][j]) {
        next[i][j] = j;
        d[i][j] = 2 - x_values[i] - x_values[j];
      }
    }
  }

  for (auto k : nodes) {
    for (auto i : nodes) {
      if (k == i) continue;
      for (auto j : nodes) {
        if (i == j || k == j || next[i][k] == -1 || next[k][j] == -1) continue;
        if (d[i][j] > d[i][k] + d[k][j]) {
          d[i][j] = d[i][k] + d[k][j];
          next[i][j] = next[i][k];
        }
      }
    }
  }
}

void ActionPrecedenceGraph::construct_shortest_path(
    const std::vector<double> &x_values, int a, int b, std::vector<int> &path) {
  int current_node = a;
  path.resize(1);
  path[0] = a;
  while (current_node != b) {
    current_node = next[current_node][b];
    path.push_back(current_node);
  }
}

std::vector<int> ActionPrecedenceGraph::topological_sort(
    const std::vector<int> &input_nodes) {
  std::vector<int> sorted;
  std::unordered_map<int, int> num_edges;
  std::queue<int> candidates;

  for (auto a : input_nodes) {
    num_edges[a] = 0;
    for (auto b : input_nodes) {
      if (a != b && edges[b][a]) {
        num_edges[a] += 1;
      }
    }
    if (num_edges[a] == 0) candidates.push(a);
  }

  while (!candidates.empty()) {
    int a = candidates.front();
    candidates.pop();
    for (auto b : input_nodes) {
      if (a != b && edges[a][b]) {
        num_edges[b] -= 1;
        if (num_edges[b] == 0) candidates.push(b);
      }
    }
    sorted.push_back(a);
  }

  return std::move(sorted);
}