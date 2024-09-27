#ifndef RESOURCE_DETECTION_H
#define RESOURCE_DETECTION_H

#include "../search_engine.h"
#include "../task_proxy.h"
#include "../lp/lp_solver.h"
#include <map>

namespace options {
    class Options;
}

namespace rd{
    enum class ActionTypeRD {
        CONSUMER, PRODUCER, NONE
    };
    struct CompareSet {
        bool operator()(std::set<int> & lhs, std::set<int> & rhs) {return rhs.size() < lhs.size();}
    };

    class ResourceDetection {
        protected:
        void initialize() {};
        std::vector<std::vector<double>> resource_mu;
        std::vector<std::vector<double>> resource_delta;
        std::vector<double> resource_max;
        bool single_action;
        int consumer;
        int producer;
        int single;
        std::vector<std::vector<ActionTypeRD>> resource_actions; // first index variable, second index action, value: weather the action is a producer, a consumer or nothing;
        void get_resource_actions(TaskProxy & task_proxy);
        bool are_v_equivalent(TaskProxy & task_proxy, int id_var, int id_op_a, int id_op_b);
        bool are_v_equivalent_test(TaskProxy & task_proxy, int id_var, int id_op_a, int id_op_b);
        std::vector<std::vector<std::vector<int>>> equivalent_actions;
        void mark_type(int id_var, std::set<int>& actions,  ActionTypeRD action_type);
        std::list<int> find_ordering_value(TaskProxy & task_proxy, int id_var, std::set<int>& actions);
        std::pair<std::set<int>,std::set<int>> find_head_tails(TaskProxy & task_proxy, int id_var, std::set<int>& actions);
        void fill_ordering_value(TaskProxy & task_proxy, int id_var, std::set<int>& actions, std::vector<std::vector<bool>> & before);
        std::vector<std::vector<int>> action_preconditions;
        std::vector<std::vector<int>> action_effects;
        std::vector<std::vector<std::vector<bool>>> v_equivalent_actions;
        std::vector<int> goal_state;
        // for writing
        std::set<int> normal;
        std::set<int> resourced;
        std::set<int> variable_n;
        std::set<int> variable_r;
        
        void populate_v_equivalente_actions(TaskProxy & task_proxy);
        void fill_actions_preconditions_and_effects(TaskProxy & task_proxy);
        bool solve_lp(TaskProxy & task_proxy, int id_var, std::vector<double> &solution);
        std::vector<bool> resource_variables;
        public:
        void create_sets_actions(TaskProxy & task_proxy, bool numeric_mode = true);
        explicit ResourceDetection(TaskProxy & task_proxy, bool single = true);
        ~ResourceDetection();
        
        bool condition_first(int id_var);
        bool condition_second(TaskProxy & task_proxy, int id_var);
        bool condition_third(TaskProxy & task_proxy, int id_var);
        bool same_order(std::list<int>&list_a, std::list<int> &list_b);
        bool same_order(std::set<int>&list_a, int id_var, std::vector<std::vector<bool>> &before);
        bool same_order(std::pair<std::set<int>,std::set<int>> &reference, std::pair<std::set<int>,std::set<int>> &compare);
        std::vector<std::vector<ActionTypeRD>> get_action_resource_type(){
            return resource_actions;
        }
        
        std::set<int> & variables_unchanged(){
            return variable_n;
        }
        std::set<int> & variables_resource() {
            return variable_r;
        }
        
        std::set<int> & operators_unchanged(){
            return normal;
        }
        
        std::set<int> & operator_resource(){
            return resourced;
        }
        
        std::list<std::set<int>> get_equivalent_actions(TaskProxy & task_proxy, int id_var);
        std::set<int> get_equivalent_actions(TaskProxy & task_proxy, int id_var, int id_op);
        double get_delta(int op_id, int var_id){
            return resource_delta[var_id][op_id];
        }
        
        double get_mu(int var_id,int value) const{
            return resource_mu[var_id][value];
        }
        double get_max(int var_id){
            return resource_max[var_id];
        }
        
        bool is_propositional_variable(int var_id){
            return variable_n.find(var_id)!=variable_n.end();
        }
        
        ActionTypeRD get_action_behaviour(int op_id, int var_id){
            return resource_actions[var_id][op_id];
        }
        
        int get_action_effects(int id_op, int id_var){
            return action_effects[id_op][id_var];
        }

    };
}

#endif

