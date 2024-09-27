#ifndef ACTION_CYCLE_ELIMINATION_CALLBACK_H_
#define ACTION_CYCLE_ELIMINATION_CALLBACK_H

#include <memory>

#include "action_precedence_graph.h"
#include "gurobi_c++.h"

namespace gurobi_ip_compilation {

class ActionCycleEliminationCallback : public GRBCallback {
 private:
  int max_num_cuts;
  bool add_user_cut;
  bool add_one_time_step;
  int num_added_cuts;
  int num_added_constraints;
  const std::vector<std::vector<GRBVar>> &x;
  std::shared_ptr<ActionPrecedenceGraph> graph;
  std::vector<double> x_values;
  std::vector<std::vector<int>> cycles;

 protected:
  void callback();

 public:
  ActionCycleEliminationCallback(int max_num_cuts, bool add_user_cut, bool add_one_time_step,
                                 const std::vector<std::vector<GRBVar>> &x_,
                                 std::shared_ptr<ActionPrecedenceGraph> graph_)
      : max_num_cuts(max_num_cuts),
        add_user_cut(add_user_cut),
        add_one_time_step(add_one_time_step),
        num_added_cuts(0),
        num_added_constraints(0),
        x(x_),
        graph(graph_),
        x_values(x[0].size()) {}
  int get_num_cuts() const { return num_added_cuts; }
  int get_num_constraints() const { return num_added_constraints; }
};

}  // namespace gurobi_ip_compilation

#endif
