#ifndef GUROBI_IP_COMPILATION_H
#define GUROBI_IP_COMPILATION_H

#include "../abstract_task.h"
#include "../search_engine.h"
#include "action_cycle_elimination_callback.h"
#include "action_precedence_graph.h"
#include "gurobi_c++.h"
#include "ip_constraint_generator.h"

namespace gurobi_ip_compilation {

/*
    this class create the model and solve one iteration of the ip compilation
    - constraint generator
 */
class GurobiIPCompilation {
  std::vector<std::shared_ptr<GurobiIPConstraintGenerator>>
      constraint_generators;

 private:
  void add_variables(const int t_min, const int t_max);
  void add_mutex_constraints(const int t_min, const int t_max);

  bool add_lazy_constraints;
  bool add_user_cuts;
  int max_num_cuts;
  bool add_one_time_step;
  bool use_linear_effects;
  bool lazy_mutex;
  int node_count;
  ap_float min_action_cost;
  ap_float min_cost_diff;
  bool use_callback;

  const std::shared_ptr<AbstractTask> task;
  GRBEnv *env;
  std::vector<std::vector<GRBVar>> x;
  std::shared_ptr<GRBModel> model;
  std::vector<std::vector<bool>> action_mutex;
  std::shared_ptr<ActionPrecedenceGraph> graph;
  std::shared_ptr<ActionCycleEliminationCallback> callback;

 public:
  GurobiIPCompilation(const options::Options &opts,
                      const std::shared_ptr<AbstractTask> &task);
  ~GurobiIPCompilation();
  void initialize(const int horizon);
  void update(const int horizon);
  void add_sequence_constraint();
  void print_statistics() const;

  ap_float get_min_action_cost();
  ap_float get_min_plan_cost_diff();
  ap_float compute_plan();
  SearchEngine::Plan extract_plan();
};

}  // namespace gurobi_ip_compilation
#endif
