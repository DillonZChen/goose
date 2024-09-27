#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"
#include "sc_compilation.h"
#include "../numeric_operator_counting/numeric_helper.h"

using namespace std;
using namespace operator_counting;
using namespace numeric_helper;


void StateChangeModel::initialize_variables(const std::shared_ptr<AbstractTask> task,
                                           std::vector<lp::LPVariable> &variables,
                                           double infinity){
    cout << "initializing variables in SC" << endl;
    TaskProxy task_proxy(*task);
    numeric_task = NumericTaskProxy(task_proxy);
    OperatorsProxy ops = task_proxy.get_operators();
    int n_ops = ops.size();
    int n_prop = numeric_task.get_n_propositions();

    index_opt->assign(n_ops,vector<int>(t_max,-1));
    index_a.assign(n_prop, vector<int> (t_max,-1));
    index_pa.assign(n_prop, vector<int> (t_max,-1));
    index_pd.assign(n_prop, vector<int> (t_max,-1));
    index_mant.assign(n_prop, vector<int> (t_max,-1));

    create_sets(task);
    add_variables(task,variables,infinity,t_min,t_max);
}

void StateChangeModel::create_sets(const std::shared_ptr<AbstractTask> task){
    TaskProxy task_proxy(*task);
    OperatorsProxy ops = task_proxy.get_operators();
    int n_prop = numeric_task.get_n_propositions();
    VariablesProxy vars = task_proxy.get_variables();
    
    pnd.assign(n_prop, set<int> ());
    anp.assign(n_prop, set<int> ());
    pd.assign(n_prop, set<int> ());
    

    for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
        const OperatorProxy &op = ops[op_id];
        vector<int> precondition(vars.size(), -1);
        vector<int> effects(vars.size(), -1);
        for (FactProxy condition : op.get_preconditions()) {
            int pre_var_id = condition.get_variable().get_id();
            if (numeric_task.is_numeric_axiom(pre_var_id)) continue;
            precondition[pre_var_id] = condition.get_value();
        }
        for (EffectProxy effect_proxy : op.get_effects()) {
            FactProxy effect = effect_proxy.get_fact();
            int var = effect.get_variable().get_id();
            if (numeric_task.is_numeric_axiom(var)) continue;
            int pre = precondition[var];
            int post = effect.get_value();
            assert(post != -1);
            //assert(pre != post);
            effects[var] = post;
            // if a is e
            if (pre != post){
                // add but not pre
                anp[numeric_task.get_proposition(var,post)].insert(op_id);
            }
        }
        for (FactProxy condition : op.get_preconditions()) {
            int var = condition.get_variable().get_id();
            int pre = condition.get_value();
            int post = effects[var];
            if (numeric_task.is_numeric_axiom(var)) continue;
            if (post == -1 || post == pre){
                pnd[numeric_task.get_proposition(var,pre)].insert(op_id);
            }else if (post != pre){
                pd[numeric_task.get_proposition(var,pre)].insert(op_id);
            }else{
                assert(false);
            }
        }
    }

}

void StateChangeModel::add_variables(
                                    const std::shared_ptr<AbstractTask> task,
                                    std::vector<lp::LPVariable> &variables,
                                    double infinity, int t_min, int t_max){
    TaskProxy task_proxy(*task);
    numeric_task = NumericTaskProxy(task_proxy);
    OperatorsProxy ops = task_proxy.get_operators();
    int n_ops = ops.size();
    int n_prop = numeric_task.get_n_propositions();

    // resize idex
    for (int op_id = 0; op_id < n_ops; ++op_id) {
        (*index_opt)[op_id].resize(t_max,-1);
    }
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        int num_values = numeric_task.get_n_proposition_value(var);
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            index_a[pr_id].resize(t_max,-1);
            index_pa[pr_id].resize(t_max,-1);
            index_pd[pr_id].resize(t_max,-1);
            index_mant[pr_id].resize(t_max,-1);
        }
    }

    // add variables
    for (int t = t_min; t < t_max; ++t) {
        // actions
        for (int op_id = 0; op_id < n_ops; ++op_id) {

            stringstream name;
            name << "x_" << ops[op_id].get_name() << "_" << t;
            (*index_opt)[op_id][t] = variables.size();
            variables.push_back(lp::LPVariable(0,1,ops[op_id].get_cost(), name.str(), lp::LPVariableType::binary));
        }
        // facts
        for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
            int num_values = numeric_task.get_n_proposition_value(var);
            for (int value = 0; value < num_values; ++value) {
                int pr_id = numeric_task.get_proposition(var,value);
                {
                    index_a[pr_id][t] = variables.size();
                    stringstream name;
                    name << "u_a_" << pr_id << "_" << t;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str(), lp::LPVariableType::continous));
                }
                {
                    index_pa[pr_id][t] = variables.size();
                    stringstream name;
                    name << "u_pa_" << pr_id << "_"  << t;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str(), lp::LPVariableType::continous));
                }
                {
                    index_pd[pr_id][t] = variables.size();
                    stringstream name;
                    name << "u_pd_" << pr_id << "_"  << t;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str(), lp::LPVariableType::continous));
                }
                {
                    index_mant[pr_id][t] = variables.size();
                    stringstream name;
                    name << "u_m_" << pr_id << "_"  << t;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str(), lp::LPVariableType::continous));
                }
            }
        }
    }
}


void StateChangeModel::initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                             std::vector<lp::LPConstraint> &constraints,
                                             double infinity){
    
    cout << "initializing constraints for SC" << endl;
    initial_state_constraint(task, constraints);
    goal_state_constraint(task, constraints, t_max);
    update_state_change_constraint(task, constraints, infinity,t_min,t_max);
    precondition_effect_constraint(task, constraints, infinity,t_min,t_max);
    mutex_proposition_constraint(task, constraints, infinity,t_min,t_max);
    mutex_relaxtion_constraint(task, constraints, infinity,t_min,t_max);
    flow_constraint(task, constraints, infinity,t_min,t_max);
}

bool StateChangeModel::update_constraints(const State &state, lp::LPSolver &lp_solver){
    return false;
}

bool StateChangeModel::update_constraints(const int horizon,
                                         lp::LPSolver &lp_solver,const std::shared_ptr<AbstractTask> task,
                                         std::vector<lp::LPVariable> &variables,
                                         double infinity, std::vector<lp::LPConstraint> & constraints){
    cout << "adding constraint from SC" << endl;
    t_min = t_max;
    t_max = horizon;
    add_variables(task,variables,infinity,t_min,t_max);
    update_state_change_constraint(task, constraints, infinity,t_min-1,t_max);
    precondition_effect_constraint(task, constraints, infinity,t_min-1,t_max);
    mutex_proposition_constraint(task, constraints, infinity,t_min,t_max);
    mutex_relaxtion_constraint(task, constraints, infinity,t_min,t_max);
    flow_constraint(task, constraints, infinity,t_min-1,t_max);
    goal_state_constraint(task, constraints, t_max);
    return false;
}

void StateChangeModel::initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                               std::vector<lp::LPConstraint> &constraints){
    
    TaskProxy task_proxy(*task);
    State initial_state =  task_proxy.get_initial_state();
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        int num_values = numeric_task.get_n_proposition_value(var);
        int initial_value = initial_state[var].get_value();
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            double lb = (initial_value == value);
            {
                lp::LPConstraint constraint(lb, lb);
                constraint.insert(index_a[pr_id][0], 1.);
                constraints.push_back(constraint);
            }
            {
                lp::LPConstraint constraint(0, 0);
                constraint.insert(index_pa[pr_id][0], 1.);
                constraints.push_back(constraint);
            }
            {
                lp::LPConstraint constraint(0, 0);
                constraint.insert(index_pd[pr_id][0], 1.);
                constraints.push_back(constraint);
            }
            {
                lp::LPConstraint constraint(0, 0);
                constraint.insert(index_mant[pr_id][0], 1.);
                constraints.push_back(constraint);
            }
        }
    }
}

void StateChangeModel::goal_state_constraint(const std::shared_ptr<AbstractTask> task,
                                            std::vector<lp::LPConstraint> &constraints,
                                            int t_max) {
    TaskProxy task_proxy(*task);
    bool first = goal_index.empty();
    if (first)
        goal_index.assign(task_proxy.get_goals().size(),-1);
    for (size_t id_goal = 0; id_goal < task_proxy.get_goals().size(); ++id_goal) {
        FactProxy goal = task_proxy.get_goals()[id_goal];
        lp::LPConstraint constraint(1., 1.);
        if (numeric_task.is_numeric_axiom(goal.get_variable().get_id())) continue;
        int index_p = numeric_task.get_proposition(goal.get_variable().get_id(),goal.get_value());
        constraint.insert(index_a[index_p][t_max-1], 1.);
        constraint.insert(index_pa[index_p][t_max-1], 1.);
        constraint.insert(index_mant[index_p][t_max-1], 1.);
        if (!numeric_task.numeric_goals_empty(id_goal)) continue; // this is a numeric goal
        if (!constraint.empty()) {
            if (first){
                goal_index[id_goal]= constraints.size();
                constraints.push_back(constraint);
            }else{
                constraints[goal_index[id_goal]] = constraint;
            }
        }
    }
}

void StateChangeModel::update_state_change_constraint(const std::shared_ptr<AbstractTask> task,
                                                      std::vector<lp::LPConstraint> &constraints,
                                                      double infinity, int t_min, int t_max){
    
    TaskProxy task_proxy(*task);
    State initial_state =  task_proxy.get_initial_state();
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        int num_values = numeric_task.get_n_proposition_value(var);
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            for (int t = t_min; t < t_max-1; ++t){
                // pa
                {
                    lp::LPConstraint constraint(0., infinity);
                    constraint.insert(index_pa[pr_id][t+1], -1.);
                    for (int op_id : pnd[pr_id]){
                        constraint.insert((*index_opt)[op_id][t], 1.);
                    }
                    constraints.push_back(constraint);
                }
                // a
                {
                    lp::LPConstraint constraint(0., infinity);
                    constraint.insert(index_a[pr_id][t+1], -1.);
                    for (int op_id : anp[pr_id]){
                        constraint.insert((*index_opt)[op_id][t], 1.);
                    }
                    constraints.push_back(constraint);
                }
                // pd
                {
                    lp::LPConstraint constraint(0., 0.);
                    constraint.insert(index_pd[pr_id][t+1], -1.);
                    for (int op_id : pd[pr_id]){
                        constraint.insert((*index_opt)[op_id][t], 1.);
                    }
                    constraints.push_back(constraint);
                }
            }
        }
    }
}

void StateChangeModel::precondition_effect_constraint(const std::shared_ptr<AbstractTask> task,
                                               std::vector<lp::LPConstraint> &constraints,
                                               double infinity, int t_min, int t_max){
    
    TaskProxy task_proxy(*task);
    State initial_state =  task_proxy.get_initial_state();
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        int num_values = numeric_task.get_n_proposition_value(var);
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            for (int t = t_min; t < t_max-1; ++t){
                // pa
                {
                    for (int op_id : pnd[pr_id]){
                        lp::LPConstraint constraint(0., infinity);
                        constraint.insert(index_pa[pr_id][t+1], 1.);
                        constraint.insert((*index_opt)[op_id][t], -1.);
                        constraints.push_back(constraint);
                   }
                }
                // a
                {
                    for (int op_id : anp[pr_id]){
                        lp::LPConstraint constraint(0., infinity);
                        constraint.insert(index_a[pr_id][t+1], 1.);
                        constraint.insert((*index_opt)[op_id][t], -1.);
                        constraints.push_back(constraint);
                   }
                }
            }
        }
    }
}

void StateChangeModel::mutex_proposition_constraint(const std::shared_ptr<AbstractTask> task,
                                               std::vector<lp::LPConstraint> &constraints,
                                               double infinity, int t_min, int t_max){
    
    TaskProxy task_proxy(*task);
    State initial_state =  task_proxy.get_initial_state();
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        int num_values = numeric_task.get_n_proposition_value(var);
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            for (int t = t_min; t < t_max; ++t){
                // 1
                {
                    lp::LPConstraint constraint(-infinity, 1);
                    constraint.insert(index_a[pr_id][t], 1.);
                    constraint.insert(index_mant[pr_id][t], 1.);
                    constraint.insert(index_pd[pr_id][t], 1.);
                    constraints.push_back(constraint);
                }
                // 2
                {
                    lp::LPConstraint constraint(-infinity, 1);
                    constraint.insert(index_pa[pr_id][t], 1.);
                    constraint.insert(index_mant[pr_id][t], 1.);
                    constraint.insert(index_pd[pr_id][t], 1.);
                    constraints.push_back(constraint);
                }
            }
        }
    }
}

void StateChangeModel::flow_constraint(const std::shared_ptr<AbstractTask> task,
                                                    std::vector<lp::LPConstraint> &constraints,
                                                    double infinity, int t_min, int t_max){
    
    TaskProxy task_proxy(*task);
    State initial_state =  task_proxy.get_initial_state();
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        int num_values = numeric_task.get_n_proposition_value(var);
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            for (int t = t_min; t < t_max-1; ++t){
                lp::LPConstraint constraint(0, infinity);
                constraint.insert(index_a[pr_id][t], 1.);
                constraint.insert(index_pa[pr_id][t], 1.);
                constraint.insert(index_mant[pr_id][t], 1.);
                constraint.insert(index_pa[pr_id][t+1], -1.);
                constraint.insert(index_mant[pr_id][t+1], -1.);
                constraint.insert(index_pd[pr_id][t+1], -1.);
                constraints.push_back(constraint);
            }
        }
    }
}

void StateChangeModel::mutex_relaxtion_constraint(const std::shared_ptr<AbstractTask> task,
                                                 std::vector<lp::LPConstraint> &constraints,
                                                 double infinity, int t_min, int t_max){
    for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
        for (int op_mutex_id : numeric_task.get_mutex_actions(op_id)){
            for (int t = t_min; t < t_max; ++t){
                lp::LPConstraint constraint(-infinity, 1);
                constraint.insert((*index_opt)[op_id][t],1);
                constraint.insert((*index_opt)[op_mutex_id][t],1);
                constraints.push_back(constraint);
            }
        }
    }
    
}

//void StateChangeModel::print_solution(vector<double> &solution,const std::shared_ptr<AbstractTask> task){
//    TaskProxy task_proxy(*task);
//    cout << "solution " << endl;
//    OperatorsProxy ops = task_proxy.get_operators();
//    int n_prop = numeric_task.get_n_propositions();
////    for (int t = 0; t < t_max; ++t){
////        for (int i_prop = 0; i_prop < n_prop; ++i_prop){
////            cout << t << " " << i_prop << " " << numeric_task.get_proposition_name(i_prop) << " : " << solution[index_a[i_prop][t]] << " " << solution[index_pa[i_prop][t]] << " " <<solution[index_mant[i_prop][t]]  << " neg " << solution[index_pd[i_prop][t]] << endl;
////        }
////    }
////    for (int t = 0; t < t_max; ++t){
////        for (int op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
////            if (solution[(*index_opt)[op_id][t]] > 0){
////                //cout << t <<"(" << solution[(*index_opt)[op_id][t]] << " " << ops[op_id].get_name() << endl;
////                cout << "(" << ops[op_id].get_name() << ")" << endl;
////            }
////        }
////    }
//    int steps = 0;
//    cout << "Solution found!" << endl;
//    cout << "Actual search time: 0.168001s [t=0.171312s]" << endl;
//    for (int t = 0; t < t_max; ++t){
//        for (int op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
//            if (solution[(*index_opt)[op_id][t]] > 0){
//                if (ops[op_id].get_name().find("forget") == 0) continue;
//                //cout << t <<" " << solution[(*index_opt)[op_id][t]] << " " << ops[op_id].get_name() << endl;
//                cout << ops[op_id].get_name() << " (1)" << endl;
//                steps++;
//            }
//        }
//    }
//    cout << "Plan length: " << steps  << " step(s)." << endl;
//}


static shared_ptr<IPConstraintGenerator> _parse(OptionParser &parser) {
    parser.document_synopsis(
                             "State based model",
                             "For each fact, a permanent constraint is added that considers the net "
                             "change of the fact, i.e., the total number of times the fact is added "
                             "minus the total number of times is removed. The bounds of each "
                             "constraint depend on the current state and the goal state and are "
                             "updated in each state. For details, see" + utils::format_paper_reference(
                                                                                                       {"Menkes van den Briel", "J. Benton", "Subbarao Kambhampati",
                                                                                                           "Thomas Vossen"},
                                                                                                       "An LP-based heuristic for optimal planning",
                                                                                                       "http://link.springer.com/chapter/10.1007/978-3-540-74970-7_46",
                                                                                                       "Proceedings of the Thirteenth International Conference on"
                                                                                                       " Principles and Practice of Constraint Programming (CP 2007)",
                                                                                                       "651-665",
                                                                                                       "2007") + utils::format_paper_reference(
                                                                                                                                               {"Blai Bonet"},
                                                                                                                                               "An admissible heuristic for SAS+ planning obtained from the"
                                                                                                                                               " state equation",
                                                                                                                                               "http://ijcai.org/papers13/Papers/IJCAI13-335.pdf",
                                                                                                                                               "Proceedings of the Twenty-Third International Joint"
                                                                                                                                               " Conference on Artificial Intelligence (IJCAI 2013)",
                                                                                                                                               "2268-2274",
                                                                                                                                               "2013") + utils::format_paper_reference(
                                                                                                                                                                                       {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert",
                                                                                                                                                                                           "Blai Bonet"},
                                                                                                                                                                                       "LP-based Heuristics for Cost-optimal Planning",
                                                                                                                                                                                       "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/7892/8031",
                                                                                                                                                                                       "Proceedings of the Twenty-Fourth International Conference"
                                                                                                                                                                                       " on Automated Planning and Scheduling (ICAPS 2014)",
                                                                                                                                                                                       "226-234",
                                                                                                                                                                                       "AAAI Press 2014"));
    
    if (parser.dry_run())
        return nullptr;
    return make_shared<StateChangeModel>();
}

static PluginShared<IPConstraintGenerator> _plugin("sc", _parse);
