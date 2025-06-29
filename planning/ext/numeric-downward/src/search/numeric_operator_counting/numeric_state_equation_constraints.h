#ifndef NUMERIC_STATE_EQUATION_CONSTRAINTS_H
#define NUMERIC_STATE_EQUATION_CONSTRAINTS_H

#include "../operator_counting/constraint_generator.h"
#include "numeric_helper.h"

class TaskProxy;

namespace lp {
class LPConstraint;
}

namespace operator_counting {


class NumericStateEquationConstraints : public ConstraintGenerator {
  
    numeric_helper::NumericTaskProxy numeric_task;
    void add_numeric_goals_constraints(std::vector<lp::LPConstraint> &constraints, double infinity);
    void add_bounds_numeric_variables(std::vector<lp::LPConstraint> &constraints, double infinity);
    
    double precision;
    double epsilon;
    std::vector<int> index_constraints_goals;
    std::vector<int> index_constraints_variables;


public:
    NumericStateEquationConstraints(const Options &opts) : precision(opts.get<double>("precision")), epsilon(opts.get<double>("epsilon")) {}

    virtual void initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                        std::vector<lp::LPConstraint> &constraints,
                                        double infinity);
    virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
};
}

#endif
