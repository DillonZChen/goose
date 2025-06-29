#ifndef GUROBI_SAS_STATE_CHANGE_MODEL_H
#define GUROBI_SAS_STATE_CHANGE_MODEL_H

#include <set>
#include <vector>

#include "../numeric_landmarks/landmark_factory_scala.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "gurobi_c++.h"
#include "ip_constraint_generator.h"

namespace gurobi_ip_compilation {
class GurobiSASStateChangeModel : public GurobiIPConstraintGenerator {
 protected:
  bool use_landmark;
  int current_horizon;
  std::vector<std::vector<std::vector<std::vector<GRBVar>>>> y;
  std::vector<std::vector<std::vector<std::set<int>>>> sc_actions;
  std::vector<std::vector<std::set<int>>> add_actions;
  std::vector<std::vector<std::pair<int, std::set<int>>>> effs_actions;
  std::vector<std::vector<std::set<int>>> pre_actions;
  std::vector<std::vector<bool>> sc_facts;
  numeric_helper::NumericTaskProxy numeric_task;
  std::unique_ptr<landmarks::LandmarkFactoryScala> factory;
  set<int> fact_landmarks;
  set<int> action_landmarks;

  void create_sets(const std::shared_ptr<AbstractTask> task,
                   bool use_linear_effects);

  void initialize_landmark(const std::shared_ptr<AbstractTask> task);

  void add_variables(const std::shared_ptr<AbstractTask> task,
                     std::shared_ptr<GRBModel> model, int t_min, int t_max);

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
  void mutex_proposition_constraint(const std::shared_ptr<AbstractTask> task,
                                    std::shared_ptr<GRBModel> model, int t_min,
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
  GurobiSASStateChangeModel(const options::Options &opts);

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
