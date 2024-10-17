#ifndef NGOOSE_GNN_GRAPH_H
#define NGOOSE_GNN_GRAPH_H

#include <iostream>
#include <vector>

typedef std::vector<std::vector<double>> FeatureMatrix;
typedef std::vector<std::vector<std::vector<int>>> EdgeIndices;

struct GnnGraph {
  FeatureMatrix x;
  EdgeIndices edge_indices;

  void dump() {
    int n_features = x[0].size();
    size_t n_edge_labels = edge_indices.size();
    int n_nodes = x.size();
    int n_edges = 0;
    for (size_t i = 0; i < n_edge_labels; i++) {
      n_edges += edge_indices[i][0].size();
    }

    std::cout << "n_features: " << n_features << std::endl;
    std::cout << "n_edge_labels: " << n_edge_labels << std::endl;
    std::cout << "n_nodes: " << n_nodes << std::endl;
    std::cout << "n_edges: " << n_edges << std::endl;
    for (size_t i = 0; i < n_edge_labels; i++) {
      std::cout << "  n_edges_" << i << ": " << edge_indices[i][0].size() << std::endl;
    }

    std::cout << "x:" << std::endl;
    for (int i = 0; i < n_nodes; i++) {
      std::vector<double> row = x[i];
      std::cout << i << ": ";
      for (double val : row) {
        std::cout << val << " ";
      }
      std::cout << std::endl;
    }

    std::cout << "edge_indices:" << std::endl;
    for (size_t i = 0; i < n_edge_labels; i++) {
      std::cout << "label_" << i << std::endl;
      for (const int val : edge_indices[i][0]) {
        std::cout << val << " ";
      }
      std::cout << std::endl;
      for (const int val : edge_indices[i][1]) {
        std::cout << val << " ";
      }
      std::cout << std::endl;
    }
  }
};

#endif
