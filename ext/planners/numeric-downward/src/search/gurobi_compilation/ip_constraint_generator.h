#ifndef GUROBI_CONSTRAINT_GENERATOR_H
#define GUROBI_CONSTRAINT_GENERATOR_H

#include <memory>
#include <vector>

#include "action_precedence_graph.h"
#include "gurobi_c++.h"

class AbstractTask;
namespace gurobi_ip_compilation {

class GurobiIPConstraintGenerator {
 public:
  virtual void initialize(const int horizon,
                          const std::shared_ptr<AbstractTask> task,
                          std::vector<std::vector<bool>> &action_mutex,
                          bool use_linear_effects) = 0;
  virtual void update(const int horizon,
                      const std::shared_ptr<AbstractTask> task,
                      std::shared_ptr<GRBModel> model,
                      std::vector<std::vector<GRBVar>> &x) = 0;
  virtual void add_action_precedence(
      const std::shared_ptr<AbstractTask> task,
      std::vector<std::vector<bool>> &action_precedence,
      std::vector<std::vector<bool>> &action_mutex) {}
  virtual void dump() {}
};
}  // namespace gurobi_ip_compilation

#endif
