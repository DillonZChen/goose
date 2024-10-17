#ifndef GUROBI_STATE_CHANGE_MODEL_H
#define GUROBI_STATE_CHANGE_MODEL_H

#include <set>
#include <vector>

#include "../numeric_landmarks/landmark_factory_scala.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "gurobi_c++.h"
#include "ip_constraint_generator.h"

namespace gurobi_ip_compilation {
class GurobiStateChangeModel : public GurobiIPConstraintGenerator {
 protected:
  bool use_landmark;
  int current_horizon;
  std::vector<std::vector<GRBVar>> y_a;
  std::vector<std::vector<GRBVar>> y_pa;
  std::vector<std::vector<GRBVar>> y_pd;
  std::vector<std::vector<GRBVar>> y_m;
  std::vector<set<int>> pnd;
  std::vector<set<int>> anp;
  std::vector<set<int>> pd;
  numeric_helper::NumericTaskProxy numeric_task;
  std::unique_ptr<landmarks::LandmarkFactoryScala> factory;
  set<int> fact_landmarks;
  set<int> action_landmarks;

  void create_sets(const std::shared_ptr<AbstractTask> task,
                   bool use_linear_effects);

  void initialize_landmark(const std::shared_ptr<AbstractTask> task);

  void add_variables(const std::shared_ptr<AbstractTask> task,
                     std::shared_ptr<GRBModel> model, int t_min, int t_max,
                     bool first);

  // constraints
  void initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                std::shared_ptr<GRBModel> model);
  void goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                             std::shared_ptr<GRBModel> model, int t_max,
                             bool first);
  void update_state_change_constraint(const std::shared_ptr<AbstractTask> task,
                                      std::shared_ptr<GRBModel> model,
                                      int t_min, int t_max);
  void effect_constraint(const std::shared_ptr<AbstractTask> task,
                         std::shared_ptr<GRBModel> model,
                         std::vector<std::vector<GRBVar>> &x, int t_min,
                         int t_max);
  void landmark_constraint(const std::shared_ptr<AbstractTask> task,
                           std::shared_ptr<GRBModel> model,
                           std::vector<std::vector<GRBVar>> &x, int t_min,
                           int t_max, bool first);
  virtual void initialize_mutex(const std::shared_ptr<AbstractTask> task,
                                std::vector<std::vector<bool>> &action_mutex);
  virtual void precondition_constraint(const std::shared_ptr<AbstractTask> task,
                                       std::shared_ptr<GRBModel> model,
                                       std::vector<std::vector<GRBVar>> &x,
                                       int t_min, int t_max);

 public:
  GurobiStateChangeModel(const options::Options &opts);

  virtual void initialize(const int horizon,
                          const std::shared_ptr<AbstractTask> task,
                          std::vector<std::vector<bool>> &action_mutex,
                          bool use_linear_effects) override;
  virtual void update(const int horizon,
                      const std::shared_ptr<AbstractTask> task,
                      std::shared_ptr<GRBModel> model,
                      std::vector<std::vector<GRBVar>> &x) override;
};
}  // namespace gurobi_ip_compilation
#endif
