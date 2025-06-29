#ifndef INVARIANT_CONSTRAINTS_H
#define INVARIANT_CONSTRAINTS_H

#include "../operator_counting/constraint_generator.h"
#include "../operator_counting/state_equation_constraints.h"
#include "numeric_helper.h"

class TaskProxy;

namespace lp {
class LPConstraint;
}

namespace operator_counting {

    struct Proposition;
    
class InvariantConstraints : public ConstraintGenerator {
  
   
    std::vector<std::vector<Proposition>> propositions;

    void build_propositions(const TaskProxy &task_proxy);
    void add_constraints(std::vector<lp::LPConstraint> &constraints, double infinity);
    void add_indices_to_constraint(lp::LPConstraint &constraint,
                                   const set<int> &indices,
                                   double coefficient);
    std::vector<std::vector<std::set<int>>> index_constraints;
    std::vector<std::vector<std::set<int>>> index_sometimes_deleted;
    std::set<int> index_constraints_set;
public:
    virtual void initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                        std::vector<lp::LPConstraint> &constraints,
                                        double infinity);
    virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
};
}

#endif
