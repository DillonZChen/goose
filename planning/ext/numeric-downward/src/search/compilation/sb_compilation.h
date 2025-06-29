#ifndef STATE_BASE_MODEL_H
#define STATE_BASE_MODEL_H

#include "../operator_counting/constraint_generator.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "ip_constraint_generator.h"

namespace numeric_helper {
    class NumericTaskProxy;
}

namespace operator_counting {
    class StateBasedModel : public IPConstraintGenerator {
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
        //std::vector<vector<int>> index_opt;
        std::vector<vector<int>> index_fact;
        std::vector<vector<int>> index_noop_fact;
        std::vector<int> goal_index;
        numeric_helper::NumericTaskProxy numeric_task;
        
        void initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                      std::vector<lp::LPConstraint> &constraints);
        void goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                                   std::vector<lp::LPConstraint> &constraints,
                                   int t_max);
        void action_precondition_constraint(const std::shared_ptr<AbstractTask> task,
                                            std::vector<lp::LPConstraint> &constraints,
                                            double infinity, int t_min, int t_max);
        void action_effect_constraint(const std::shared_ptr<AbstractTask> task,
                                      std::vector<lp::LPConstraint> &constraints,
                                      double infinity, int t_min, int t_max);
        void mutex_relaxtion_constraint(const std::shared_ptr<AbstractTask> task,
                                        std::vector<lp::LPConstraint> &constraints,
                                        double infinity, int t_min, int t_max);
        void noop_preconditions_constraint(const std::shared_ptr<AbstractTask> task,
                                           std::vector<lp::LPConstraint> &constraints,
                                           double infinity, int t_min, int t_max);
        void noopt_mutex_constraint(const std::shared_ptr<AbstractTask> task,
                                    std::vector<lp::LPConstraint> &constraints,
                                    double infinity, int t_min, int t_max);
        
        void add_variables(const std::shared_ptr<AbstractTask> task,
                           std::vector<lp::LPVariable> &variables,
                           double infinity, int t_min, int t_max);
    public:
        //void print_solution(std::vector<double> &solution, const std::shared_ptr<AbstractTask> task);
    };
}
#endif
