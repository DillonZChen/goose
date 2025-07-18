#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"
#include "sb_compilation.h"
#include "../numeric_operator_counting/numeric_helper.h"

using namespace std;
using namespace operator_counting;
using namespace numeric_helper;

void StateBasedModel::initialize_variables(const std::shared_ptr<AbstractTask> task,
                                           std::vector<lp::LPVariable> &variables,
                                           double infinity){
    cout << "initializing variables in SB" << endl;
    TaskProxy task_proxy(*task);
    numeric_task = NumericTaskProxy(task_proxy);
    OperatorsProxy ops = task_proxy.get_operators();
    int n_ops = ops.size();
    int n_prop = numeric_task.get_n_propositions();
    
    index_opt->assign(n_ops,vector<int>(t_max,-1));
    index_fact.assign(n_prop, vector<int> (t_max,-1));
    index_noop_fact.assign(n_prop, vector<int> (t_max,-1));
    
    add_variables(task,variables,infinity,t_min,t_max);
}

void StateBasedModel::add_variables(
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
        //if (numeric_task.is_numeric_axiom(var)) continue;
        int num_values = numeric_task.get_n_proposition_value(var);
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            index_fact[pr_id].resize(t_max,-1);
            index_noop_fact[pr_id].resize(t_max,-1);
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
            //if (numeric_task.is_numeric_axiom(var)) continue;
            int num_values = numeric_task.get_n_proposition_value(var);
            for (int value = 0; value < num_values; ++value) {
                int pr_id = numeric_task.get_proposition(var,value);
                {
                    index_fact[pr_id][t] = variables.size();
                    stringstream name;
                    name << "up_" << pr_id << "_" << t;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str(), lp::LPVariableType::continous));
                }
                {
                    index_noop_fact[pr_id][t] = variables.size();
                    stringstream name;
                    name << "up_nooop_" << pr_id << "_" << t;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str(), lp::LPVariableType::continous));
                }
            }
        }
    }
    
}

void StateBasedModel::initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                             std::vector<lp::LPConstraint> &constraints,
                                             double infinity){
    
    cout << "initializing constraints for SB" << endl;
    initial_state_constraint(task, constraints);
    goal_state_constraint(task, constraints, t_max);
    action_precondition_constraint(task, constraints, infinity,t_min,t_max);
    action_effect_constraint(task, constraints, infinity,t_min,t_max);
    mutex_relaxtion_constraint(task, constraints, infinity,t_min,t_max);
    noop_preconditions_constraint(task, constraints, infinity,t_min,t_max);
    noopt_mutex_constraint(task, constraints, infinity,t_min,t_max);
}

bool StateBasedModel::update_constraints(const State &state, lp::LPSolver &lp_solver){
    return false;
}

bool StateBasedModel::update_constraints(const int horizon,
                                         lp::LPSolver &lp_solver,const std::shared_ptr<AbstractTask> task,
                                         std::vector<lp::LPVariable> &variables,
                                         double infinity, std::vector<lp::LPConstraint> & constraints){
    cout << "adding constraint from SB" << endl;
    t_min = t_max;
    t_max = horizon;
    add_variables(task,variables,infinity,t_min,t_max);
    action_precondition_constraint(task, constraints, infinity,t_min,t_max);
    action_effect_constraint(task, constraints, infinity,t_min-1,t_max);
    mutex_relaxtion_constraint(task, constraints, infinity,t_min,t_max);
    noop_preconditions_constraint(task, constraints, infinity,t_min,t_max);
    noopt_mutex_constraint(task, constraints, infinity,t_min,t_max);
    goal_state_constraint(task, constraints, t_max);
    return false;
}

void StateBasedModel::initial_state_constraint(const std::shared_ptr<AbstractTask> task,
                                               std::vector<lp::LPConstraint> &constraints){
    
    TaskProxy task_proxy(*task);
    State initial_state =  task_proxy.get_initial_state();
    for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        //if (numeric_task.is_numeric_axiom(var)) continue;
        int num_values = numeric_task.get_n_proposition_value(var);
        int initial_value = initial_state[var].get_value();
        for (int value = 0; value < num_values; ++value) {
            int pr_id = numeric_task.get_proposition(var,value);
            double lb = (initial_value == value);
            lp::LPConstraint constraint(lb, lb);
            constraint.insert(index_fact[pr_id][0], 1.);
            constraints.push_back(constraint);
        }
    }
}

void StateBasedModel::action_precondition_constraint(const std::shared_ptr<AbstractTask> task,
                                                     std::vector<lp::LPConstraint> &constraints,
                                                     double infinity, int t_min, int t_max){
    
    for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
        for (int i : numeric_task.get_action_pre_list(op_id)){
            for (int t = t_min; t < t_max; ++t){
                lp::LPConstraint constraint(0., infinity);
                constraint.insert((*index_opt)[op_id][t], -1.);
                constraint.insert(index_fact[i][t], 1.);
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
}

void StateBasedModel::action_effect_constraint(const std::shared_ptr<AbstractTask> task,
                                               std::vector<lp::LPConstraint> &constraints,
                                               double infinity, int t_min, int t_max){
    
    for (size_t fact_id = 0; fact_id < numeric_task.get_n_propositions(); ++fact_id){
        set<int> achievers = numeric_task.get_achievers(fact_id);
        for(int t = t_min; t < t_max-1; ++t){
            lp::LPConstraint constraint(0., infinity);
            constraint.insert(index_fact[fact_id][t+1], -1.);
            for (int a : achievers) {
                constraint.insert((*index_opt)[a][t], 1.);
            }
            constraint.insert(index_noop_fact[fact_id][t],1.);
            constraints.push_back(constraint);
        }
    }
}

void StateBasedModel::mutex_relaxtion_constraint(const std::shared_ptr<AbstractTask> task,
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

void StateBasedModel::noop_preconditions_constraint(const std::shared_ptr<AbstractTask> task,
                                                    std::vector<lp::LPConstraint> &constraints,
                                                    double infinity, int t_min, int t_max){
    for (size_t fact_id = 0; fact_id < numeric_task.get_n_propositions(); ++fact_id){
        for (int t = t_min; t < t_max; ++t){
            lp::LPConstraint constraint(-infinity, 0);
            constraint.insert(index_fact[fact_id][t],-1);
            constraint.insert(index_noop_fact[fact_id][t],1);
            constraints.push_back(constraint);
        }
    }
}

void StateBasedModel::noopt_mutex_constraint(const std::shared_ptr<AbstractTask> task,
                                             std::vector<lp::LPConstraint> &constraints,
                                             double infinity, int t_min, int t_max){
    TaskProxy task_proxy(*task);
    VariablesProxy vars = task_proxy.get_variables();
    OperatorsProxy ops = task_proxy.get_operators();
    vector<set<int>> mutex_lit(numeric_task.get_n_propositions());
    for (int var = 0; var < vars.size(); ++var){
        //if (numeric_task.is_numeric_axiom(var)) continue;
        VariableProxy variable = vars[var];
        for (int val = 0; val < variable.get_domain_size(); ++val){
            int p = numeric_task.get_proposition(var,val);
            for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
                const OperatorProxy &op = ops[op_id];
                bool mutex = false;
                for (EffectProxy effect_proxy : op.get_effects()) {
                    FactProxy effect = effect_proxy.get_fact();
                    int v = effect.get_variable().get_id();
                    int post = effect.get_value();
                    if (var == v && val != post) mutex = true;
                }
                if (mutex) mutex_lit[p].insert(op_id);
            }
            for (int t = t_min; t < t_max; ++t){
                lp::LPConstraint constraint(-infinity, 1);
                constraint.insert(index_noop_fact[p][t],1);
                for (int op_id : mutex_lit[p]) {
                    constraint.insert((*index_opt)[op_id][t],1);
                }
                constraints.push_back(constraint);
            }
        }
    }
}
// TODO check if goal is at the last timestep

void StateBasedModel::goal_state_constraint(const std::shared_ptr<AbstractTask> task,
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
        constraint.insert(index_fact[numeric_task.get_proposition(goal.get_variable().get_id(),goal.get_value())][t_max-1], 1.);
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
//void StateBasedModel::print_solution(vector<double> &solution,const std::shared_ptr<AbstractTask> task){
//    TaskProxy task_proxy(*task);
//    OperatorsProxy ops = task_proxy.get_operators();
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
    return make_shared<StateBasedModel>();
}

static PluginShared<IPConstraintGenerator> _plugin("sb", _parse);

