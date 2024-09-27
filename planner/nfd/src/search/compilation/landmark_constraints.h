#ifndef LANDMARK_CONSTRAINTS_H
#define LANDMARK_CONSTRAINTS_H

#include "ip_constraint_generator.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../numeric_landmarks/landmark_factory_scala.h"


namespace numeric_helper {
    class NumericTaskProxy;
}

namespace operator_counting {
    class LandmarkConstraints : public IPConstraintGenerator {
        
        virtual void initialize_variables(
                                          const std::shared_ptr<AbstractTask> task,
                                          std::vector<lp::LPVariable> &variables,
                                          double infinity);
        virtual void initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                            std::vector<lp::LPConstraint> &constraints,
                                            double infinity);
        virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
        virtual bool update_constraints(const int horizon,
                                        lp::LPSolver &lp_solver,const std::shared_ptr<AbstractTask> task,
                                        std::vector<lp::LPVariable> &variables,
                                        double infinity,std::vector<lp::LPConstraint> & constraints);
        
    private:
        std::vector<vector<int>> index_numeric_var;
        std::vector<int> goal_index;

        numeric_helper::NumericTaskProxy numeric_task;
        landmarks::LandmarkFactoryScala *factory;

        void landmark_constraints(const std::shared_ptr<AbstractTask> task,
                                  std::vector<lp::LPConstraint> &constraints,
                                  double infinity, int t_max);
        set<int> fact_landmarks;
        set<int> action_landmarks;
        std::vector<int> landmark_constraints_index;
 public:
        void print_solution(std::vector<double> &solution, const std::shared_ptr<AbstractTask> task){ };
    };
}
#endif
