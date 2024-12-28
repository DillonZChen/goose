#ifndef GRAPH_GRAPH_HPP
#define GRAPH_GRAPH_HPP

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace graph {
  class Graph {
   public:
    Graph(const std::vector<int> &node_colours,
          const std::vector<double> &node_values,
          const std::vector<std::string> &node_names,
          const std::vector<std::vector<std::pair<int, int>>> &edges);

    Graph(const std::vector<int> &node_colours,
          const std::vector<double> &node_values,
          const std::vector<std::vector<std::pair<int, int>>> &edges);

    Graph(const std::vector<int> &node_colours,
          const std::vector<std::string> &node_names,
          const std::vector<std::vector<std::pair<int, int>>> &edges);
          
    Graph(const std::vector<int> &node_colours,
          const std::vector<std::vector<std::pair<int, int>>> &edges);

    Graph(bool store_node_names);

   public:
    // nodes and edges should be read only when used publicly
    // nodes[u] is the initial node colour of u
    std::vector<int> nodes;
    std::vector<double> node_values;

    // (r, v) = edges[u] is an edge from u to v with relation r
    // profiling showed that this is faster than the v = edges[u][r][j] structure
    std::vector<std::vector<std::pair<int, int>>> edges;

    // returns the node index
    int add_node(const std::string &node_name, int colour, double value);
    int add_node(const std::string &node_name, int colour);

    // does not assume undirected graph, so this is called twice for adding undirected edges
    void add_edge(const int u, const int r, const int v);
    void add_edge(const std::string &u_name, const int r, const std::string &v_name);

    void change_node_colour(const int u, const int new_colour);
    void change_node_colour(const std::string &node_name, const int new_colour);
    void change_node_value(const int u, const double new_value);
    void change_node_value(const std::string &node_name, const double new_value);

    std::vector<std::set<int>> get_node_to_neighbours() const;

    std::string get_node_name(const int u) const;

    // assumes we checked that the node exists
    int get_node_index(const std::string &node_name) const;

    int get_n_nodes() const;
    int get_n_edges() const;

    std::string to_string() const;

    // set to false when directly modifying the base graph to prevent excessive memory usage
    void set_store_node_names(bool store_node_names) { this->store_node_names = store_node_names; }

    void dump() const;

   private:
    bool store_node_names;
    std::unordered_map<std::string, int> node_to_index_;
    std::vector<std::string> index_to_node_;
  };
}  // namespace graph

#endif  // GRAPH_GRAPH_HPP
