#ifndef OBJECT_PAIR_GRAPH_H
#define OBJECT_PAIR_GRAPH_H

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "coloured_graph.h"

class ObjectPairGraph : public CGraph {
 public:
  explicit ObjectPairGraph(const std::string &path);

  ObjectPairGraph(const Edges &edges, const std::vector<int> &colour);

  std::shared_ptr<CGraph>
  predicate_arguments_to_graph(const PredicateArguments pred_args) override;

 private:
  int largest_predicate_size;
  static const int F_POS_GOAL = 0;
  static const int T_POS_GOAL = 1;
  static const int T_NON_GOAL = 2;
  std::unordered_map<std::string, int> pred_to_idx;
};

#endif  // OBJECT_PAIR_GRAPH_H
