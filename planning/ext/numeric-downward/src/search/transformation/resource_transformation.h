#ifndef TASKS_RESOURCE_TASK_H
#define TASKS_RESOURCE_TASK_H

#include <memory>
#include "numeric_explicit_task.h"
#include "../task_proxy.h"
#include "../plugin.h"
#include "../resource_detection/resource_detection.h"

class AbstractTask;

using namespace std;
using namespace numeric_tasks;

namespace extra_tasks {
    extern std::shared_ptr<AbstractTask> create_resource_task(
                                                                            const std::shared_ptr<AbstractTask> &parent);
    
    /*
     Task transformation for TNF tasks.
     
     Since the constructor has to be called with the correct arguments,
     describing a task in TNF, we keep the class in the .cc file and
     provide a factory function in the header file for creating TNF tasks.
     */
    class ResourceTask : public numeric_tasks::NumericExplicitTask {
        public:
        ResourceTask(
                                 const shared_ptr<AbstractTask> &parent,
                                 vector<numeric_tasks::NumericExplicitVariable> &&variables,
                                 vector<numeric_tasks::NumericExplicitNumericVariable> &&num_variables,
                                 vector<numeric_tasks::NumericExplicitComparison> &&num_comp,
                                 vector<numeric_tasks::NumericExplicitAssignment> &&num_effects,
                                 vector<vector<set<Fact>>> &&mutex_facts,
                                 vector<numeric_tasks::NumericExplicitOperator> &&operators,
                                 vector<int> &&initial_state_values,
                     vector<Fact> &&goals, map<int,int> && vars_map, map<int,int> && num_vars_map, map<int,int> && operator_map, rd::ResourceDetection &rd)
        : NumericExplicitTask(
                       parent,
                       move(variables),
                       move(num_variables),
                       move(num_comp),
                       move(num_effects),
                       move(mutex_facts),
                       move(operators),
                       {},
                       move(initial_state_values),
                       move(goals), move(vars_map),
                       move(num_vars_map), move(operator_map)), resource_detection(rd), variables_resource(rd.variables_resource()) {
            assert(variables.size() == goals.size());
        }
        ResourceTask(
                                 const shared_ptr<AbstractTask> &parent,
                                 vector<numeric_tasks::NumericExplicitVariable> &&variables,
                                 vector<numeric_tasks::NumericExplicitNumericVariable> &&num_variables,
                                 vector<numeric_tasks::NumericExplicitComparison> &&num_comp,
                                 vector<numeric_tasks::NumericExplicitAssignment> &&num_effects,
                                 vector<vector<set<Fact>>> &&mutex_facts,
                                 vector<numeric_tasks::NumericExplicitOperator> &&operators,
                                 vector<numeric_tasks::NumericExplicitOperator> &&axioms,
                                 vector<int> &&initial_state_values,
                                 vector<Fact> &&goals, map<int,int> && vars_map, map<int,int> && num_vars_map, map<int,int> && operator_map, rd::ResourceDetection &rd)
        : NumericExplicitTask(
                       parent,
                       move(variables),
                       move(num_variables),
                       move(num_comp),
                       move(num_effects),
                       move(mutex_facts),
                       move(operators),
                       move(axioms),
                       move(initial_state_values),
                       move(goals),
                       move(vars_map),
                       move(num_vars_map), move(operator_map)), resource_detection(rd) {
            assert(variables.size() == goals.size());
        }
        virtual std::vector<ap_float> get_numeric_state_values(const GlobalState &global_state) const override;
        
        
        static std::vector<numeric_tasks::NumericExplicitVariable> create_variables(
                                                                     const TaskProxy &parent_task_proxy,
                                                                     rd::ResourceDetection &resource_detection);
        static std::vector<numeric_tasks::NumericExplicitNumericVariable> create_numeric_variables(
                                                                                    const TaskProxy &parent_task_proxy,
                                                                                                   rd::ResourceDetection &resource_detection, std::map<int,int> &map_num_vars, std::map<int,int> &map_num_max);
        static vector<int> create_initial_state(const TaskProxy &parent_task_proxy, rd::ResourceDetection &resource_detection, map<int,int>& map_vars);
        static vector<int> get_precondition_values(const OperatorProxy &op, int num_vars);
        static vector<int> get_effect_values(const OperatorProxy &op, int num_vars);
        static set<int> get_ordered_mentioned_variables(const OperatorProxy &op);
        static vector<vector<set<Fact>>> create_mutexes( const TaskProxy &parent_task_proxy);
        static vector<Fact> create_goals(const TaskProxy &parent_task_proxy, map<int,int> &map_variables);
        static void create_unchanged_operators( const TaskProxy &parent_task_proxy,
                                            set<int> &id_op,
                                            vector<numeric_tasks::NumericExplicitOperator> &operators, map<int,int> &map_variables, map<int,int> &map_operators);
        static numeric_tasks::NumericExplicitOperator create_unchanged_operator( const TaskProxy &parent_task_proxy,
                                              const OperatorProxy &op,std::map<int,int> &map_variables);
        
        static void create_numeric_operators( TaskProxy &parent_task_proxy,
                                             rd::ResourceDetection &resource_detection,                                     vector<numeric_tasks::NumericExplicitOperator> &operators, map<int,int> &map_variables, map<int,int> &map_numeric_variables, map<int,int> &map_numeric_max, vector<numeric_tasks::NumericExplicitComparison>& comps, vector<numeric_tasks::NumericExplicitAssignment> & ass, vector<numeric_tasks::NumericExplicitNumericVariable> &num_vars, std::vector<int> &initial_state, std::vector<numeric_tasks::NumericExplicitVariable> &variables, map<int,int> &map_operators);
        
        static numeric_tasks::NumericExplicitOperator create_numeric_operator(
                                                          const TaskProxy &parent_task_proxy, rd::ResourceDetection &resource_detection,
                                                                              const OperatorProxy &op, map<int,int> &map_variables, map<int,int> &map_numeric_variables, map<int,int> &map_numeric_max, vector<numeric_tasks::NumericExplicitComparison>& comps, vector<numeric_tasks::NumericExplicitAssignment> & ass, vector<numeric_tasks::NumericExplicitNumericVariable> &num_vars,
                                                                              std::map<double,int> &constant_values, std::vector<int> &initial_state, std::vector<numeric_tasks::NumericExplicitVariable> &variables,map<int,map<int,numeric_tasks::NumericExplicitComparison>> &min_comp, map<int,map<int,numeric_tasks::NumericExplicitComparison>> &max_comp);
        
        static int add_constant_if_necessary(double value, map<double,int> &map_constant_value, vector<numeric_tasks::NumericExplicitNumericVariable> &num_vars, rd::ResourceDetection &resource_detection,string &var_name, int id_op, int var);
        static NumericExplicitComparison & add_comparison_if_necessary(int var,int value, map<int,numeric_tasks::NumericExplicitComparison> &map_comparison, comp_operator comp, std::vector<numeric_tasks::NumericExplicitVariable> &variables,vector<int> &initial_state, vector<numeric_tasks::NumericExplicitComparison>& comps);
        

        static int get_unknown_value( const TaskProxy &parent_task_proxy, int var_id);
        static void create_forget_operators(
                                            const TaskProxy &parent_task_proxy,
                                            const vector<bool> &unknown_fact_needed,
                                            vector<tasks::ExplicitOperator> &operators);
        
        
    private:
        const rd::ResourceDetection resource_detection;
        set<int> variables_resource;
    };
    
}

#endif

