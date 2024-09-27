#include "wlf_graph.h"
#include <iostream>

namespace ngoose_wlf_graph {

WlfGraph::WlfGraph(const std::shared_ptr<pybind11::object> &model,
             const std::unordered_map<std::string,
                                      std::pair<std::string, std::vector<std::string>>>
                 fact_to_pred_objects,
             const std::vector<std::string> &fluent_names)
    : model(model) {

  std::cout << "getting name to idx" << std::endl;
  name_to_idx =
      model->attr("get_name_to_idx")().cast<std::unordered_map<std::string, int>>();

  std::cout << "getting x" << std::endl;
  x_cat = model->attr("get_x_cat")().cast<std::vector<int>>();
  x_con = model->attr("get_x_con")().cast<std::vector<ap_float>>();

  std::cout << "getting neighbours" << std::endl;
  neighbours = std::vector<std::vector<std::pair<int, int>>>();
  for (size_t i = 0; i < x_cat.size(); i++) {
    neighbours.push_back(
        model->attr("get_neighbours")(i).cast<std::vector<std::pair<int, int>>>());
  }

  std::cout << "getting bool goals" << std::endl;
  bool_goals = model->attr("get_bool_goals")().cast<std::unordered_set<std::string>>();

  std::cout << "getting fluent node indices" << std::endl;
  num_vars = model->attr("get_fluents")().cast<std::vector<std::string>>();

  std::cout << "getting pred to idx" << std::endl;
  std::unordered_map<std::string, int> pred_to_idx =
      model->attr("get_pred_to_idx")().cast<std::unordered_map<std::string, int>>();
  fluent_node_indices = std::vector<int>();
  for (const std::string &name : fluent_names) {
    if (name_to_idx.count(name) == 0) {
      std::cout << "Fluent name not found in name_to_idx: " << name << std::endl;
      exit(-1);
    }
    fluent_node_indices.push_back(name_to_idx.at(name));
  }

  // preprocessing to access indices for fact nodes faster
  std::cout << "preprocessing index access" << std::endl;
  for (auto it = fact_to_pred_objects.begin(); it != fact_to_pred_objects.end(); it++) {
    std::string fact = it->first;
    std::string pred = it->second.first;
    std::vector<std::string> objs = it->second.second;

    if (pred_to_idx.count(pred) == 0) {
      std::cout << "Pred not found in pred_to_idx: " << pred << std::endl;
      exit(-1);
    }
    int cat_offset = 1 + 3 * pred_to_idx.at(pred);

    std::vector<int> obj_idxs = std::vector<int>();
    for (const std::string &obj : objs) {
      if (name_to_idx.count(obj) == 0) {
        std::cout << "Obj not found in name_to_idx: " << obj << std::endl;
        exit(-1);
      }
      obj_idxs.push_back(name_to_idx.at(obj));
    }
    fact_to_cat_offset_and_obj_indices[fact] = std::make_pair(cat_offset, obj_idxs);
  }

  std::cout << "WLF graph init done" << std::endl;
}

WlfGraph::WlfGraph(const std::vector<int> &x_cat, const std::vector<ap_float> &x_con,
             const std::vector<std::vector<std::pair<int, int>>> &neighbours)
    : x_cat(x_cat), x_con(x_con), neighbours(neighbours) {}

WlfGraph::~WlfGraph() {}

WlfGraph WlfGraph::state_to_graph(const std::vector<std::string> &bool_vals,
                            const std::vector<ap_float> &num_vals) const {
  std::vector<int> x_cat_ret = std::vector<int>(this->x_cat);
  std::vector<ap_float> x_con_ret = std::vector<ap_float>(this->x_con);
  std::vector<std::vector<std::pair<int, int>>> neighbours_ret =
      std::vector<std::vector<std::pair<int, int>>>(this->neighbours);

  // facts
  for (const std::string &var : bool_vals) {
    const std::pair<int, std::vector<int>> &cat_offset_and_obj_indices =
        fact_to_cat_offset_and_obj_indices.at(var);
    const int cat_offset = cat_offset_and_obj_indices.first;
    if (bool_goals.count(var)) {
      int idx = name_to_idx.at(var);
      x_cat_ret[idx] = cat_offset + 0;
    } else {
      // nodes and edges are only added here
      int idx = x_cat_ret.size();  // this must be before the push_back later
      x_cat_ret.push_back(cat_offset + 2);
      x_con_ret.push_back(0.0);
      int n_objs = cat_offset_and_obj_indices.second.size();
      neighbours_ret.push_back(std::vector<std::pair<int, int>>(n_objs));
      for (int i = 0; i < n_objs; i++) {
        int obj_idx = cat_offset_and_obj_indices.second[i];
        neighbours_ret[obj_idx].push_back(std::make_pair(idx, i));  // this exists
        neighbours_ret[idx][i] = std::make_pair(obj_idx, i);        // this new
      }
    }
  }

  // fluents
  for (size_t i = 0; i < num_vals.size(); i++) {
    x_con_ret[fluent_node_indices[i]] = num_vals[i];
  }

  // numeric goals
  std::vector<std::tuple<int, int, ap_float>> numeric_goals =
      model->attr("get_num_goal_updates")(bool_vals, num_vals)
          .cast<std::vector<std::tuple<int, int, ap_float>>>();
  for (const std::tuple<int, int, ap_float> &goal : numeric_goals) {
    int idx = std::get<0>(goal);
    x_cat_ret[idx] = std::get<1>(goal);
    x_con_ret[idx] = std::get<2>(goal);
  }

  return WlfGraph(x_cat_ret, x_con_ret, neighbours_ret);
}

void WlfGraph::dump() const {
  std::cout << "x_cat" << std::endl;
  for (size_t i = 0; i < x_cat.size(); i++) {
    std::cout << i << "_cat: " << x_cat[i] << std::endl;
  }
  std::cout << std::endl;

  std::cout << "x_con" << std::endl;
  for (size_t i = 0; i < x_con.size(); i++) {
    std::cout << i << "_con: " << x_con[i] << std::endl;
  }
  std::cout << std::endl;

  std::cout << "neighbours" << std::endl;
  for (size_t i = 0; i < neighbours.size(); i++) {
    std::cout << i << ": ";
    std::vector<std::pair<int, int>> neighbours_i = neighbours[i];
    sort(neighbours_i.begin(), neighbours_i.end());
    for (const std::pair<int, int> &neighbour : neighbours_i) {
      std::cout << neighbour.first << " " << neighbour.second << " ";
    }
    std::cout << std::endl;
  }
}

}  // namespace ngoose_wlf_graph
