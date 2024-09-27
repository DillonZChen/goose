#include "numeric_state_equation_constraints.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"

using namespace std;
using namespace numeric_helper;

namespace operator_counting {


    void NumericStateEquationConstraints::add_numeric_goals_constraints(std::vector<lp::LPConstraint> &constraints, double infinity){
        for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
            list<int> goals = numeric_task.get_numeric_goals(id_goal);
            if (goals.empty()) continue; // this is not a numeric goal
            for (int id_n_con : goals){
                const LinearNumericCondition& lnc = numeric_task.get_condition(id_n_con);
                lp::LPConstraint constraint(-infinity, infinity);
                for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
                    for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
                        constraint.insert(op_id, lnc.coefficients[n_id]*numeric_task.get_action_eff_list(op_id)[n_id]);
                    }
                }
                if (!constraint.empty()) {
                    index_constraints_goals[id_n_con] = constraints.size();
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void NumericStateEquationConstraints::add_bounds_numeric_variables(std::vector<lp::LPConstraint> &constraints, double infinity){

        for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
            lp::LPConstraint constraint(-infinity, infinity);
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
                constraint.insert(op_id, numeric_task.get_action_eff_list(op_id)[n_id]);
            }
            
            if (!constraint.empty()) {
                index_constraints_variables[n_id] = constraints.size();
                constraints.push_back(constraint);
            }
        }
    }
    
void NumericStateEquationConstraints::initialize_constraints(
    const shared_ptr<AbstractTask> task, vector<lp::LPConstraint> &constraints,
    double infinity) {

    TaskProxy task_proxy(*task);
    verify_no_conditional_effects(task_proxy);
    numeric_task = NumericTaskProxy(task_proxy, false, true, epsilon, precision, infinity);
    
    index_constraints_variables.resize(numeric_task.get_n_numeric_variables());
    index_constraints_goals.assign(numeric_task.get_n_conditions(),-1);
    add_numeric_goals_constraints(constraints,infinity);
    add_bounds_numeric_variables(constraints,infinity);
}

bool NumericStateEquationConstraints::update_constraints(const State &state,
                                                  lp::LPSolver &lp_solver) {
    
    for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
        list<int> goals = numeric_task.get_numeric_goals(id_goal);
        if (goals.empty()) continue; // this is not a numeric goal
        for (int id_n_con : goals){
            const LinearNumericCondition& lnc = numeric_task.get_condition(id_n_con);
            //cout << lnc << endl;
            double lower_bound = -lnc.constant + numeric_task.get_epsilon(id_n_con);
            for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
                int id_num = numeric_task.get_numeric_variable(n_id).id_abstract_task;
                //cout << "\t" << n_id << " " << lnc.coefficients[n_id] << " " << state.nval(id_num) << endl;
                lower_bound -= (lnc.coefficients[n_id]*state.nval(id_num));
            }
            lp_solver.set_constraint_lower_bound(index_constraints_goals[id_n_con], lower_bound);
        }
    }
    
    for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
        int id_num = numeric_task.get_numeric_variable(n_id).id_abstract_task;
        double state_nval = state.nval(id_num);
        double lower_bound = numeric_task.get_numeric_variable(n_id).lower_bound;
        double upper_bound = numeric_task.get_numeric_variable(n_id).upper_bound;

        if (lower_bound > -lp_solver.get_infinity())
            lp_solver.set_constraint_lower_bound(index_constraints_variables[n_id], -state_nval + lower_bound);
        else
            lp_solver.set_constraint_lower_bound(index_constraints_variables[n_id], lower_bound);

        if (upper_bound < lp_solver.get_infinity())
            lp_solver.set_constraint_upper_bound(index_constraints_variables[n_id], -state_nval + upper_bound);
        else
            lp_solver.set_constraint_upper_bound(index_constraints_variables[n_id], upper_bound);
    }

    return false;
}

static shared_ptr<ConstraintGenerator> _parse(OptionParser &parser) {
    parser.document_synopsis(
                             "Delete relaxation constraints",
                             "For details, see" + utils::format_paper_reference(
                                                                                {"Chiara Piacentini", "Margarita P. Castro", "André Augusto Ciré", "J. Christopher Beck"},
                                                                                "Linear and Integer Programming-Based Heuristics for Cost-Optimal Numeric Planning.",
                                                                                "",
                                                                                "AAAI",
                                                                                "6254-6261",
                                                                                "2018"));
    parser.add_option<ap_float>("epsilon", "small value added to strict inequalities", "0");
    parser.add_option<ap_float>("precision", "values less than this value are considered as zero", "0.000001");
    
    if (parser.dry_run())
        return nullptr;

    Options opts = parser.parse();
    return make_shared<NumericStateEquationConstraints>(opts);
}

static PluginShared<ConstraintGenerator> _plugin("numeric_state_equation_constraints", _parse);
}
