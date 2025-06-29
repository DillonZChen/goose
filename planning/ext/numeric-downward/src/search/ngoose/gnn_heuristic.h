#ifndef GNN_HEURISTIC_H
#define GNN_HEURISTIC_H

#include "gnn_graph.h"
#include "pybind_heuristic.h"

#include <chrono>

namespace gnn_heuristic {
class GnnHeuristic : public pybind_heuristic::PybindHeuristic {
 protected:
  GnnGraph graph;

  int d_;
  int bool_label_offset_;
  int num_goal_offset_;

  // these variables are copied from ngoose/graph.h ...
  std::unordered_map<std::string, int> name_to_idx;
  std::unordered_set<std::string> bool_goals;

  std::unordered_map<std::string, std::pair<int, std::vector<int>>>
      fact_to_cat_offset_and_obj_indices;
  std::vector<int> fluent_node_indices;
  std::vector<int> fluent_node_columns;

  std::chrono::duration<double> start_time, end_time;
  double graph_time;

  GnnGraph state_to_graph(const std::vector<std::string> &bool_vals,
                          const std::vector<ap_float> &num_vals) const;

  virtual ap_float compute_heuristic(const GlobalState &global_state) override;
  virtual std::vector<ap_float>
  compute_heuristic_batch(const std::vector<GlobalState> &states) override;

  virtual void print_statistics() override;

 public:
  GnnHeuristic(const options::Options &options);
  virtual ~GnnHeuristic();
};

}  // namespace gnn_heuristic

#endif
