#ifndef GUROBI_SAS_CUT_STATE_CHANGE_MODEL_H
#define GUROBI_SAS_CUT_STATE_CHANGE_MODEL_H

#include <set>
#include <vector>

#include "../numeric_operator_counting/numeric_helper.h"
#include "gurobi_c++.h"
#include "sas_compilation.h"

namespace gurobi_ip_compilation {
class GurobiSASStateChangeModelWithCuts : public GurobiSASStateChangeModel {
 protected:
  virtual void initialize_mutex(
      const std::shared_ptr<AbstractTask> task,
      std::vector<std::vector<bool>> &action_mutex) override;
  virtual void precondition_constraint(const std::shared_ptr<AbstractTask> task,
                                       std::shared_ptr<GRBModel> model,
                                       std::vector<std::vector<GRBVar>> &x,
                                       int t_min, int t_max) override;

  std::vector<std::vector<bool>> action_precedence_inner;

 public:
  GurobiSASStateChangeModelWithCuts(const options::Options &opts);

  virtual void add_action_precedence(
      const std::shared_ptr<AbstractTask> task,
      std::vector<std::vector<bool>> &action_precedence,
      std::vector<std::vector<bool>> &action_mutex) override;
};
}  // namespace gurobi_ip_compilation
#endif
