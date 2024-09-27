#ifndef WLF_HEURISTIC_H
#define WLF_HEURISTIC_H

#include "wlf_graph.h"
#include "pybind_heuristic.h"

#include <chrono>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace wlf_heuristic {
class WlfHeuristic : public pybind_heuristic::PybindHeuristic {
 protected:
  virtual ap_float compute_heuristic(const GlobalState &global_state) override;
  virtual std::vector<ap_float>
  compute_heuristic_batch(const std::vector<GlobalState> &states) override;

  int cat_iterations_;
  int con_iterations_;
  int n_models_;
  std::vector<bool> round_;
  std::vector<std::vector<ap_float>> weights_list_;
  std::unordered_map<std::string, int> hash_;
  int n_init_features_;
  int n_con_features_;
  int n_cat_features_;
  int n_features_;
  bool numeric;

  std::vector<int> heuristic_models_;
  std::vector<int> deadend_models_;
  std::vector<int> policy_models_;

  std::unordered_map<std::string, int> schema_to_index_;

  std::chrono::duration<double> start_time, end_time;
  double graph_time;
  double wl_time;
  double linear_time;

  std::shared_ptr<ngoose_wlf_graph::WlfGraph> graph;

  const ngoose_wlf_graph::WlfGraph state_to_graph(const GlobalState &global_state);
  std::vector<ap_float> compute_features(const ngoose_wlf_graph::WlfGraph &state_graph);

  ap_float estimate(const std::vector<ap_float> x, int model_index);
  bool predict_binary(const std::vector<ap_float> x, int model_index);
  ap_float predict_heuristic(const std::vector<ap_float> x, int model_index);

  virtual void print_statistics() override;

 public:
  WlfHeuristic(const options::Options &options);
  virtual ~WlfHeuristic();

  std::vector<ap_float> compute_features(const GlobalState &global_state);
  std::vector<ap_float> compute_multi_heuristics(const GlobalState &global_state);
  ap_float compute_heuristic(const std::vector<ap_float> x);
  std::vector<bool> compute_pref_schemata(const std::vector<ap_float> x);

  int n_heuristics() const { return n_models_; }
  std::unordered_map<std::string, int> get_schema_to_index() const {
    return schema_to_index_;
  }
};

}  // namespace wlf_heuristic

#endif
