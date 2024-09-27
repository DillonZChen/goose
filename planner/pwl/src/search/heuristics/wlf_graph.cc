#include "wlf_graph.h"
#include <iostream>

namespace ngoose_wlf_graph {

WlfGraph::WlfGraph(const std::shared_ptr<pybind11::object> &model, const Task &task)
    : model(model) {

  std::cout << "getting name to idx" << std::endl;
  name_to_idx =
      model->attr("get_name_to_idx")().cast<std::unordered_map<std::string, int>>();

  std::cout << "getting x" << std::endl;
  x = model->attr("get_x_cat")().cast<std::vector<int>>();

  std::cout << "getting neighbours" << std::endl;
  neighbours = std::vector<std::vector<std::pair<int, int>>>();
  for (size_t i = 0; i < x.size(); i++) {
    neighbours.push_back(
        model->attr("get_neighbours")(i).cast<std::vector<std::pair<int, int>>>());
  }

  std::cout << "getting bool goals" << std::endl;
  bool_goals = model->attr("get_bool_goals")().cast<std::unordered_set<std::string>>();

  std::cout << "getting get_pred_to_idx" << std::endl;
  pred_to_idx =
      model->attr("get_pred_to_idx")().cast<std::unordered_map<std::string, int>>();

  std::cout << "computing pwl_object_to_name" << std::endl;
  pwl_object_to_name = std::vector<std::string>();
  for (size_t i = 0; i < task.objects.size(); i++) {
    pwl_object_to_name.push_back(task.objects[i].get_name());
  }

  std::cout << "computing pwl_object_to_idx" << std::endl;
  pwl_object_to_idx = std::vector<int>();
  for (size_t i = 0; i < task.objects.size(); i++) {
    pwl_object_to_idx.push_back(name_to_idx.at(pwl_object_to_name[i]));
  }

  std::cout << "WLF graph init done" << std::endl;
}

WlfGraph::WlfGraph(const std::vector<int> &x,
                   const std::vector<std::vector<std::pair<int, int>>> &neighbours)
    : x(x), neighbours(neighbours) {}

WlfGraph::~WlfGraph() {}

WlfGraph WlfGraph::state_to_graph(const DBState &s, const Task &task) const {
  std::vector<int> x_ret = std::vector<int>(this->x);
  std::vector<std::vector<std::pair<int, int>>> neighbours_ret =
      std::vector<std::vector<std::pair<int, int>>>(this->neighbours);

  // nullary facts
  const auto& nullary_atoms = s.get_nullary_atoms();
  for (size_t j = 0; j < nullary_atoms.size(); ++j) {
    if (nullary_atoms[j]) {
      std::string pred = task.predicates[j].get_name();
      int cat_offset = 1 + 3 * pred_to_idx.at(pred);

      std::string fact = pred + "()";
      if (bool_goals.count(fact)) {
        int idx = name_to_idx.at(fact);
        x_ret[idx] = cat_offset + 0;
      } else {
        // nullary atoms have no objects and hence no neighbours to add
        x_ret.push_back(cat_offset + 2);
        neighbours_ret.push_back(std::vector<std::pair<int, int>>());
      }
    }
  }

  // facts
  const auto& relations = s.get_relations();
  for (size_t i = 0; i < relations.size(); ++i) {
    std::string pred = task.predicates[i].get_name();
    if (!pred_to_idx.count(pred)) {
      // skip predicates such as = and type predicates
      continue;
    }
    int cat_offset = 1 + 3 * pred_to_idx.at(pred);
    for (const GroundAtom &pwl_object_indices : relations[i].tuples) {
      std::string fact = pred + "(";
      int n_objects = pwl_object_indices.size();
      for (int i = 0; i < n_objects - 1; i++) {
        fact += pwl_object_to_name[pwl_object_indices[i]] + ", ";
      }
      fact += pwl_object_to_name[pwl_object_indices[n_objects - 1]] + ")";

      if (bool_goals.count(fact)) {
        int idx = name_to_idx.at(fact);
        x_ret[idx] = cat_offset + 0;
      } else {
        // nodes and edges are only added here
        size_t idx = x_ret.size();  // this must be before the push_back later
        x_ret.push_back(cat_offset + 2);

        neighbours_ret.push_back(std::vector<std::pair<int, int>>(n_objects));
        for (int i = 0; i < n_objects; i++) {
          int obj_idx = pwl_object_to_idx[pwl_object_indices[i]];
          neighbours_ret[obj_idx].push_back(std::make_pair(idx, i));  // this exists
          neighbours_ret[idx][i] = std::make_pair(obj_idx, i);        // this new
        }
      }
    }
  }

  return WlfGraph(x_ret, neighbours_ret);
}

void WlfGraph::dump() const {
  std::cout << "x" << std::endl;
  for (size_t i = 0; i < x.size(); i++) {
    std::cout << i << "_cat: " << x[i] << std::endl;
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
