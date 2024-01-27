#ifndef INSTANCE_LEARNING_GRAPH_H
#define INSTANCE_LEARNING_GRAPH_H

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "coloured_graph.h"

class InstanceLearningGraph : public CGraph {
 public:
  explicit InstanceLearningGraph(const std::string &path);

  InstanceLearningGraph(const Edges &edges, const std::vector<int> &colour);

  std::shared_ptr<CGraph>
  predicate_arguments_to_graph(const PredicateArguments pred_args) override;

 private:
  static const int F_POS_GOAL = 0;
  static const int T_POS_GOAL = 1;
  static const int T_NON_GOAL = 2;
  std::unordered_map<std::string, int> pred_to_idx;
};

#endif  // INSTANCE_LEARNING_GRAPH_H
