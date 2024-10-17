#include "resource_detection.h"
#include "../plugin.h"
#include "../option_parser.h"
#include "../global_state.h"
#include "../globals.h"
#include "../task_proxy.h"
#include <algorithm>
#include <fstream>

using namespace std;
using namespace rd;

bool rddebug = false;
ResourceDetection::ResourceDetection(TaskProxy & task_proxy, bool single)  {
    // measure time
    utils::Timer detection_timer;
    consumer = 0;
    producer = 0;
    single_action = single;
    get_resource_actions(task_proxy);
    create_sets_actions(task_proxy);
    detection_timer.stop();
    cout << "Detection time: " << detection_timer << endl;
    cout << "Found " << variable_r.size() << "/" << task_proxy.get_variables().size() <<" resource variables" << endl;
    cout << "Found " << consumer << "/" << task_proxy.get_operators().size() <<" consumer actions" << endl;
    cout << "Found " << producer << "/" << task_proxy.get_operators().size() <<" producer actions" << endl;

}

ResourceDetection::~ResourceDetection(){
    
}

void ResourceDetection::populate_v_equivalente_actions(TaskProxy & task_proxy){
    VariablesProxy vars = task_proxy.get_variables();
    OperatorsProxy ops = task_proxy.get_operators();
    v_equivalent_actions.assign(vars.size(),vector<vector<bool>>(ops.size(),vector<bool>(ops.size(),false)));

    for(size_t id_op_a = 0; id_op_a < ops.size(); ++id_op_a){
        const OperatorProxy &op_a = task_proxy.get_operators()[id_op_a];
        for(size_t id_op_b = id_op_a + 1; id_op_b < ops.size(); ++id_op_b){
            const OperatorProxy &op_b = task_proxy.get_operators()[id_op_b];
            if (op_a.get_cost() != op_a.get_cost()) continue;
            if (op_a.get_preconditions().size() != op_b.get_preconditions().size()) continue;
            if (op_a.get_effects().size() != op_b.get_effects().size()) continue;
            
            int id_var = -1;
            int different_var = 0;
            for (size_t var = 0; var < task_proxy.get_variables().size(); var++){
                
                {
                    int val_a = action_preconditions[id_op_a][var];
                    int val_b = action_preconditions[id_op_b][var];
                    if (val_a!=val_b) {
                        id_var = var;
                        different_var++;
                    }
                }
                
                {
                    int val_a = action_effects[id_op_a][var];
                    int val_b = action_effects[id_op_b][var];
                    if (val_a!=val_b && id_var != var){
                        different_var++;
                        id_var = var;
                    }
                }
                
            }
            if (different_var == 1){
    
                //cout << id_var << " " << op_a.get_name() << " equivalent " << op_b.get_name() << endl;
                v_equivalent_actions[id_var][id_op_a][id_op_b] = true;
                v_equivalent_actions[id_var][id_op_b][id_op_a] = true;
            }
        }
        for (size_t var = 0; var < task_proxy.get_variables().size(); var++){
            v_equivalent_actions[var][id_op_a][id_op_a] = true;
        }
    }

}
void ResourceDetection::get_resource_actions(TaskProxy & task_proxy){
    VariablesProxy vars = task_proxy.get_variables();
    OperatorsProxy ops = task_proxy.get_operators();
    fill_actions_preconditions_and_effects(task_proxy);
    populate_v_equivalente_actions(task_proxy);
    resource_actions.assign(vars.size(),vector<ActionTypeRD>(ops.size(),ActionTypeRD::NONE));
    resource_max.assign(vars.size(),0);
    resource_mu.resize(vars.size());
    resource_delta.assign(vars.size(),vector<double>(ops.size(),0));
    resource_variables.assign(vars.size(),false);
    for (size_t id_var = 0; id_var < vars.size(); ++id_var){
        resource_mu[id_var].assign(vars[id_var].get_domain_size(),0);
    }
    
    for (size_t id_var = 0; id_var < vars.size(); ++id_var){
        // check if it's in goal state (condition (i))
        if (!condition_first(id_var)) {
            if(rddebug) cout << "id var " << id_var << " " << vars[id_var].get_name() << " does not satisfy goal condition"<< endl;
            continue;
        }
        // check condition (ii)
        if (!condition_second(task_proxy,id_var)) {
             if(rddebug) cout << "id var " << id_var << " " << vars[id_var].get_name() << " does not satisfy second condition"<< endl;
            continue;
        }
        
        // check condition (iii)
        bool iii = condition_third(task_proxy,id_var);
        if (!iii) if(rddebug) cout << "id var " << id_var << " " << vars[id_var].get_name() << " does not satisfy third condition" << endl;
        if (iii) resource_variables[id_var] = true;
        if (iii){
             if(rddebug) cout << "id var " << id_var << " " << vars[id_var].get_name() << " (" << task_proxy.get_variables()[id_var].get_domain_size() << ")" << endl;
            for (int d = 0; d < task_proxy.get_variables()[id_var].get_domain_size(); ++d){
                FactProxy fact = task_proxy.get_variables()[id_var].get_fact(d);
                 if(rddebug) cout << "\tfacts " << fact.get_name() << endl;
            }
        }
    }
}

bool ResourceDetection::condition_first(int id_var){
    return goal_state[id_var]==-1;
}

bool ResourceDetection::condition_second(TaskProxy & task_proxy, int id_var){
    for(size_t i = 0; i < action_preconditions.size(); ++i){

        if (action_effects[i][id_var] != -1){
            if (action_preconditions[i][id_var] == -1) {
                //if (rddebug) cout << "action " << task_proxy.get_operators()[i].get_name() << " " << action_preconditions[i][id_var] << " " << action_effects[i][id_var] << endl;

             return false;
            }
        }
        
        // TODO check this part
        if (action_preconditions[i][id_var] != -1){
            if (action_effects[i][id_var] == -1) {
                //if (rddebug) cout << "\taction " << task_proxy.get_operators()[i].get_name() << " " << action_preconditions[i][id_var] << " " << action_effects[i][id_var] << endl;
                return false;
            }
        }
    }
    return true;
}

bool ResourceDetection::condition_third(TaskProxy & task_proxy, int id_var){
    // get equivalent operator
    list<set<int>> ea = get_equivalent_actions(task_proxy,id_var);
        // debug:
    if (rddebug){
    cout << "\tvariable " << task_proxy.get_variables()[id_var].get_name() << " (" << task_proxy.get_variables()[id_var].get_domain_size() << ")" << endl;
    for (int d = 0; d < task_proxy.get_variables()[id_var].get_domain_size(); ++d){
        FactProxy fact = task_proxy.get_variables()[id_var].get_fact(d);
        cout << "\tfacts " << fact.get_name() << endl;

        }
//        for(set<int> e : ea){
//            if (e.empty()) continue;
//            cout << "\t";
//            for (int a : e){
//                cout << task_proxy.get_operators()[a].get_name() << "; ";
//            }
//            cout << endl;
//        }
    }
    // mark consumers, producers
    // get ordering
    int id_order = 0;
    list<int> reference_ordering;
    pair<set<int>,set<int>> head_tail_reference;
    int domain_size = task_proxy.get_variables()[id_var].get_domain_size();
    //vector<vector<bool>> before(domain_size,vector<bool>(domain_size,false));
    if (ea.empty() && !single_action) return false;
    ea.sort(CompareSet());
    for(set<int> & actions : ea){
        if (rddebug){
            cout << "\taction size " <<  actions.size() << " " << task_proxy.get_variables()[id_var].get_domain_size() << endl;
            //if (actions.size() < task_proxy.get_variables()[id_var].get_domain_size() - 1) return false;
            for (int a : actions){
                cout << task_proxy.get_operators()[a].get_name() << "; ";
            }
            cout << endl;
        }
        
        if (id_order == 0){
            list<int> ordering;
            mark_type(id_var, actions, ActionTypeRD::CONSUMER);
            if (rddebug) cout << "CONSUMER" << endl;
            if (ea.size()==1) continue;
            head_tail_reference = find_head_tails(task_proxy, id_var, actions);
            //reference_ordering = find_ordering_value(task_proxy, id_var, actions);
            //fill_ordering_value(task_proxy, id_var, actions,before);
                
        }else{
            pair<set<int>,set<int>> head_tail = find_head_tails(task_proxy, id_var, actions);

            //mark_type(id_var, actions, ActionTypeRD::CONSUMER);
            //list<int> values_ordering = find_ordering_value(task_proxy, id_var, actions);
            //if( same_order(values_ordering,reference_ordering)) {
            //if( same_order(actions,id_var,before)) {
            if( same_order(head_tail_reference,head_tail)) {
                mark_type(id_var, actions, ActionTypeRD::CONSUMER);
                if (rddebug) cout << "CONSUMER" << endl;
            }else{
              mark_type(id_var, actions, ActionTypeRD::PRODUCER);
                if (rddebug) cout << "PRODUCER" << endl;
            }

        }
        id_order++;

        
    }
    // create lp if solvable, than it's a resource, otherwise no
    vector<double> solution;
    bool solved = solve_lp(task_proxy,id_var,solution);
    //cout << "third condition is " << solved << endl;
    if (!solved){
         for(set<int> & actions : ea){
             mark_type(id_var, actions, ActionTypeRD::NONE);
         }
    }
    return solved;
}

bool ResourceDetection::same_order(pair<set<int>,set<int>> &reference, pair<set<int>,set<int>> &compare){
    set<int> &head_reference = reference.first;
    set<int> &head = compare.first;
    set<int> &tail_reference = reference.second;
    set<int> &tail = compare.second;
    set<int> head_intersection;
    set<int> tail_intersection;
    //head_intersection.begin() =
    set_intersection(head_reference.begin(),head_reference.end(),head.begin(),head.end(),inserter(head_intersection,head_intersection.begin()));
        //tail_intersection.begin() =
    set_intersection(tail_reference.begin(),tail_reference.end(),tail.begin(),tail.end(),inserter(tail_intersection,tail_intersection.begin()));
    if (!head_intersection.empty() && !tail_intersection.empty()) return true;
    if (head_intersection.empty()  && tail_intersection.empty()) return false;
    assert(false);
    return false;
}
bool ResourceDetection::solve_lp(TaskProxy & task_proxy, int id_var, vector<double> &solution){
    
    lp::LPSolver lp_solver(lp::LPSolverType::CPLEX,lp::LPConstraintType::LP);
    vector<lp::LPVariable> variables;
    vector<lp::LPConstraint> constraints;
    double infinity = lp_solver.get_infinity();
    map<int,int> map_mu;
    map<int,int> map_delta;
    //map<int,int> map_xc;
    //map<int,int> map_xp;
    int map_maxmu;
    VariableProxy var = task_proxy.get_variables()[id_var];
    
    // variables
    for (int id_domain = 0; id_domain < var.get_domain_size(); ++id_domain){
        map_mu[id_domain] = variables.size();
        stringstream name;
        name << "mu_" << id_domain;
        variables.push_back(lp::LPVariable(0, infinity, 0, name.str(),lp::LPVariableType::continous));
    }
    
    for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
        ActionTypeRD type = resource_actions[id_var][id_op];
        if (type == ActionTypeRD::NONE) continue;
        map_delta[id_op] = variables.size();
        //map_xc[id_op] = variables.size()+1;
        //map_xp[id_op] = variables.size()+2;
        stringstream name;
        //stringstream namexp;
        //stringstream namexc;
        name << "delta_" << task_proxy.get_operators()[id_op].get_name();
        //namexp << "namexp" << task_proxy.get_operators()[id_op].get_name();
        //namexc << "namexc" << task_proxy.get_operators()[id_op].get_name();
        if (type == ActionTypeRD::CONSUMER){
            variables.push_back(lp::LPVariable(-infinity,0,0,name.str(),lp::LPVariableType::continous));
            //variables.push_back(lp::LPVariable(0,1,0,namexp.str(),lp::LPVariableType::integer));
            //variables.push_back(lp::LPVariable(0,1,0,namexc.str(),lp::LPVariableType::integer));
        }else if (type == ActionTypeRD::PRODUCER) {
            variables.push_back(lp::LPVariable(0,infinity,0,name.str(),lp::LPVariableType::continous));
            //variables.push_back(lp::LPVariable(0,1,0,namexp.str(),lp::LPVariableType::integer));
            //variables.push_back(lp::LPVariable(0,1,0,namexc.str(),lp::LPVariableType::integer));
        }else{
            assert(false);
        }
    }
    map_maxmu = variables.size();
    variables.push_back(lp::LPVariable(0,infinity,1,"max_mu",lp::LPVariableType::continous));

    // constraint 1.
    for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
        ActionTypeRD type = resource_actions[id_var][id_op];
        if (type == ActionTypeRD::NONE) continue;
        int index_pre = map_mu[action_preconditions[id_op][id_var]];
        int index_eff = map_mu[action_effects[id_op][id_var]];
        int delta_a = map_delta[id_op];
        lp::LPConstraint constraint(0,0);
        constraint.insert(index_pre,1);
        constraint.insert(index_eff,-1);
        constraint.insert(delta_a,1);
        constraints.push_back(constraint);
    }
    
    // constraint 2.
    for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
        ActionTypeRD type = resource_actions[id_var][id_op];
        if (type == ActionTypeRD::NONE) continue;
        for (size_t id_op_b = 0; id_op_b < task_proxy.get_operators().size(); ++id_op_b){
            ActionTypeRD type_b = resource_actions[id_var][id_op_b];
            if (type_b == ActionTypeRD::NONE) continue;
            if (id_op_b == id_op) continue;
            if(are_v_equivalent(task_proxy,id_var,id_op,id_op_b)){
                int delta_a = map_delta[id_op];
                int delta_b = map_delta[id_op_b];
                lp::LPConstraint constraint(0,0);
                constraint.insert(delta_a,1);
                constraint.insert(delta_b,-1);
                constraints.push_back(constraint);
            }
        }
    }
    
    // constraint 3.
    for (int id_domain = 0; id_domain < var.get_domain_size(); ++id_domain){
        lp::LPConstraint constraint(0,infinity);
        constraint.insert(map_maxmu,1);
        constraint.insert(map_mu[id_domain],-1);
        constraints.push_back(constraint);
    }
    
    // constraint 4.
    for (int id_domain = 0; id_domain < var.get_domain_size(); ++id_domain){
        for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
            ActionTypeRD type = resource_actions[id_var][id_op];
            if (type == ActionTypeRD::CONSUMER){
                bool exists = false;
                for (size_t id_op_b = 0; id_op_b < task_proxy.get_operators().size(); ++id_op_b){
                    ActionTypeRD type_b = resource_actions[id_var][id_op_b];
                    if (type_b == ActionTypeRD::NONE || type_b == ActionTypeRD::PRODUCER) continue;
                    //if (id_op_b == id_op) continue;
                    if (are_v_equivalent(task_proxy,id_var,id_op,id_op_b)){
                        if (action_preconditions[id_op_b][id_var] == id_domain) exists = true;
                    }
                }
                if (!exists){
                    lp::LPConstraint constraint(-infinity,-1);
                    constraint.insert(map_mu[id_domain],1);
                    constraint.insert(map_delta[id_op],1);
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    // constraint 5.
    for (int id_domain = 0; id_domain < var.get_domain_size(); ++id_domain){
        for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
            ActionTypeRD type = resource_actions[id_var][id_op];
            if (type == ActionTypeRD::PRODUCER){
                bool exists = false;
                for (size_t id_op_b = 0; id_op_b < task_proxy.get_operators().size(); ++id_op_b){
                    ActionTypeRD type_b = resource_actions[id_var][id_op_b];
                    if (type_b == ActionTypeRD::NONE || type_b == ActionTypeRD::CONSUMER) continue;
                    //if (id_op_b == id_op) continue;
                    if (are_v_equivalent(task_proxy,id_var,id_op,id_op_b)){
                        if (action_preconditions[id_op_b][id_var] == id_domain) exists = true;
                    }
                }
                if (!exists){
                    lp::LPConstraint constraint(1,infinity);
                    constraint.insert(map_mu[id_domain],1);
                    constraint.insert(map_delta[id_op],1);
                    constraint.insert(map_maxmu,-1);
                    constraints.push_back(constraint);
                }
            }
        }
    }
//    // constraint 4-5.
//    for (int id_domain = 0; id_domain < var.get_domain_size(); ++id_domain){
//        double M = var.get_domain_size()*2;
//        double epsilon = 1;
//        for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
//            bool exists = false;
//            ActionTypeRD type = resource_actions[id_var][id_op];
//            if (type == ActionTypeRD::NONE) continue;
//            for (size_t id_op_b = 0; id_op_b < task_proxy.get_operators().size(); ++id_op_b){
//                //if (id_op_b == id_op) continue;
//                ActionTypeRD type_b = resource_actions[id_var][id_op_b];
//                if (type_b == ActionTypeRD::NONE) continue;
//                if (are_v_equivalent(task_proxy,id_var,id_op,id_op_b)){
//                    if (action_preconditions[id_op_b][id_var] == id_domain) exists = true;
//                }
//            }
//            if (!exists){
//                lp::LPConstraint constraint(-infinity,M-epsilon);
//                constraint.insert(map_mu[id_domain],1);
//                constraint.insert(map_delta[id_op],1);
//                constraint.insert(map_xc[id_op],M);
//                constraints.push_back(constraint);
//            }
//
//            if (!exists){
//                lp::LPConstraint constraint(-M+epsilon,infinity);
//                constraint.insert(map_mu[id_domain],1);
//                constraint.insert(map_delta[id_op],1);
//                constraint.insert(map_maxmu,-1);
//                constraint.insert(map_xp[id_op],-M);
//                constraints.push_back(constraint);
//            }
//
//            if (!exists){
//                lp::LPConstraint constraint(1,1);
//                constraint.insert(map_xp[id_op],1);
//                constraint.insert(map_xc[id_op],1);
//                constraints.push_back(constraint);
//            }
//        }
//    }

    lp_solver.load_problem(lp::LPObjectiveSense::MINIMIZE, variables, constraints);
    lp_solver.solve();
    if (lp_solver.has_optimal_solution()){
        solution = lp_solver.extract_solution();
        for (int id_domain = 0; id_domain < var.get_domain_size(); ++id_domain){
            FactProxy fact = task_proxy.get_variables()[id_var].get_fact(id_domain);
            if (rddebug){
              cout << id_domain << " "<< fact.get_name() << " " << solution[map_mu[id_domain]] << " " <<  solution[map_maxmu] << endl;
            }
            resource_mu[id_var][id_domain] = solution[map_mu[id_domain]];
        }
        for (size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
            ActionTypeRD type = resource_actions[id_var][id_op];
            if (type == ActionTypeRD::NONE) continue;
            resource_delta[id_var][id_op] = solution[map_delta[id_op]];
            //if (resource_delta[id_var][id_op] < 0) resource_actions[id_var][id_op] = ActionTypeRD::CONSUMER;
            //else resource_actions[id_var][id_op] = ActionTypeRD::PRODUCER;
            if (rddebug) cout << id_op << " " << task_proxy.get_operators()[id_op].get_name() << " " << solution[map_delta[id_op]] << endl;
        }

        resource_max[id_var] = solution[map_maxmu];
        //cout << "max mu " << solution[map_maxmu] << endl;
    }
    return lp_solver.has_optimal_solution();
}
bool ResourceDetection::same_order(list<int>&list_a, list<int> &list_b){
    //TODO: change this in case of subset
    return list_a==list_b;
    for (list<int>::iterator a = list_a.begin(); a != list_a.end(); ++a){
        list<int>::iterator p = find(list_b.begin(),list_b.end(),*a);
        if (p == list_b.end()) continue;
        else if (++p == list_b.end()) return false;
        else if ( *p == *(a++)) return true;
    }
    return false;
}

bool ResourceDetection::same_order(set<int>&list_a, int id_var, std::vector<std::vector<bool>> &before){
    //TODO: change this in case of subset
    for (int op : list_a){
        int pre = action_preconditions[op][id_var];
        int eff = action_effects[op][id_var] ;
        //if (pre == -1 || eff == -1 ) continue;
        if (before[pre][eff]) {
            return true;
        }else{
            for (int i = 0; i < before.size(); ++i){
                if (before[eff][i] && before[pre][i]) return true;
            }
        }
    }
    return false;
}

bool ResourceDetection::are_v_equivalent(TaskProxy & task_proxy, int id_var, int id_op_a, int id_op_b){
    return v_equivalent_actions[id_var][id_op_a][id_op_b];
    //return are_v_equivalent_test(task_proxy,id_var,id_op_a,id_op_b);
    if(v_equivalent_actions[id_var][id_op_a][id_op_b] != are_v_equivalent_test(task_proxy,id_var,id_op_a,id_op_b)){
        cout << "check here " << id_var << " " << task_proxy.get_operators()[id_op_a].get_name() << " " << task_proxy.get_operators()[id_op_b].get_name() << " " << v_equivalent_actions[id_var][id_op_a][id_op_b] << " " << are_v_equivalent_test(task_proxy,id_var,id_op_a,id_op_b) << endl;
    }
    return are_v_equivalent_test(task_proxy,id_var,id_op_a,id_op_b);
}

bool ResourceDetection::are_v_equivalent_test(TaskProxy & task_proxy, int id_var, int id_op_a, int id_op_b){
    //return v_equivalent_actions[id_var][id_op_a][id_op_b];
    const OperatorProxy &op_a = task_proxy.get_operators()[id_op_a];
    const OperatorProxy &op_b = task_proxy.get_operators()[id_op_b];
    if (op_a.get_preconditions().size() != op_b.get_preconditions().size()) return false;
    if (op_a.get_effects().size() != op_b.get_effects().size()) return false;
    bool found = false;
    for (int var = 0; var < task_proxy.get_variables().size(); var++){
        if (var == id_var) {
            found = true;
            continue;
        }
        
        {
            int val_a = action_preconditions[id_op_a][var];
            int val_b = action_preconditions[id_op_b][var];
            if (val_a!=val_b) return false;
        }
        
        {
            int val_a = action_effects[id_op_a][var];
            int val_b = action_effects[id_op_b][var];
            if (val_a!=val_b) return false;
        }
        
    }
//    for (FactProxy condition_a : op_a.get_preconditions()) {
//        int var_a = condition_a.get_variable().get_id();
//        int val_a = action_preconditions[id_op_a][var_a];
//        int val_b = action_preconditions[id_op_b][var_a];
//        if (val_a!=val_b && var_a!=id_var) return false;
//        if (var_a==id_var) {
//            found = true;
//        }
//    }
//
//    for (FactProxy condition_b : op_b.get_preconditions()) {
//        int var_b = condition_b.get_variable().get_id();
//        int val_b = action_preconditions[id_op_b][var_b];
//        int val_a = action_preconditions[id_op_a][var_b];
//        if (val_a!=val_b && var_b!=id_var) return false;
//        if (var_b==id_var) {
//            found = true;
//        }
//    }
//
//
//    for (EffectProxy eff_a : op_a.get_effects()) {
//        int var_a = eff_a.get_fact().get_variable().get_id();
//        int val_a = action_effects[id_op_a][var_a];
//        int val_b = action_effects[id_op_b][var_a];
//        if (val_a!=val_b && var_a!=id_var) return false;
//        if (var_a==id_var) {
//            found = true;
//        }
//    }
//
//    for (EffectProxy eff_b : op_b.get_effects()) {
//        int var_b = eff_b.get_fact().get_variable().get_id();
//        int val_b = action_effects[id_op_b][var_b];
//        int val_a = action_effects[id_op_a][var_b];
//        if (val_a!=val_b && var_b!=id_var) return false;
//        if (var_b==id_var) {
//            found = true;
//        }
//    }
    return found && op_a.get_cost() == op_b.get_cost();
}

set<int> ResourceDetection::get_equivalent_actions(TaskProxy & task_proxy, int id_var, int id_op){
    set<int> equivalent_ops;
    const OperatorsProxy & ops = task_proxy.get_operators();
    for (int i = 0; i < ops.size(); ++i){
        //if (id_op != i && are_v_equivalent(task_proxy, id_var, id_op,i)) equivalent_ops.insert(i);
        if (id_op != i && are_v_equivalent(task_proxy, id_var, id_op,i)) equivalent_ops.insert(i);
    }
    return equivalent_ops;
}

void ResourceDetection::fill_actions_preconditions_and_effects(TaskProxy & task_proxy){
    
    const OperatorsProxy & ops = task_proxy.get_operators();
    VariablesProxy vars = task_proxy.get_variables();
    
    action_preconditions.assign(ops.size(),vector<int>(vars.size(),-1));
    action_effects.assign(ops.size(),vector<int>(vars.size(),-1));
    goal_state.assign(vars.size(),-1);
    
    for (size_t i = 0; i < ops.size(); ++i){
        const OperatorProxy &op = task_proxy.get_operators()[i];
        if (rddebug) cout << op.get_name() << endl;
        for (FactProxy condition : op.get_preconditions()) {
            int var = condition.get_variable().get_id();
            int val = condition.get_value();
            action_preconditions[i][var] = val;
            if (rddebug) cout << "  prec : " << condition.get_variable().get_name() << " " << var << " " << val << " " << condition.get_name() << endl;
        }
        for (EffectProxy eff : op.get_effects()) {
            int var = eff.get_fact().get_variable().get_id();
            int val = eff.get_fact().get_value();
            action_effects[i][var] = val;
            if (rddebug) cout << "  eff : " << var << " " << val << " " << eff.get_fact().get_name() << endl;
        }
    }
    for (FactProxy goal : task_proxy.get_goals()) {
        if (rddebug) cout << "  goal : " << goal.get_variable().get_id() << " " << goal.get_name() << endl;
        goal_state[goal.get_variable().get_id()] = goal.get_value();
    }
}

void ResourceDetection::mark_type(int id_var, set<int>& actions,  ActionTypeRD action_type){
    for (int a : actions){
        if (action_type == ActionTypeRD::CONSUMER){
            if (resource_actions[id_var][a] ==  ActionTypeRD::NONE) consumer++;
        }else if (action_type == ActionTypeRD::PRODUCER){
            if (resource_actions[id_var][a] ==  ActionTypeRD::NONE) producer++;
        }else if (action_type == ActionTypeRD::NONE){
            if (resource_actions[id_var][a] ==  ActionTypeRD::CONSUMER){
                consumer--;
            }else if(resource_actions[id_var][a] ==  ActionTypeRD::PRODUCER){
                producer--;
            }
        }
        resource_actions[id_var][a] = action_type;

    }
}

list<set<int>> ResourceDetection::get_equivalent_actions(TaskProxy & task_proxy, int id_var){
    list<set<int>> equivalent_actions;
    set<int> seen_actions;
    const OperatorsProxy & ops = task_proxy.get_operators();
    for (size_t i = 0; i < ops.size(); ++i){
        int pre = action_preconditions[i][id_var];
        int eff = action_effects[i][id_var];
        if (pre == -1 || eff == -1) continue;
        if (seen_actions.find(i)!= seen_actions.end()) continue;
        set<int> eq_actions = get_equivalent_actions(task_proxy, id_var, i);

        if (!eq_actions.empty() || single_action){
            eq_actions.insert(i);
            equivalent_actions.push_back(eq_actions);
            seen_actions.insert(eq_actions.begin(),eq_actions.end());
        }
    }
    
//    // debug:
//    cout << "variable " << task_proxy.get_variables()[id_var].get_name() << " (" << task_proxy.get_variables()[id_var].get_domain_size() << ")" << endl;
//    for(set<int> ea : equivalent_actions){
//        if (ea.empty()) continue;
//        cout << "\t";
//        for (int a : ea){
//            cout << ops[a].get_name() << "; ";
//        }
//        cout << endl;
//    }
    return equivalent_actions;
}
std::pair<std::set<int>,std::set<int>> ResourceDetection::find_head_tails(TaskProxy & task_proxy, int id_var, std::set<int>& actions){
    std::pair<std::set<int>,std::set<int>> head_tail;
    for (int d = 0; d < task_proxy.get_variables()[id_var].get_domain_size(); ++d){
        bool head = true;
        bool tail = true;
        for (int a : actions){
            int pre = action_preconditions[a][id_var];
            int eff = action_effects[a][id_var];
            if (pre == d) head = false;
            if (eff == d) tail = false;
            if (!head && !tail) break;
        }
        //assert( !(head && tail));
        if (rddebug){
            if (head && tail){
                FactProxy fact = task_proxy.get_variables()[id_var].get_fact(d);
                cout << fact.get_name() << " is both head and tail" << endl;
            }
        }
        if (head) head_tail.first.insert(d);
        if (tail) head_tail.second.insert(d);
    }
    return head_tail;
}

list<int> ResourceDetection::find_ordering_value(TaskProxy & task_proxy, int id_var, std::set<int>& actions){
    list<int> ordering;
    if (!actions.empty()){
//        cout << "group for variable " << id_var << " ";
//        for (int op : actions){
//            cout << task_proxy.get_operators()[op].get_name() << "; ";
//        }
//        cout << endl;
//        cout << actions.size() << " " << task_proxy.get_variables()[id_var].get_domain_size() << endl;
//        assert( (actions.size() >= task_proxy.get_variables()[id_var].get_domain_size() - 1) || actions.empty());
        for (int op : actions){
            int pre = action_preconditions[op][id_var];
            int eff = action_effects[op][id_var] ;
            list<int>::iterator p = find (ordering.begin(), ordering.end(), pre);
            if (p==ordering.end()) ordering.push_front(pre);
            list<int>::iterator e = find (ordering.begin(), ordering.end(), eff);
            if (e==ordering.end()) ordering.insert(++find(ordering.begin(), ordering.end(), pre),eff);
           // cout << pre << " " << eff << endl;
        }
//        cout << "final list: ";
//        for (int o : ordering) cout << o << " ";
//        cout << endl;
    }
    return ordering;
}

void ResourceDetection::fill_ordering_value(TaskProxy & task_proxy, int id_var, std::set<int>& actions, std::vector<std::vector<bool>> & before){
    if (!actions.empty()){
        int max = before.size();
        for (int op : actions){
            int pre = action_preconditions[op][id_var];
            int eff = action_effects[op][id_var] ;
            before[pre][eff] = true;
            for (int i = 0; i < max; ++i){
                if (before[i][pre] && i!=pre) before[i][eff] = true;
                if (before[eff][i] && i!=eff) before[pre][i] = true;
            }
        }
        bool keep = true;
        while (keep){
            keep = false;
            for (int i = 0; i < max; ++i){
                for (int j = i; j < max; ++j){
                    if (before[i][j]){
                        for (int z = j; z < max; ++z){
                            if (before[j][z] && !before[i][z]){
                                before[i][z] = true;
                                keep = true;
                            }
                            
                        }
                    }
                }
            }
        }
        for (int i = 0; i < max; ++i){
            for (int j = 0; j < max; ++j){
                cout << before[i][j] << " ";
            }
            cout << endl;
        }
    }
    
}

void ResourceDetection::create_sets_actions(TaskProxy & task_proxy, bool numeric_mode){
    normal.clear();
    resourced.clear();
    variable_n.clear();
    variable_r.clear();
    for(size_t id_op = 0; id_op < task_proxy.get_operators().size(); ++id_op){
        bool not_mod = true;
        for (size_t id_var = 0; id_var < task_proxy.get_variables().size(); ++id_var){
            if (resource_actions[id_var][id_op] != ActionTypeRD::NONE){
                //cout << task_proxy.get_operators()[id_op].get_name() << " is modifying " << id_var << endl;
                if(numeric_mode)
                    not_mod = false;
            }
        }
        if (!not_mod){
            resourced.insert(id_op);
        }else{
            normal.insert(id_op);
        }
    }
    
    for (size_t id_var = 0; id_var < task_proxy.get_variables().size(); ++id_var){
        if (!numeric_mode){
            variable_n.insert(id_var);
        }else{
            if (!resource_variables[id_var]) variable_n.insert(id_var);
            else variable_r.insert(id_var);
        }
    }
}
