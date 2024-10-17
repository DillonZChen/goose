#ifndef NGOOSE_WLF_GRAPH_H
#define NGOOSE_WLF_GRAPH_H

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "../globals.h"

#include <algorithm>
#include <chrono>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ngoose_wlf_graph {
class WlfGraph {
 protected:
  const std::shared_ptr<pybind11::object> model;
  
  // this is the important stuff
  std::vector<int> x_cat;
  std::vector<ap_float> x_con;
  std::vector<std::vector<std::pair<int, int>>> neighbours;  // idx, label

  // everything below here only exists once
  std::unordered_map<std::string, int> name_to_idx;
  std::unordered_set<std::string> bool_goals;
  std::vector<std::string> num_vars;

  std::unordered_map<std::string, std::pair<int, std::vector<int>>>
      fact_to_cat_offset_and_obj_indices;
  std::vector<int> fluent_node_indices;

 public:
  WlfGraph(const std::shared_ptr<pybind11::object> &model,
        const std::unordered_map<std::string,
                                 std::pair<std::string, std::vector<std::string>>>
            fact_to_pred_objects,
        const std::vector<std::string> &fluent_names);

  WlfGraph(const std::vector<int> &x_cat, const std::vector<ap_float> &x_con,
        const std::vector<std::vector<std::pair<int, int>>> &neighbours);
  virtual ~WlfGraph();

  WlfGraph state_to_graph(const std::vector<std::string> &bool_vals,
                       const std::vector<ap_float> &num_vals) const;

  std::vector<int> get_x_cat() const { return x_cat; }
  std::vector<ap_float> get_x_con() const { return x_con; }
  std::vector<std::vector<std::pair<int, int>>> get_neighbours() const {
    return neighbours;
  }

  void dump() const;
};
}  // namespace ngoose_graph

#endif
