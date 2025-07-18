#include "delete_test.h"
#include "../operator_counting/state_equation_constraints.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"

#include "../axioms.h"

#include "../numeric_landmarks/landmark_factory_scala.h"
#include "../priority_queue.h"

#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace numeric_helper;

struct Proposition;

namespace operator_counting {
    
    void DeleteTestConstraints::add_actions_constraints(
                                                                    vector<lp::LPConstraint> &constraints, double infinity) {
        //cout << "add actions constraints" << endl;
        for (size_t id_op = 0; id_op < numeric_task.get_n_actions(); ++id_op) {
            
            lp::LPConstraint constraint(0., infinity);
            constraint.insert(indices_m_a[id_op], 1.);
            constraint.insert(indices_u_a[id_op], -1.);
            
            if (!constraint.empty()) {
                constraints.push_back(constraint);
            }
            
        }
    }
    
    void DeleteTestConstraints::add_initial_state_constraints(
                                                   vector<lp::LPConstraint> &constraints) {
        //cout << "add initial state constraints" << endl;
        for (size_t var = 0; var < numeric_task.get_n_vars(); ++var) {
            int num_values = numeric_task.get_n_proposition_value(var);
            if (numeric_task.is_numeric_axiom(var)) continue;
            for (int value = 0; value < num_values; ++value) {
                int index = numeric_task.get_proposition(var,value);
                // change this
                lp::LPConstraint constraint(0., 1.);
                for (int i : numeric_task.get_add_actions(var,value)){
                    constraint.insert(indices_e_a_p[i][index], -1.);
                }
                constraint.insert(indices_u_p[index], 1.);
                if (!constraint.empty()) {
                    index_constraints[var][value] = constraints.size();
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_goal_state_constraints(
                                                                    vector<lp::LPConstraint> &constraints, TaskProxy &task_proxy) {
        //cout << "add goal state constraints" << endl;
        for (size_t id_goal = 0; id_goal < task_proxy.get_goals().size(); ++id_goal) {
            FactProxy goal = task_proxy.get_goals()[id_goal];
            lp::LPConstraint constraint(1., 1.);
            if (numeric_task.is_numeric_axiom(goal.get_variable().get_id())) continue;
            constraint.insert(indices_u_p[numeric_task.get_proposition(goal.get_variable().get_id(),goal.get_value())], 1.);
            goals.insert(numeric_task.get_proposition(goal.get_variable().get_id(),goal.get_value()));
            //cout << "goal " << task_proxy.get_variables()[goal.get_variable().get_id()].get_fact(goal.get_value()).get_name() << endl;
            if (!numeric_task.numeric_goals_empty(id_goal)) continue; // this is a numeric goal
            if (!constraint.empty()) {
                constraints.push_back(constraint);
            }
        }
    }
    
    void DeleteTestConstraints::add_preconditions_constraints_inv(
                                                                 vector<lp::LPConstraint> &constraints, double infinity) {
        //cout << "add preconditions constraints" << endl;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            for (int i : numeric_task.get_action_pre_list(op_id)){
                lp::LPConstraint constraint(0., infinity);
                constraint.insert(indices_u_p[i], 1.);
                for (int j : inverse_actions[op_id]){
                    constraint.insert(indices_e_a_p[j][i], -1.);
                }
                constraint.insert(indices_u_a[op_id], -1.);
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_preconditions_constraints(
                                                                    vector<lp::LPConstraint> &constraints, double infinity) {
        //cout << "add preconditions constraints" << endl;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            for (int i : numeric_task.get_action_pre_list(op_id)){
                lp::LPConstraint constraint(0., infinity);
                constraint.insert(indices_u_p[i], 1.);
                constraint.insert(indices_u_a[op_id], -1.);
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_effects_constraints(
                                                                 vector<lp::LPConstraint> &constraints, double infinity) {
        //cout << "add effects constraints" << endl;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            for (int i : numeric_task.get_action_add_list(op_id)){
                lp::LPConstraint constraint(0, infinity);
                constraint.insert(indices_e_a_p[op_id][i], -1.);
                constraint.insert(indices_u_a[op_id], 1.);
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_sequencing_constraints(
                                                                 vector<lp::LPConstraint> &constraints, double infinity) {
        //cout << "add sequencing constraints" << endl;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            for (int i : numeric_task.get_action_pre_list(op_id)){
                lp::LPConstraint constraint(0., infinity);
                constraint.insert(indices_t_p[i], -1.);
                constraint.insert(indices_t_a[op_id], 1.);
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
            for (int i : numeric_task.get_action_add_list(op_id)){
                lp::LPConstraint constraint(-infinity, numeric_task.get_n_actions());
                constraint.insert(indices_t_p[i], -1.);
                constraint.insert(indices_t_a[op_id], 1.);
                constraint.insert(indices_e_a_p[op_id][i], (numeric_task.get_n_actions() + 1.));
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_numeric_conditions_constraints(std::vector<lp::LPConstraint> &constraints, double infinity){
        //cout << "add numeric conditions constraints" << endl;
        for (size_t i = 0; i < numeric_task.get_n_conditions(); ++i){
            LinearNumericCondition& lnc = numeric_task.get_condition(i);
            double l_b = lnc.constant;
            lp::LPConstraint constraint(-infinity, infinity);
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
                for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
                    constraint.insert(indices_m_a_c[op_id][i], lnc.coefficients[n_id]*numeric_task.get_action_eff_list(op_id)[n_id]);
                }
            }
            for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
                if (lnc.coefficients[n_id] > 0)
                    l_b += lnc.coefficients[n_id]*numeric_task.get_numeric_variable(n_id).lower_bound;
                else
                    l_b += lnc.coefficients[n_id]*numeric_task.get_numeric_variable(n_id).upper_bound;

            }
            
            constraint.insert(indices_u_c[i], numeric_task.get_small_m(i));
            if (!constraint.empty()) {
                index_constraints_numeric[i] = constraints.size();
                constraints.push_back(constraint);
            }
        }
    }
    
    void DeleteTestConstraints::add_numeric_preconditions_constraints(std::vector<lp::LPConstraint> &constraints, double infinity){
        //cout << "add numeric preconditions constraints" << endl;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            for (int pre : numeric_task.get_action_num_list(op_id)){
                for (int i : numeric_task.get_numeric_conditions_id(pre)){
                    lp::LPConstraint constraint(numeric_task.get_epsilon(i), infinity);
                    constraint.insert(indices_u_c[i], 1.);
                    constraint.insert(indices_u_a[op_id], -1.);
                    if (!constraint.empty()) {
                        constraints.push_back(constraint);
                    }
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_numeric_goals_constraints(std::vector<lp::LPConstraint> &constraints){
        for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
            list<int> numeric_goals = numeric_task.get_numeric_goals(id_goal);
            if (numeric_goals.empty()) continue; // this is not a numeric goal
            for (int id_n_con : numeric_goals){
                lp::LPConstraint constraint(1., 1.);
                constraint.insert(indices_u_c[id_n_con], 1.);
                goals.insert(id_n_con + numeric_task.get_n_propositions());
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_numeric_effects_constraints(std::vector<lp::LPConstraint> &constraints, double infinity){
        //cout << "add numeric effects constraints" << endl;
        for (size_t i = 0; i < numeric_task.get_n_conditions(); ++i){
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
                lp::LPConstraint constraint(0, infinity);
                constraint.insert(indices_e_a_c[op_id][i], -1.);
                constraint.insert(indices_u_a[op_id], 1.);
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    void DeleteTestConstraints::add_numeric_counters_constraints(std::vector<lp::LPConstraint> &constraints, double infinity){
        //cout << "add numeric counters constraints" << endl;
        for (size_t i = 0; i < numeric_task.get_n_conditions(); ++i){
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
                {
                    lp::LPConstraint constraint(0, infinity);
                    constraint.insert(indices_m_a[op_id], 1.);
                    constraint.insert(indices_m_a_c[op_id][i], -1.);
                    if (!constraint.empty()) {
                        constraints.push_back(constraint);
                    }
                }
                {
                    lp::LPConstraint constraint(0, infinity);
                    constraint.insert(indices_m_a_c[op_id][i], -1.);
                    constraint.insert(indices_e_a_c[op_id][i], bigM);
                    if (!constraint.empty()) {
                        constraints.push_back(constraint);
                    }
                }
            }
        }
    }
    
    void DeleteTestConstraints::add_numeric_sequencing_constraints(
                                                                 vector<lp::LPConstraint> &constraints, double infinity) {
        //cout << "add numeric sequencing constraints" << endl;
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            for (int pre : numeric_task.get_action_num_list(op_id)){
                for (int i : numeric_task.get_numeric_conditions_id(pre)){
                    lp::LPConstraint constraint(0., infinity);
                    constraint.insert(indices_t_c[i], -1.);
                    constraint.insert(indices_t_a[op_id], 1.);
                    if (!constraint.empty()) {
                        constraints.push_back(constraint);
                    }
                }
            }
            for (size_t i = 0; i < numeric_task.get_n_conditions(); ++i){
                lp::LPConstraint constraint(-infinity, numeric_task.get_n_actions());
                constraint.insert(indices_t_c[i], -1.);
                constraint.insert(indices_t_a[op_id], 1.);
                constraint.insert(indices_e_a_c[op_id][i], (numeric_task.get_n_actions() + 1.));
                if (!constraint.empty()) {
                    constraints.push_back(constraint);
                }
            }
        }
    }
    
    void DeleteTestConstraints::initialize_variables(
                              const std::shared_ptr<AbstractTask> task,
                              std::vector<lp::LPVariable> &variables,
                              double infinity){
        //cout << "Initializing variables from delete relaxation." << endl;
        // why this is not passed?
        TaskProxy task_proxy(*task);
        numeric_task = NumericTaskProxy(task_proxy);

        bigM = 100000;
        OperatorsProxy ops = task_proxy.get_operators();
        int n_ops = ops.size();
        indices_m_a.assign(n_ops,-1);
        indices_u_a.assign(n_ops,-1);
        indices_t_a.assign(n_ops,-1);
        indices_e_a_p.assign(n_ops,vector<int>(numeric_task.get_n_propositions(),-1));
        //cout << "assigning " << n_ops << " " << numeric_task.get_n_propositions() << " " <<  indices_e_a_p[0].size() << endl;
        indices_e_a_c.assign(n_ops,vector<int>(numeric_task.get_n_conditions(),-1));
        indices_m_a_c.assign(n_ops,vector<int>(numeric_task.get_n_conditions(),-1));
        indices_u_c.assign(numeric_task.get_n_conditions(),-1);
        indices_t_c.assign(numeric_task.get_n_conditions(),-1);
        //
        fact_eliminated.assign(numeric_task.get_n_propositions(),false);
        action_eliminated.assign(n_ops,false);
        
        index_constraints.resize(numeric_task.get_n_vars());
        index_constraints_numeric.assign(numeric_task.get_n_conditions(),-1);
        factory = utils::make_unique_ptr<landmarks::LandmarkFactoryScala>(task);
        
        relevant_actions.assign(numeric_task.get_n_actions(),false);
        relevant_facts.assign(numeric_task.get_n_propositions() + numeric_task.get_n_conditions(),false);

        for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
            indices_m_a[op_id] = op_id;
            indices_u_a[op_id] = variables.size();
            variables.push_back(lp::LPVariable(0, 1, 0, "ua_" + ops[op_id].get_name()));
            //cout << op_id << " " << ops[op_id].get_name() << endl;
    
            indices_t_a[op_id] = variables.size();
            variables.push_back(lp::LPVariable(0, n_ops, 0, "ta_" + ops[op_id].get_name()));
            
            for (int proposition : numeric_task.get_action_add_list(op_id)){
                indices_e_a_p[op_id][proposition] = variables.size();
                stringstream name;
                name << "ep_" << ops[op_id].get_name() << "_" << proposition;
                variables.push_back(lp::LPVariable(0, 1, 0, name.str()));
            }
            
            for (size_t var = 0; var < numeric_task.get_n_conditions(); ++var) {
                {
                    indices_e_a_c[op_id][var] = variables.size();
                    stringstream name;
                    name << "ec_" << ops[op_id].get_name() << "_" << var;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str()));
                }
                {
                    indices_m_a_c[op_id][var] = variables.size();
                    stringstream name;
                    name << "mac_" << ops[op_id].get_name() << "_" << var;
                    variables.push_back(lp::LPVariable(0, infinity, 0, name.str()));
                }
            }
        }
        // add propositions
        indices_u_p.assign(numeric_task.get_n_propositions(),-1);
        indices_t_p.assign(numeric_task.get_n_propositions(),-1);
        for (size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
            int num_values = numeric_task.get_n_proposition_value(var);
            index_constraints[var].resize(num_values);//,-1);
            for (int value = 0; value < num_values; ++value) {
                int pr_id = numeric_task.get_proposition(var,value);
                {
                    indices_u_p[pr_id] = variables.size();
                    stringstream name;
                    name << "up_" << pr_id;
                    variables.push_back(lp::LPVariable(0, 1, 0, name.str()));
                }
                {
                    indices_t_p[pr_id] = variables.size();
                    stringstream name;
                    name << "tp_" << pr_id;
                    variables.push_back(lp::LPVariable(0, n_ops, 0, name.str()));
                }
                
                //cout << "in add var constraints " <<  var << " " << value << " " << index_constraints[var][value] << " " << endl;

            }
        }
        

        for (size_t var = 0; var < numeric_task.get_n_conditions(); ++var) {
            {
                indices_u_c[var] = variables.size();
                stringstream name;
                name << "uc_" << var;
                variables.push_back(lp::LPVariable(0, 1, 0, name.str()));
            }
            {
                indices_t_c[var] = variables.size();
                stringstream name;
                name << "tc_" << var;
                variables.push_back(lp::LPVariable(0, n_ops, 0, name.str()));
            }
        }
    }
    
    void DeleteTestConstraints::iterative_variable_elimination(const State &state, vector<bool> &fact_eliminated, vector<bool> &action_eliminated){
        // TODO add if condition
        fill(fact_eliminated.begin(),fact_eliminated.end(),false);
        fill(action_eliminated.begin(), action_eliminated.end(), false);
        
        // extract landmark
        if (landmark_constraints){
            vector<set<int>> & landmarks_table = factory->get_landmarks_table();
            build_first_achiever(landmarks_table);
        }else{
            build_achiever();
        }
        if (!enhanced_constraints) return;
        bool iterate = true;
        
        relevant_action_reduction(state);

        while(iterate){
            iterate = enhanced_seq_constraints ? dominated_seq_action_elimination(state) : dominated_action_elimination(state);
            if (iterate)
                iterate = relevant_action_reduction(state);
        }
    }
    
    bool DeleteTestConstraints::dominated_action_elimination(const State &state){
        bool changed = false;
        // check among the relevant actions only
        for (size_t i = 0; i < numeric_task.get_n_actions(); ++i){
            if (action_eliminated[i]) continue;
            bool dominated = false;
            int dm = 0;
            for (size_t j = 0; j < numeric_task.get_n_actions(); ++j){
                if ( i == j ) continue;
                if (action_eliminated[j]) continue;
                // condition (iii)
                bool check = numeric_task.get_action_cost(i) >= numeric_task.get_action_cost(j);
                if (!check) continue;
                
                check = dominated_action_first_condition(i,j);
                if (!check) continue;

                check = dominated_action_second_condition(i,j,state);
                if (!check) continue;
                
                check = dominated_action_fourth_condition(i,j);
                if (!check) continue;
                
                dominated = true;
                dm = j;
                break;
            }
            if (dominated) {
//                cout << "action " << i << "is dominated by " << dm << endl;
                action_eliminated[i] = true;
                changed = true;
                
            }
        }
        return changed;
    }
    
    bool DeleteTestConstraints::dominated_seq_action_elimination(const State &state){
        bool changed = false;
        // check among the relevant actions only
        for (size_t i = 0; i < numeric_task.get_n_actions(); ++i){
            if (action_eliminated[i]) continue;
            bool dominated = false;
            int dm = 0;
            for (size_t j = 0; j < numeric_task.get_n_actions(); ++j){
                if ( i == j ) continue;
                if (action_eliminated[j]) continue;
                // condition (iii)
                bool check = numeric_task.get_action_cost(i) >= numeric_task.get_action_cost(j);
                if (!check) continue;
                
                check = dominated_action_first_condition(i,j);
                if (!check) continue;
                
                check = dominated_action_second_condition(i,j,state);
                if (!check) continue;
                
                check = dominated_action_seq_fourth_condition(i);
                if (!check) continue;
                
                check = dominated_seq_condition(i,j);
                if (!check) continue;
                
                dominated = true;
                dm = j;
                break;
            }
            if (dominated) {
                //                cout << "action " << i << "is dominated by " << dm << endl;
                action_eliminated[i] = true;
                changed = true;
                
            }
        }
        return changed;
    }
    
    bool DeleteTestConstraints::dominated_action_first_condition(int i, int j){
        
        for (int p : numeric_task.get_action_add_list(i)){
            if (fadd[i][p] && !fadd[j][p]) return false;
        }
        for (size_t p = numeric_task.get_n_propositions(); p < numeric_task.get_n_propositions() + numeric_task.get_n_conditions(); ++p){
            if (fadd[i][p] && !fadd[j][p]) return false;
        }
        return true;
    }

    bool DeleteTestConstraints::dominated_action_second_condition(int i, int j, const State &state){
        for (int p : numeric_task.get_action_pre_list(j)){
            int var = numeric_task.get_var(p);
            if ( !action_landmarks[i][p] && state[var].get_value() ) return false;
        }
        return true;
    }
    
    // this is state-independent, can be precomputed
    bool DeleteTestConstraints::dominated_seq_condition(int i, int j){
        set<int> &set_i = numeric_task.get_action_pre_del_list(i);
        set<int> &set_j = numeric_task.get_action_pre_del_list(j);
        if (set_i.size() < set_j.size()) return false;
        for (int p : set_j){
            if (set_i.find(p) != set_i.end()) return false;
        }
        return true;
    }
    
    // this is state-independent, can be precomputed
    bool DeleteTestConstraints::dominated_action_fourth_condition(int i, int j){
        for (size_t v = 0; v < numeric_task.get_n_numeric_variables(); ++v){
            double vi = numeric_task.get_action_eff_list(i)[v];
            double vj = numeric_task.get_action_eff_list(j)[v];
            if (vi*vj < 0 || fabs(vi) >= fabs(vj)) return false;
        }
        return true;
    }
    
    // only actions with no numeric effects can be dominated (this is also state independent)
    bool DeleteTestConstraints::dominated_action_seq_fourth_condition(int i){
        for (size_t v = 0; v < numeric_task.get_n_numeric_variables(); ++v){
            double vi = numeric_task.get_action_eff_list(i)[v];
            if (fabs(vi) > 0) return false;
        }
        return true;
    }
    
    bool DeleteTestConstraints::relevant_action_reduction(const State &state){
        bool changed = false;
        fill(relevant_actions.begin(),relevant_actions.end(),false);
        fill(relevant_facts.begin(),relevant_facts.end(),false);
        stack<int> queue;
        // fill queue with goals
        for (int g : goals) queue.push(g);
        
        while (!queue.empty()){
            int p = queue.top();
            queue.pop();
            if (!relevant_facts[p]){
                relevant_facts[p] = true;
                for (int op_id : first_achievers[p]){
                    if (action_eliminated[op_id]) continue;
                    if (!relevant_actions[op_id]){
                        relevant_actions[op_id] = true;
                        // add preconditions to queue
                        for (int i : numeric_task.get_action_pre_list(op_id)) queue.push(i);
                        for (int pre : numeric_task.get_action_num_list(op_id)){
                            for (int i : numeric_task.get_numeric_conditions_id(pre)){
                                queue.push(i+numeric_task.get_n_propositions());
                            }
                        }
                    }
                }
            } else {
            }
        }
        
        // initial state (if fact is in initial state, we already say that is true, so we cannot say
        // that is not irrelevant
        for (size_t var = 0; var < numeric_task.get_n_vars(); ++var) {
            if (numeric_task.is_numeric_axiom(var)) continue;
            relevant_facts[numeric_task.get_proposition(var,state[var].get_value())] = true;
        }

        for (size_t i = 0; i < relevant_actions.size(); ++i){
            if (!relevant_actions[i]) {
                action_eliminated[i] = true;
            }
        }
        
        for (size_t i = 0; i < relevant_facts.size(); ++i){
            if (!relevant_facts[i] && fact_eliminated[i] == false) changed = true;
            if (!relevant_facts[i]) {
                fact_eliminated[i] = true;
//                if (i < numeric_task.get_n_propositions()){
//                    cout << i << " " << numeric_task.get_proposition_name(i) << " eliminated " << endl;
//                }else{
//                    cout << i << " - " << i << " eliminated " << endl;
//
//                }
            }else{
//                if (i < numeric_task.get_n_propositions()){
//                    cout << i << " " << numeric_task.get_proposition_name(i) << " relevant " << endl;
//                }else{
//                    cout << i << " - " << i << " relevant " << endl;
//                }
            }
        }
        return changed;
    }
    
    void DeleteTestConstraints::build_first_achiever(vector<set<int>> &landmarks_table){
        fadd.assign(numeric_task.get_n_actions(),vector<bool>(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false));
        action_landmarks.assign(numeric_task.get_n_actions(),vector<bool>(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false));
        
        first_achievers.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),set<int>());
        
        // find action landmarks
        int n_propositions = numeric_task.get_n_propositions();
        for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
            
            set<int> & pre_list = numeric_task.get_action_pre_list(op_id);
            for (int p : pre_list){
                set<int> & landmarks = landmarks_table[p];
                for (int l : landmarks) action_landmarks[op_id][l] = true;
            }
            
            set<int> & num_list = numeric_task.get_action_num_list(op_id);
            //cout << "op : " << op_id << endl;
            for (int pre : num_list){
                //cout << "\tpre : " << pre << endl;
                for (int c : numeric_task.get_numeric_conditions_id(pre)){
                    //cout << "\t\tc : " << c << endl;
                    set<int> & landmarks = landmarks_table[c + n_propositions];
                    for (int l : landmarks) action_landmarks[op_id][l] = true;
                }
            }
            
            // now find the first achievers
            set<int> & add_list = numeric_task.get_action_add_list(op_id);
            for (int p : add_list){
                if (!action_landmarks[op_id][p]) {
                    fadd[op_id][p] = true;
                    first_achievers[p].insert(op_id);
                }
            }
            
            set<int> & possible_add_list = numeric_task.get_action_possible_add_list(op_id);
            for (int c : possible_add_list) {
                if (!action_landmarks[op_id][c]) {
                    fadd[op_id][c] = true;
                    first_achievers[c].insert(op_id);
                }
            }
        }
    }
    
    void DeleteTestConstraints::build_achiever(){
        fadd.assign(numeric_task.get_n_actions(),vector<bool>(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false));
        action_landmarks.assign(numeric_task.get_n_actions(),vector<bool>(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false));
        
        first_achievers.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),set<int>());
        
        // find action landmarks
        for (size_t fact_id = 0; fact_id < numeric_task.get_n_propositions()+numeric_task.get_n_conditions(); ++fact_id){
            first_achievers[fact_id] = numeric_task.get_achievers(fact_id);
            for (size_t op_id : first_achievers[fact_id]) fadd[op_id][fact_id] = true;
        }
    }
    
    void DeleteTestConstraints::inverse_action_detection(){
        
        int n_ops = numeric_task.get_n_actions();
        inverse_actions.assign(n_ops,set<int>());
        for (int i = 0; i < n_ops; ++i){
            for (int j = 0; j < n_ops; ++j){
                // TODO: for inverse action you can use effects on numeric conditions
                set<int> & pre_i = numeric_task.get_action_pre_list(i);
                set<int> & pre_j = numeric_task.get_action_pre_list(j);
                set<int> & add_i = numeric_task.get_action_add_list(i);
                set<int> & add_j = numeric_task.get_action_add_list(j);

                if (set_include(add_i, pre_j) && set_include(add_j, pre_i)){
                    bool numeric_part = true;
                    for (size_t v = 0; v < numeric_task.get_n_numeric_variables(); ++v){
                        double k_i = numeric_task.get_action_eff_list(i)[v];
                        double k_j = numeric_task.get_action_eff_list(j)[v];
                        if (k_i * k_j >= 0){
                            numeric_part = false;
                            break;
                        }
                    }
                    if (numeric_part){
                        inverse_actions[i].insert(j);
                        //cout << i << " " << j << " are inverse actions " << endl;
                    }
                }
            }
        }
    }
    
    bool DeleteTestConstraints::set_include(set<int> &first, set<int> &second){
        return std::includes(first.begin(), first.end(),
                             second.begin(), second.end());
    }
    void DeleteTestConstraints::initialize_constraints(
                                                          const shared_ptr<AbstractTask> task, vector<lp::LPConstraint> &constraints,
                                                          double infinity) {
        //cout << "Initializing constraints from delete relaxation" << endl;
        TaskProxy task_proxy(*task);
        //verify_no_axioms(task_proxy);
        verify_no_conditional_effects(task_proxy);
        add_actions_constraints(constraints,infinity);
        add_initial_state_constraints(constraints);
        add_goal_state_constraints(constraints, task_proxy);
        if (basic_constraints){
            if (inverse_constraints){
                inverse_action_detection();
                add_preconditions_constraints_inv(constraints, infinity);
            }else{
                add_preconditions_constraints(constraints, infinity);
            }
        }
        add_effects_constraints(constraints, infinity);
        if(basic_constraints && temporal_constraints) add_sequencing_constraints(constraints, infinity);
        add_numeric_conditions_constraints(constraints, infinity);
        if (basic_constraints) add_numeric_preconditions_constraints(constraints, infinity);
        add_numeric_goals_constraints(constraints);
        add_numeric_effects_constraints(constraints, infinity);
        add_numeric_counters_constraints(constraints, infinity);
        if(basic_constraints && temporal_constraints) add_numeric_sequencing_constraints(constraints, infinity);
        
  }
    
    bool DeleteTestConstraints::update_constraints(const State &state,
                                                      lp::LPSolver &lp_solver) {
        
        set<int> fact_landmarks;
        set<int> action_landmarks;
        if (landmark_constraints){
            fact_landmarks = factory->compute_landmarks(state);
            action_landmarks = factory->compute_action_landmarks(fact_landmarks);
            iterative_variable_elimination(state,fact_eliminated,action_eliminated);
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++ op_id){
                lp_solver.set_variable_lower_bound(indices_u_a[op_id], 0);
            }
        }

        // Compute the bounds for the rows in the LP.
        for (size_t var = 0; var < numeric_task.get_n_vars(); ++var) {
            int num_values = numeric_task.get_n_proposition_value(var);
            if (numeric_task.is_numeric_axiom(var)) continue;
            for (int value = 0; value < num_values; ++value) {
                double lower_bound = 0;
                if (state[var].get_value() == value) {
                    lower_bound = 1;
                }
                int fact_id = numeric_task.get_proposition(var, value);
                lp_solver.set_constraint_lower_bound(index_constraints[var][value], lower_bound);
                lp_solver.set_constraint_upper_bound(index_constraints[var][value], lower_bound);
                if (landmark_constraints){
                    lp_solver.set_variable_lower_bound(indices_u_p[fact_id], 0);
                    lp_solver.set_variable_upper_bound(indices_u_p[fact_id], 1);

                    //first achievers
                    for (size_t id_op = 0; id_op < numeric_task.get_n_actions(); ++id_op){
                        if (fadd[id_op][fact_id]){
                            lp_solver.set_variable_upper_bound(indices_e_a_p[id_op][fact_id], 1);
                        }else{
                            if (indices_e_a_p[id_op][fact_id] >= 0)
                                lp_solver.set_variable_upper_bound(indices_e_a_p[id_op][fact_id], 0);
                        }
                    }
                }
            }
        }

        for (size_t var = 0; var < numeric_task.get_n_conditions(); ++var) {
            LinearNumericCondition &num_values = numeric_task.get_condition(var);
            double lower_bound = numeric_task.get_small_m(var) - num_values.constant + numeric_task.get_epsilon(var);
            for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
                int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
                lower_bound -= state.nval(id_num) * num_values.coefficients[i];
            }
            lp_solver.set_constraint_lower_bound(index_constraints_numeric[var], lower_bound);
            //lp_solver.set_constraint_lower_bound(index_constraints_numeric[var], lower_bound);

       
            if (landmark_constraints && basic_constraints){
                lp_solver.set_variable_lower_bound(indices_u_c[var], 0);
                int fact_id = var + numeric_task.get_n_propositions();
                for (size_t id_op = 0; id_op < numeric_task.get_n_actions(); ++id_op){
                    if (fadd[id_op][fact_id]){
                        lp_solver.set_variable_upper_bound(indices_e_a_c[id_op][var], 1);
                    }else{
                        if (indices_e_a_c[id_op][var] >= 0)
                            lp_solver.set_variable_upper_bound(indices_e_a_c[id_op][var], 1);
                    }
                }
            }
        }

        if (enhanced_constraints){
            for (size_t i = 0; i < fact_eliminated.size(); ++i){
                if (fact_eliminated[i]){
                    if (fact_landmarks.find(i)!=fact_landmarks.end()) continue;
                    if (i < numeric_task.get_n_propositions()){
                        lp_solver.set_variable_upper_bound(indices_u_p[i], 0);
                    }else{
                        lp_solver.set_variable_upper_bound(indices_u_c[i - numeric_task.get_n_propositions()], 0);
                        
                    }
                }else{
                    if (i < numeric_task.get_n_propositions()){
                        lp_solver.set_variable_upper_bound(indices_u_p[i], 1);
                    }else{
                        lp_solver.set_variable_upper_bound(indices_u_c[i - numeric_task.get_n_propositions()], 1);
                        
                    }
                }
                
            }
            
            // actions
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++ op_id){
                if (action_landmarks.find(op_id)!=action_landmarks.end()) continue;
                if (action_eliminated[op_id])
                    lp_solver.set_variable_upper_bound(indices_u_a[op_id], 0);
                else
                    lp_solver.set_variable_upper_bound(indices_u_a[op_id], 1);
            }
        }

        for (size_t i : fact_landmarks){
            if (i < numeric_task.get_n_propositions()){
                //cout << numeric_task.get_proposition_name(i) << " is a fact landmark" << endl;
                // if not a int var
                if(!numeric_task.is_numeric_axiom(numeric_task.get_var(i))){
                    lp_solver.set_variable_lower_bound(indices_u_p[i], 1);
                }
            } else {
                //cout << numeric_task.get_condition(i - numeric_task.get_n_propositions()) << " is a condition landmark" << endl;
                lp_solver.set_variable_lower_bound(indices_u_c[i - numeric_task.get_n_propositions()], 1);
            }
        }

        for (size_t i : action_landmarks){
            lp_solver.set_variable_lower_bound(indices_u_a[i], 1);
        }
        
        return false;
    }
    
    static shared_ptr<ConstraintGenerator> _parse(OptionParser &parser) {
        parser.document_synopsis(
                                 "Delete relaxation constraints",
                                 "For details, see" + utils::format_paper_reference(
                                                                                 {"Tatsuya Imai", "Alex Fukunaga"},
                                                                                  "On a practical, integer-linear programming model for delete-free tasks and its use as a heuristic for cost-optimal planning",
                                                                                                           "https://doi.org/10.1613/jair.4936",
                                                                                                           "Journal of Artificial Intelligence Research",
                                                                                                           "631-677",
                                                                                                           "2015"));
        add_dr_test_solver_option_to_parser(parser);
        Options opts = parser.parse();
        vector<string> constraints = opts.get_list<string>("test_constraints");
        for (string c : constraints){
            if (c == "basic") DeleteTestConstraints::basic_constraints = true;
            else if (c == "landmarks")  DeleteTestConstraints::landmark_constraints = true;
            else if (c == "enhanced")  DeleteTestConstraints::enhanced_constraints = true;
            else if (c == "enhanced_seq")  {
                DeleteTestConstraints::enhanced_constraints = true;
                DeleteTestConstraints::enhanced_seq_constraints = true;
            }
            else if (c == "inverse")  DeleteTestConstraints::inverse_constraints = true;
            else if (c == "temporal") DeleteTestConstraints::temporal_constraints = true;
            else cout <<"unknown option" << endl;

        }
        if (parser.help_mode())
            return nullptr;
        if (parser.dry_run())
            return nullptr;
        return make_shared<DeleteTestConstraints>();
    }
    
    static PluginShared<ConstraintGenerator> _plugin("test_constraints", _parse);
 
    void add_dr_test_solver_option_to_parser(OptionParser &parser) {
        parser.document_note(
                             "Note",
                             "add extra constraints");
        parser.add_list_option<string>("test_constraints","add list options");
    }
    
    bool DeleteTestConstraints::basic_constraints = true;
    bool DeleteTestConstraints::landmark_constraints = false;
    bool DeleteTestConstraints::enhanced_constraints = false;
    bool DeleteTestConstraints::enhanced_seq_constraints = false;
    bool DeleteTestConstraints::inverse_constraints = false;
    bool DeleteTestConstraints::temporal_constraints = true;

}
