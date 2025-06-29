#ifndef EXECUTE_RESOURCE_DETECTION_H
#define EXECUTE_RESOURCE_DETECTION_H

#include "../search_engine.h"
#include "../task_proxy.h"
#include "../lp/lp_solver.h"
#include <map>
#include "resource_detection.h"

namespace options {
    class Options;
}

namespace rd{
//    enum class ActionTypeRD {
//        CONSUMER, PRODUCER, NONE
//    };
//
    class ExecuteDetection : public SearchEngine {
        protected:
        virtual void initialize() override {};
        virtual SearchStatus step() override {};
//        std::vector<std::vector<double>> resource_mu;
//        std::vector<std::vector<double>> resource_delta;
//        std::vector<double> resource_max;
//        std::vector<std::vector<ActionTypeRD>> resource_actions; // first index variable, second index action, value: weather the action is a producer, a consumer or nothing;
//        void get_resource_actions(TaskProxy & task_proxy);
//        bool are_v_equivalent(TaskProxy & task_proxy, int id_var, int id_op_a, int id_op_b);
//        std::list<std::set<int>> get_equivalent_actions(TaskProxy & task_proxy, int id_var);
//        std::set<int> get_equivalent_actions(TaskProxy & task_proxy, int id_var, int id_op);
//        void mark_type(int id_var, std::set<int>& actions,  ActionTypeRD action_type);
//        std::list<int> find_ordering_value(TaskProxy & task_proxy, int id_var, std::set<int>& actions);
//        std::vector<std::vector<int>> action_preconditions;
//        std::vector<std::vector<int>> action_effects;
//        std::vector<int> goal_state;
//        // for writing
//        std::set<int> normal;
//        std::set<int> resourced;
//        std::set<int> variable_n;
//        std::set<int> variable_r;
//
//        void fill_actions_preconditions_and_effects(TaskProxy & task_proxy);
//        bool solve_lp(TaskProxy & task_proxy, int id_var, std::vector<double> &solution);
//        std::vector<bool> resource_variables;
//        void create_sets_actions(TaskProxy & task_proxy, bool numeric_mode);
        ResourceDetection * resource_detection;
        public:
        explicit ExecuteDetection(const options::Options &opts);
        virtual ~ExecuteDetection() override;
        virtual void print_statistics() const override{};

//        bool condition_first(int id_var);
//        bool condition_second(TaskProxy & task_proxy, int id_var);
//        bool condition_third(TaskProxy & task_proxy, int id_var);
//        bool same_order(std::list<int>&list_a, std::list<int> &list_b);
//        std::vector<std::vector<ActionTypeRD>> get_action_resource_type(){
//            return resource_actions;
//        }
        //void write_sas(TaskProxy & task_proxy, bool numeric_mode = true);
        void write_domain(TaskProxy & task_proxy, bool numeric_mode = true);
        

        
        
        
    };
}

#endif
