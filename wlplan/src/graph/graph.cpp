#include "../../include/graph/graph.hpp"

namespace graph {
  Graph::Graph(bool store_node_names) : store_node_names(store_node_names) {}

  Graph::Graph(const std::vector<int> &node_colours,
               const std::vector<std::vector<std::pair<int, int>>> &edges)
      : nodes(node_colours), edges(edges), store_node_names(false) {}

  Graph::Graph(const std::vector<int> &node_colours,
               const std::vector<std::string> &node_names,
               const std::vector<std::vector<std::pair<int, int>>> &edges)
      : nodes(node_colours), edges(edges), store_node_names(true), index_to_node_(node_names) {
    for (size_t u = 0; u < node_names.size(); u++) {
      node_to_index_[node_names[u]] = u;
    }
  }

  int Graph::add_node(const std::string &node_name, int colour) {
    int index = nodes.size();
    nodes.push_back(colour);
    if (store_node_names) {
      node_to_index_[node_name] = index;
      index_to_node_.push_back(node_name);
    }
    edges.push_back(std::vector<std::pair<int, int>>());
    return index;
  }

  void Graph::add_edge(const int u, const int r, const int v) {
    edges[u].push_back(std::make_pair(r, v));
  }

  void Graph::add_edge(const std::string &u_name, const int r, const std::string &v_name) {
    if (!store_node_names) {
      throw std::runtime_error("Cannot add edge by name as store_node_names is false");
    }
    add_edge(node_to_index_.at(u_name), r, node_to_index_.at(v_name));
  }

  void Graph::change_node_colour(const int u, const int new_colour) { nodes[u] = new_colour; }

  void Graph::change_node_colour(const std::string &node_name, const int new_colour) {
    if (!store_node_names) {
      throw std::runtime_error("Cannot change node colour by name as store_node_names is false");
    }
    change_node_colour(node_to_index_.at(node_name), new_colour);
  }

  std::string Graph::get_node_name(const int u) const {
    if (!store_node_names) {
      throw std::runtime_error("Cannot get node name as store_node_names is false");
    }
    return index_to_node_[u];
  }

  int Graph::get_node_index(const std::string &node_name) const {
    return node_to_index_.at(node_name);
  }

  int Graph::get_n_edges() const {
    int ret = 0;
    for (size_t u = 0; u < nodes.size(); u++) {
      ret += edges[u].size();
    }
    return ret;
  }

  std::string Graph::to_string() const {
    std::string ret = "<Graph with " + std::to_string(nodes.size()) + " nodes and " +
                      std::to_string(get_n_edges()) + " edges>";
    return ret;
  }

  void Graph::dump() const {
    std::cout << nodes.size() << " nodes" << std::endl;
    for (size_t u = 0; u < nodes.size(); u++) {
      std::cout << u;
      if (store_node_names) {
        std::cout << " : " << get_node_name(u);
      }
      std::cout << " : " << nodes[u] << std::endl;
    }

    std::cout << get_n_edges() << " edges" << std::endl;
    for (size_t u = 0; u < nodes.size(); u++) {
      for (size_t j = 0; j < edges[u].size(); j++) {
        std::cout << u << " " << edges[u][j].first << " " << edges[u][j].second << std::endl;
      }
    }
  }
}  // namespace graph
