#ifndef NGOOSE_WLF_GRAPH_H
#define NGOOSE_WLF_GRAPH_H

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "../states/state.h"
#include "../task.h"

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
  std::vector<int> x;
  std::vector<std::vector<std::pair<int, int>>> neighbours;  // idx, label

  // everything below here only exists once
  std::unordered_map<std::string, int> name_to_idx;
  std::unordered_set<std::string> bool_goals;
  std::unordered_map<std::string, int> pred_to_idx;
  std::vector<std::string> pwl_object_to_name;
  std::vector<int> pwl_object_to_idx;

 public:
  WlfGraph(const std::shared_ptr<pybind11::object> &model, const Task &task);

  WlfGraph(const std::vector<int> &x, 
           const std::vector<std::vector<std::pair<int, int>>> &neighbours);
  virtual ~WlfGraph();

  WlfGraph state_to_graph(const DBState &s, const Task &task) const;

  std::vector<int> get_x() const { return x; }
  std::vector<std::vector<std::pair<int, int>>> get_neighbours() const {
    return neighbours;
  }

  void dump() const;
};
}  // namespace ngoose_graph

#endif
