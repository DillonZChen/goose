#ifndef NUMERIC_TASKS_EXPLICIT_TASK_H
#define NUMERIC_TASKS_EXPLICIT_TASK_H

#include "../tasks/explicit_task.h"
#include "../tasks/delegating_task.h"

#include "../utils/collections.h"

#include <cassert>
#include <set>
#include <map>

namespace numeric_tasks {
    struct NumericExplicitEffect;
    struct NumericExplicitOperator;
    struct NumericExplicitVariable;
    struct NumericExplicitNumericVariable;
    struct NumericExplicitComparison;
    struct NumericExplicitAssignment;
    /*
     Task transformation that holds all task information itself.
     
     Instead of asking parent transformations for data and possibly
     converting it, this class holds all task data internally.
     
     When querying and transforming data from the parent(s) is more
     expensive than storing it in the task, this class can be used to
     "start over" in the task transformation hierarchy.
     
     While this class implements all methods for accessing task data, it
     doesn't provide implementations for the two state and operator
     conversion methods. This is to allow inheriting classes to decide how
     to make these conversions.
     */
    class NumericExplicitTask : public tasks::DelegatingTask {
        bool run_sanity_check() const;
        
    protected:
        const std::vector<NumericExplicitVariable> variables;
        const std::vector<NumericExplicitNumericVariable> num_variables;
        const std::vector<std::vector<std::set<Fact>>> mutexes;
        const std::vector<NumericExplicitOperator> operators;
        const std::vector<NumericExplicitOperator> axioms;
        const std::vector<NumericExplicitComparison> num_comp;
        const std::vector<NumericExplicitAssignment> num_effects;
        const std::vector<int> initial_state_values;
        std::vector<ap_float> initial_state_numeric_values;
        std::vector<ap_float> numeric_state;
        const std::vector<Fact> goals;
        
        const NumericExplicitVariable &get_variable(int var) const;
        const NumericExplicitEffect &get_effect(int op_id, int effect_id, bool is_axiom) const;
        const NumericExplicitOperator &get_operator_or_axiom(int index, bool is_axiom) const;
        
        std::map<int,int> operator_map;
        std::map<int,int> variables_map;
        std::map<int,int> num_variables_map;
    public:
        NumericExplicitTask(
                     const std::shared_ptr<AbstractTask> &parent,
                     std::vector<NumericExplicitVariable> &&variables,
                     std::vector<NumericExplicitNumericVariable> &&num_variables,
                     std::vector<NumericExplicitComparison> &&num_comp,
                     std::vector<NumericExplicitAssignment> &&num_effects,
                     std::vector<std::vector<std::set<Fact>>> &&mutexes,
                     std::vector<NumericExplicitOperator> &&operators,
                     std::vector<NumericExplicitOperator> &&axioms,
                     std::vector<int> &&initial_state_values,
                     std::vector<Fact> &&goals,
                     std::map<int,int> &&variables_map,
                     std::map<int,int> &&num_variables_map,
                     std::map<int,int> &&operator_map
                     );
        
        virtual int get_num_variables() const override;
        virtual const std::string &get_variable_name(int var) const override;
        virtual int get_variable_domain_size(int var) const override;
        //virtual int get_variable_axiom_layer(int var) const override;
        //virtual int get_variable_default_axiom_value(int var) const override;
        virtual const std::string &get_fact_name(const Fact &fact) const override;
        virtual bool are_facts_mutex(
                                     const Fact &fact1, const Fact &fact2) const override;
        
        virtual ap_float get_operator_cost(int index, bool is_axiom) const override;
        virtual const std::string &get_operator_name(int index, bool is_axiom) const override;
        
        virtual int get_num_operators() const override;
        virtual int get_num_operator_preconditions(
                                                   int index, bool is_axiom) const override;
        virtual Fact get_operator_precondition(
                                               int op_index, int fact_index, bool is_axiom) const override;
        virtual int get_num_operator_effects(
                                             int op_index, bool is_axiom) const override;
        virtual int get_num_operator_ass_effects(int op_index, bool is_axiom) const override;
        virtual int get_num_operator_ass_effect_conditions(
                                                           int op_index, int ass_eff_index, bool is_axiom) const override;
        virtual int get_num_operator_effect_conditions(
                                                       int op_index, int eff_index, bool is_axiom) const override;
        virtual Fact get_operator_effect_condition(
                                                   int op_index, int eff_index, int cond_index, bool is_axiom) const override;
        virtual Fact get_operator_effect(
                                         int op_index, int eff_index, bool is_axiom) const override;
        virtual AssEffect get_operator_ass_effect(int op_index, int eff_index, bool is_axiom) const override;
//        virtual const GlobalOperator *get_global_operator(
//                                                          int index, bool is_axiom) const override;
        
        virtual const GlobalOperator *get_global_operator(int index, bool is_axiom) const override {
            //ABORT("TNF tasks don't support retrieving GlobalOperators");
            std::map<int,int>::const_iterator new_index = operator_map.find(index);
            std::cout << index << " " << operator_map.size() << " " << new_index->first << " " <<  new_index-> second << std::endl;
            if (operator_map.find(index) == operator_map.end()) assert("operator not found");
            return parent->get_global_operator(new_index->second,is_axiom);
        }
        
        virtual int get_num_axioms() const override;
        
        virtual int get_num_goals() const override;
        virtual Fact get_goal_fact(int index) const override;
        
        virtual std::vector<int> get_initial_state_values() const override;
        //virtual void convert_state_values_from_parent(
        //    std::vector<int> &values) const override = 0;
        virtual std::vector<int> get_state_values(const GlobalState &global_state) const override;
        
        virtual std::vector<ap_float> get_numeric_state_values(const GlobalState &global_state) const override;
        
        virtual int get_num_cmp_axioms() const override;
        virtual int get_comparison_axiom_argument(int axiom_index, bool left) const override;
        virtual comp_operator get_comparison_axiom_operator(int axiom_index) const override;
        virtual Fact get_comparison_axiom_effect(int axiom_index, bool evaluation_result) const override;
        virtual int get_num_numeric_variables() const override;
        virtual const std::string &get_numeric_variable_name(int var) const override;
        virtual numType get_numeric_var_type(int index) const override;
        virtual std::vector<ap_float> get_initial_state_numeric_values() const override;


    };
    
    
    struct NumericExplicitVariable {
        const int domain_size;
        const std::string name;
        const std::vector<std::string> fact_names;
        const int axiom_layer;
        const int axiom_default_value;
        
        NumericExplicitVariable(
                         int domain_size,
                         std::string &&name,
                         std::vector<std::string> &&fact_names,
                         int axiom_layer,
                         int axiom_default_value);
    };
    
    struct NumericExplicitNumericVariable {
        const std::string name;
        const numType num_type;
        const double initial_value;
    
        NumericExplicitNumericVariable(
                                std::string &&name,
                                numType num_type,
                                double initial_value
                                );
    };
    
    struct NumericExplicitComparison {
        const int lhs;
        const int rhs;
        const comp_operator comp;
        const int id_var;
        
        NumericExplicitComparison(
                                       int lhs,
                                       int rhs,
                                       comp_operator comp,
                                       int id_var
                                       );
    };
    
    struct NumericExplicitAssignment {
        const int lhs;
        const int rhs;
        const f_operator op;
        NumericExplicitAssignment(
                                  int lhs,
                                  int rhs,
                                  f_operator op
                                );
    };
    
    struct NumericExplicitEffect {
        const Fact fact;
        const std::vector<Fact> conditions;
        
        NumericExplicitEffect(int var, int value, std::vector<Fact> &&conditions);
    };
    
    
    struct NumericExplicitOperator {
        const std::vector<Fact> preconditions;
        const std::vector<NumericExplicitEffect> effects;
        // add comparison and effects
        const std::vector<NumericExplicitComparison> num_preconditions;
        const std::vector<NumericExplicitAssignment> num_effects;
        
        const int cost;
        const std::string name;
        const bool is_an_axiom;
        const int copy_id;
        NumericExplicitOperator(
                         std::vector<Fact> &&preconditions,
                         std::vector<NumericExplicitEffect> &&effects,
                         int cost,
                         const std::string &name,
                         bool is_an_axiom,
                         int copy_id
                         );
        NumericExplicitOperator(
                                std::vector<Fact> &&preconditions,
                                std::vector<NumericExplicitEffect> &&effects,
                                std::vector<NumericExplicitComparison> &&num_preconditions,
                                std::vector<NumericExplicitAssignment> &&num_effects,
                                int cost,
                                const std::string &name,
                                bool is_an_axiom,
                                int copy_id
                                );
    };
    
    
    inline const NumericExplicitVariable &NumericExplicitTask::get_variable(int var) const {
        assert(utils::in_bounds(var, variables));
        return variables[var];
    }
    
    inline const NumericExplicitEffect &NumericExplicitTask::get_effect(
                                                          int op_id, int effect_id, bool is_axiom) const {
        const NumericExplicitOperator &op = get_operator_or_axiom(op_id, is_axiom);
        assert(utils::in_bounds(effect_id, op.effects));
        return op.effects[effect_id];
    }
    
    inline const NumericExplicitOperator &NumericExplicitTask::get_operator_or_axiom(
                                                                       int index, bool is_axiom) const {
        if (is_axiom) {
            assert(utils::in_bounds(index, axioms));
            return axioms[index];
        } else {
            assert(utils::in_bounds(index, operators));
            return operators[index];
        }
    }
}

#endif
