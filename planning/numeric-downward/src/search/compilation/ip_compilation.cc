#include "ip_compilation.h"
#include "../option_parser.h"
#include "../plugin.h"

using namespace std;
using namespace ip_compilation;

IPCompilation::IPCompilation(const Options &opts, const std::shared_ptr<AbstractTask> &t) : constraint_generators(
                                                                                                                  opts.get_list<shared_ptr<IPConstraintGenerator>>("ipmodel")), lp_solver(lp::LPSolverType(opts.get_enum("lpsolver")),lp::LPConstraintType(opts.get_enum("lprelaxation"))), task(t) {
    // options here
}
void IPCompilation::initialize(int t) {
    double infinity = lp_solver.get_infinity();
    vector<vector<int>> * index_opt = new vector<vector<int>>();
    for (auto generator : constraint_generators) {
        generator->set_max_time(t);
        generator->set_min_time(0);
        generator->set_index_opt(index_opt);
        generator->initialize_variables(task, variables, infinity);
    }
    
    for (auto generator : constraint_generators) {
        generator->set_index_opt(index_opt);
        generator->initialize_constraints(task, constraints, infinity);
    }
    lp_solver.load_problem(lp::LPObjectiveSense::MINIMIZE, variables, constraints);
}

ap_float IPCompilation::get_min_action_cost(){
    TaskProxy task_proxy(*task);
    OperatorsProxy ops = task_proxy.get_operators();
    ap_float min_cost = 9999999.;
    for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
        const OperatorProxy &op = ops[op_id];
        int cost = op.get_cost();
        string name = op.get_name();
        if (name.find("forget")==0)
            continue;
        if (cost < min_cost)
            min_cost = cost;
    }
    return min_cost;
}

bool IPCompilation::contains_forget(){
    TaskProxy task_proxy(*task);
    OperatorsProxy ops = task_proxy.get_operators();
    ap_float min_cost = 9999999.;
    for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
        const OperatorProxy &op = ops[op_id];
        string name = op.get_name();
        if (name.find("forget")==0)
            return true;
    }
    return false;
}

ap_float IPCompilation::compute_plan(const int horizon, const double time){
    for (auto generator : constraint_generators) {
        double infinity = lp_solver.get_infinity();
        bool dead_end = generator->update_constraints(horizon, lp_solver,task,variables,infinity,constraints);
        if (dead_end) {
            lp_solver.clear_temporary_constraints();
            return -1;
        }
    }
    cout << "updating " << horizon << " remaining time " << time << endl;
    lp_solver.clear_temporary_constraints();
    lp_solver.load_problem(lp::LPObjectiveSense::MINIMIZE, variables, constraints);
    
    ap_float result;
    lp_solver.set_time_limit(time);
    lp_solver.solve();
    if (lp_solver.has_optimal_solution()) {
        double epsilon = 0.01;
        double objective_value = lp_solver.get_objective_value();
        result = ceil(objective_value - epsilon);
    } else {
        result = -1;
    }
    lp_solver.clear_temporary_constraints();
    return result;
    
}

void IPCompilation::print_plan(){
    ap_float result;
    double epsilon = 0.01;
    double objective_value = lp_solver.get_objective_value();
    result = ceil(objective_value - epsilon);   vector<double> solution = lp_solver.extract_solution();
    for (auto generator : constraint_generators){
        generator->print_solution(solution, task);
    }
    cout << "Plan cost: " << result << endl;
}

