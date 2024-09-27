#ifndef TASK_PROXY_H
#define TASK_PROXY_H

#include "abstract_task.h"
#include "task_proxy.h"

#include "utils/hash.h"
#include "utils/system.h"

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "../utils/countdown_timer.h"

using namespace std;

namespace numeric_helper {
    
    /* An action is an operator where effects are espressed as add and eff of proposition.
     A proposition is an atom of the form Var = Val */
    
    struct Action {
        std::set<int> pre_list;
        std::set<int> num_list; // numeric preconditions
        std::set<int> add_list;
        std::set<int> del_list;
        std::vector<double> eff_list; // numeric effects
        std::set<int> possible_add_list; // numeric effects on conditions (id condition is already con + n_propositions)
        int cost = 1;
        Action(int size_eff) {
            eff_list = std::vector<double>(size_eff,0);
        }
        
        ~Action() = default;
    };
    
    /* Linear Numeric Conditions */
    
    struct LinearNumericCondition {
        LinearNumericCondition(std::vector<double> & c, double k) : coefficients(c), constant(k), is_strictly_greater(false) { }
        LinearNumericCondition(int size_coefficients) {
            coefficients.assign(size_coefficients,0);
            constant = 0;
            is_strictly_greater = false;
        }
        std::vector<double> coefficients;
        double constant;
        bool is_strictly_greater;
        LinearNumericCondition operator + (const LinearNumericCondition &lnc) const{
            std::vector<double> _coefficients(coefficients.size());
            double _constant;
            for (size_t num_id = 0; num_id < coefficients.size(); ++num_id){
                _coefficients[num_id] = coefficients[num_id] + lnc.coefficients[num_id];
            }
            _constant = constant + lnc.constant;
            return LinearNumericCondition(_coefficients,_constant);
        }
        
        LinearNumericCondition operator - (const LinearNumericCondition &lnc) const{
            std::vector<double> _coefficients(coefficients.size());
            double _constant;
            for (size_t num_id = 0; num_id < coefficients.size(); ++num_id){
                _coefficients[num_id] = coefficients[num_id] - lnc.coefficients[num_id];
            }
            _constant = constant - lnc.constant;
            return LinearNumericCondition(_coefficients,_constant);
        }
        
        bool simple_condition(int v){
            assert(v<coefficients.size());
            for (size_t c_id = 0; c_id < coefficients.size();++c_id){
                if (v != c_id && fabs(coefficients[c_id]) > 0.00001) return false;
            }
            return true;
        }
        
        bool dominate(LinearNumericCondition &other) const;
        
    };

    std::ostream& operator<<(std::ostream& os, const LinearNumericCondition& lnc);
    
    /* Linear Numeric Conditions */
    struct NumericVariable {
        int id_var;
        int id_abstract_task;
        double upper_bound;
        double lower_bound;
        NumericVariable(int id_, int id_at, double lb_, double ub_);
    };
    
    class LinearTaskProxy  : TaskProxy{
        const AbstractTask *task;
    public:
        explicit LinearTaskProxy(const AbstractTask &task)
        : task(&task) {}
        ~TaskProxy() = default;
        
        VariablesProxy get_variables() const {
            return VariablesProxy(*task);
        }
        
        NumericVariablesProxy get_numeric_variables() const {
            return NumericVariablesProxy(*task);
        }
        
        
        OperatorsProxy get_operators() const {
            return OperatorsProxy(*task);
        }
        
        AxiomsProxy get_axioms() const {
            return AxiomsProxy(*task);
        }
        
        ComparisonAxiomsProxy get_comparison_axioms() const {
            return ComparisonAxiomsProxy(*task);
        }
        
        AssignmentAxiomsProxy get_assignment_axioms() const {
            return AssignmentAxiomsProxy(*task);
        }
        
        GoalsProxy get_goals() const {
            return GoalsProxy(*task);
        }
        
        State get_initial_state() const {
            //        if(DEBUG) std::cout << "task proxy returning initial state " << std::endl;
            return State(*task, task->get_initial_state_values(), task->get_initial_state_numeric_values());
        }
        
        State convert_global_state(const GlobalState &global_state) const {
            //        if(DEBUG) std::cout << "task proxy returning converted global state " << std::endl;
            return State(*task, task->get_state_values(global_state), task->get_numeric_state_values(global_state));
        }
        
    };
    
    
    inline FactProxy::FactProxy(const AbstractTask &task, const Fact &fact)
    : task(&task), fact(fact) {
        assert(fact.var >= 0 && fact.var < task.get_num_variables());
        assert(fact.value >= 0 && fact.value < get_variable().get_domain_size());
    }
    
    inline FactProxy::FactProxy(const AbstractTask &task, int var_id, int value)
    : FactProxy(task, Fact(var_id, value)) {
    }
    
    inline NumAssProxy::NumAssProxy(const AbstractTask &task, int aff_var_id, f_operator op_type, int ass_var_id)
    : task(&task), aff_var_id(aff_var_id), op_type(op_type), ass_var_id(ass_var_id) {
        assert(aff_var_id >= 0 && aff_var_id < task.get_num_numeric_variables());
        assert(ass_var_id >= 0 && ass_var_id < task.get_num_numeric_variables());
    }
    
    inline VariableProxy FactProxy::get_variable() const {
        return VariableProxy(*task, fact.var);
    }
    
    inline NumericVariableProxy NumAssProxy::get_affected_variable() const {
        return NumericVariableProxy(*task, aff_var_id);
    }
    
    inline f_operator NumAssProxy::get_assigment_operator_type() const {
        return op_type;
    }
    
    inline NumericVariableProxy NumAssProxy::get_assigned_variable() const {
        return NumericVariableProxy(*task, ass_var_id);
    }
    
    inline bool does_fire(EffectProxy effect, const State &state) {
        for (FactProxy condition : effect.get_conditions()) {
            if (state[condition.get_variable()] != condition)
                return false;
        }
        return true;
    }
    
}
#endif

