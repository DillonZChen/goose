#ifndef IP_COMPILATION_H
#define IP_COMPILATION_H

#include "../lp/lp_solver.h"
#include "ip_constraint_generator.h"
#include "../operator_counting/operator_counting_heuristic.h"

using namespace lp;
using namespace operator_counting;


namespace ip_compilation{
    
    /*
        this class create the model and solve one iteration of the ip compilation
        - constraint generator
     */
    enum class ModelType {
        SB, SC, SAS
    };
    
    class IPCompilation{
        std::vector<std::shared_ptr<IPConstraintGenerator>> constraint_generators;
        lp::LPSolver lp_solver;
    public:
        IPCompilation(const options::Options &opts, const std::shared_ptr<AbstractTask> &task);
        ~IPCompilation() {}
        void initialize(int t);
        ap_float get_min_action_cost();
        ap_float compute_plan(const int horizon, const double time);
        bool contains_forget();
        void print_plan();
    protected:
        const std::shared_ptr<AbstractTask> task;
        std::vector<lp::LPVariable> variables;
        std::vector<lp::LPConstraint> constraints;

    };
    
}
#endif


