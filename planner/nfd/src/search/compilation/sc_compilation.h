#ifndef STATE_CHANGE_MODEL_H
#define STATE_CHANGE_MODEL_H

#include "../operator_counting/constraint_generator.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "ip_constraint_generator.h"

namespace numeric_helper {
    class NumericTaskProxy;
}

namespace operator_counting {
    class StateChangeModel : public IPConstraintGenerator {

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
        std::vector<vector<int>> index_a;
        std::vector<vector<int>> index_pa;
        std::vector<vector<int>> index_pd;
        std::vector<vector<int>> index_mant;
        std::vector<int> goal_index;
        std::vector<set<int>> pnd;
        std::vector<set<int>> anp;
        std::vector<set<int>> pd;
        numeric_helper::NumericTaskProxy numeric_task;
        void add_variables(const std::shared_ptr<AbstractTask> task,
                           std::vector<lp::LPVariable> &variables,
                           double infinity, int t_min, int t_max);
        void create_sets(const std::shared_ptr<AbstractTask> task);
        
        // constraints
        void initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                      std::vector<lp::LPConstraint> &constraints);
        void goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                                   std::vector<lp::LPConstraint> &constraints,
                                   int t_max);
        void update_state_change_constraint(const std::shared_ptr<AbstractTask> task,
                                            std::vector<lp::LPConstraint> &constraints,
                                            double infinity, int t_min, int t_max);
        void precondition_effect_constraint(const std::shared_ptr<AbstractTask> task,
                                            std::vector<lp::LPConstraint> &constraints,
                                            double infinity, int t_min, int t_max);
        void mutex_proposition_constraint(const std::shared_ptr<AbstractTask> task,
                                     std::vector<lp::LPConstraint> &constraints,
                                     double infinity, int t_min, int t_max);
        
        void flow_constraint(const std::shared_ptr<AbstractTask> task,
                                          std::vector<lp::LPConstraint> &constraints,
                                          double infinity, int t_min, int t_max);
        void mutex_relaxtion_constraint(const std::shared_ptr<AbstractTask> task,
                                        std::vector<lp::LPConstraint> &constraints,
                                        double infinity, int t_min, int t_max);
    public:
                //void print_solution(std::vector<double> &solution, const std::shared_ptr<AbstractTask> task);
    };
}
#endif
