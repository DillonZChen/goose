#ifndef IP_GENERATOR_H
#define IP_GENERATOR_H

#include "../operator_counting/constraint_generator.h"


namespace operator_counting {
    class IPConstraintGenerator : public ConstraintGenerator {
    public:
        virtual void print_solution(std::vector<double> &solution, const std::shared_ptr<AbstractTask> task);
        
        void set_index_opt(std::vector<std::vector<int>>* io){
            index_opt = io;
        }

        virtual bool update_constraints(const int horizon,
                                        lp::LPSolver &lp_solver,const std::shared_ptr<AbstractTask> task,
                                        std::vector<lp::LPVariable> &variables,
                                        double infinity,std::vector<lp::LPConstraint> & constraints){return false;};
        
        
        void set_max_time(const int t) {
            t_max = t;
        }
        
        void set_min_time(const int t) {
            t_min = t;
        }
        
    protected:
        
        std::vector<std::vector<int>> *index_opt;
        int t_max = 0;
        int t_min = 0;
        
    };
}
#endif
