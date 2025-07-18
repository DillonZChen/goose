#include "transition_normal_form_task.h"

#include "explicit_task.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"
#include "../task_tools.h"

#include "../utils/collections.h"

#include <set>
#include <unordered_set>
#include <vector>

using namespace std;

namespace extra_tasks {
#ifndef NDEBUG
    bool TransitionNormalFormTask::is_in_transition_normal_form(const tasks::ExplicitOperator &op) {
        if (op.preconditions.size() != op.effects.size()) {
            return false;
        }
        unordered_set<int> precondition_vars;
        unordered_set<int> effect_vars;
        for (const Fact &fact : op.preconditions) {
            precondition_vars.insert(fact.var);
        }
        for (const tasks::ExplicitEffect &effect : op.effects) {
            effect_vars.insert(effect.fact.var);
        }
        return precondition_vars == effect_vars;
    }
#endif
    
    
    int TransitionNormalFormTask::get_unknown_value(
                                                    const TaskProxy &parent_task_proxy, int var_id) {
        return parent_task_proxy.get_variables()[var_id].get_domain_size();
    }
    
    set<int> TransitionNormalFormTask::get_ordered_mentioned_variables(const OperatorProxy &op) {
        set<int> vars;
        for (FactProxy precondition : op.get_preconditions()) {
            vars.insert(precondition.get_variable().get_id());
        }
        for (EffectProxy effect : op.get_effects()) {
            vars.insert(effect.get_fact().get_variable().get_id());
        }
        return vars;
    }
    
    vector<int> TransitionNormalFormTask::get_precondition_values(const OperatorProxy &op, int num_vars) {
        vector<int> precondition_values(num_vars, -1);
        for (FactProxy precondition : op.get_preconditions()) {
            //const Fact fact = precondition.get_pair();
            precondition_values[precondition.get_variable().get_id()] = precondition.get_value();
        }
        return precondition_values;
    }
    
    vector<int> TransitionNormalFormTask::get_effect_values(const OperatorProxy &op, int num_vars) {
        vector<int> effect_values(num_vars, -1);
        for (EffectProxy effect : op.get_effects()) {
            const FactProxy fact = effect.get_fact();
            effect_values[fact.get_variable().get_id()] = fact.get_value();
        }
        return effect_values;
    }
    
    /*
     TODO: This function runs in time O(|facts|^2). We could think about a
     faster way of passing mutex information between tasks (see issue661).
     */
    vector<vector<set<Fact>>> TransitionNormalFormTask::create_mutexes(
                                                                       const TaskProxy &parent_task_proxy,
                                                                       const vector<bool> &unknown_fact_needed) {
        VariablesProxy parent_variables = parent_task_proxy.get_variables();
        
        // Initialize structure.
        vector<vector<set<Fact>>> mutexes(parent_variables.size());
        for (VariableProxy var : parent_variables) {
            int tnf_domain_size = var.get_domain_size();
            if (unknown_fact_needed[var.get_id()])
                ++tnf_domain_size;
            mutexes[var.get_id()].resize(tnf_domain_size);
        }
        
        // Fill structure.
        FactsProxy facts = parent_variables.get_facts();
        for (FactProxy fact1_proxy : facts) {
            for (FactProxy fact2_proxy : facts) {
                if (fact1_proxy.is_mutex(fact2_proxy)) {
                    mutexes[fact1_proxy.get_variable().get_id()][fact1_proxy.get_value()].insert(Fact(fact2_proxy.get_variable().get_id(),fact2_proxy.get_value()));
                }
            }
        }
        return mutexes;
    }
    
    vector<tasks::ExplicitVariable> TransitionNormalFormTask::create_variables(
                                                                               const TaskProxy &parent_task_proxy,
                                                                               const vector<bool> &unknown_fact_needed) {
        vector<tasks::ExplicitVariable> variables;
        variables.reserve(parent_task_proxy.get_variables().size());
        for (VariableProxy var : parent_task_proxy.get_variables()) {
            int var_id = var.get_id();
            int parent_domain_size = var.get_domain_size();
            int tnf_domain_size = parent_domain_size;
            if (unknown_fact_needed[var_id]) {
                ++tnf_domain_size;
            }
            string var_name = var.get_name();
            
            vector<string> fact_names;
            fact_names.reserve(tnf_domain_size);
            for (int value = 0; value < parent_domain_size; ++value) {
                FactProxy fact = var.get_fact(value);
                fact_names.push_back(fact.get_name());
            }
            if (unknown_fact_needed[var_id]) {
                fact_names.push_back(var_name + " " + "unknown");
            }
            
            variables.emplace_back(
                                   tnf_domain_size, move(var_name), move(fact_names), -1, -1);
        }
        return variables;
    }
    
    tasks::ExplicitOperator TransitionNormalFormTask::create_normal_operator(
                                                                             const TaskProxy &parent_task_proxy,
                                                                             vector<bool> &unknown_fact_needed,
                                                                             const OperatorProxy &op) {
        int num_vars = parent_task_proxy.get_variables().size();
        vector<int> precondition_values = TransitionNormalFormTask::get_precondition_values(op, num_vars);
        vector<int> effect_values = TransitionNormalFormTask::get_effect_values(op, num_vars);
        
        vector<Fact> preconditions;
        vector<tasks::ExplicitEffect> effects;
        for (int var_id : TransitionNormalFormTask::get_ordered_mentioned_variables(op)) {
            int pre_value = precondition_values[var_id];
            if (pre_value == -1) {
                unknown_fact_needed[var_id] = true;
                pre_value = get_unknown_value(parent_task_proxy, var_id);
            }
            int post_value = effect_values[var_id];
            if (post_value == -1) {
                assert(precondition_values[var_id] != -1);
                post_value = pre_value;
            }
            preconditions.emplace_back(var_id, pre_value);
            effects.emplace_back(var_id, post_value, vector<Fact>());
        }
        return tasks::ExplicitOperator(
                                       move(preconditions),
                                       move(effects),
                                       op.get_cost(),
                                       op.get_name(),
                                       false, op.get_id());
    }
    
    void TransitionNormalFormTask::create_normal_operators(
                                                           const TaskProxy &parent_task_proxy,
                                                           vector<bool> &unknown_fact_needed,
                                                           vector<tasks::ExplicitOperator> &operators) {
        for (OperatorProxy op : parent_task_proxy.get_operators()) {
            operators.push_back(
                                create_normal_operator(parent_task_proxy, unknown_fact_needed, op));
        }
    }
    
    tasks::ExplicitOperator TransitionNormalFormTask::create_forget_operator(
                                                          const FactProxy &fact, int post_value) {
        int var_id = fact.get_variable().get_id();
        return tasks::ExplicitOperator(
                                       {Fact(var_id,fact.get_value())},
                                       {tasks::ExplicitEffect(var_id, post_value, {})},
                                       0,
                                       "forget " + fact.get_name(),
                                       false, -1);
    }
    
    void TransitionNormalFormTask::create_forget_operators(
                                        const TaskProxy &parent_task_proxy,
                                        const vector<bool> &unknown_fact_needed,
                                        vector<tasks::ExplicitOperator> &operators) {
        for (VariableProxy var : parent_task_proxy.get_variables()) {
            int var_id = var.get_id();
            if (unknown_fact_needed[var.get_id()]) {
                int post_value = TransitionNormalFormTask::get_unknown_value(parent_task_proxy, var_id);
                for (int value = 0; value < var.get_domain_size(); ++value) {
                    FactProxy fact = var.get_fact(value);
                    operators.push_back(create_forget_operator(fact, post_value));
                }
            }
        }
    }
    
    // Create fully defined goal state.
    vector<Fact> TransitionNormalFormTask::create_goals(const TaskProxy &parent_task_proxy) {
        VariablesProxy variables = parent_task_proxy.get_variables();
        vector<Fact> goals;
        goals.reserve(variables.size());
        for (VariableProxy var : variables) {
            int var_id = var.get_id();
            goals.emplace_back(var_id, get_unknown_value(parent_task_proxy, var_id));
        }
        for (FactProxy goal : parent_task_proxy.get_goals()) {
            goals[goal.get_variable().get_id()] = Fact(goal.get_variable().get_id(),goal.get_value());
        }
        return goals;
    }
    
    shared_ptr<AbstractTask> create_transition_normal_form_task(
                                                                const shared_ptr<AbstractTask> &parent) {
        TaskProxy parent_task_proxy(*parent);
        //verify_no_axioms(parent_task_proxy);
        verify_no_conditional_effects(parent_task_proxy);
        
        /*
         We add an "unknown" fact for variables occuring in effects, but
         not in preconditions, and variables missing from the goal
         description.
         */
        vector<bool> unknown_fact_needed(
                                         parent_task_proxy.get_variables().size(), false);
        
        /*
         Compute TNF versions of normal operators and record which
         variables need "unknown" fact.
         */
        vector<tasks::ExplicitOperator> operators;
        TransitionNormalFormTask::create_normal_operators(parent_task_proxy, unknown_fact_needed, operators);
        
        // Create TNF goals.
        vector<Fact> goals = TransitionNormalFormTask::create_goals(parent_task_proxy);
        
        // Variables missing in goal description need an "unknown" fact.
        for (const Fact &goal : goals) {
            if (goal.value == TransitionNormalFormTask::get_unknown_value(parent_task_proxy, goal.var)) {
                unknown_fact_needed[goal.var] = true;
            }
        }
        
        // Create "forget" operators for the variables with "unknown" facts.
        TransitionNormalFormTask::create_forget_operators(parent_task_proxy, unknown_fact_needed, operators);
        
        return make_shared<TransitionNormalFormTask>(
                                                     parent,
                                                     TransitionNormalFormTask::create_variables(parent_task_proxy, unknown_fact_needed),
                                                     TransitionNormalFormTask::create_mutexes(parent_task_proxy, unknown_fact_needed),
                                                     move(operators),
                                                     parent->get_initial_state_values(),
                                                     move(goals));
    }
    
    static shared_ptr<AbstractTask> _parse(OptionParser &parser) {
        parser.document_language_support("conditional effects", "not supported");
        parser.document_language_support("axioms", "not supported");
        
        parser.add_option<shared_ptr<AbstractTask>>(
                                                    "transform",
                                                    "Parent task transformation",
                                                    "no_transform");
        
        Options opts = parser.parse();
        
        if (parser.dry_run())
            return nullptr;
        
        return create_transition_normal_form_task(
                                                  opts.get<shared_ptr<AbstractTask>>("transform"));
    }
    
    static PluginShared<AbstractTask> _plugin("tnf", _parse);
}

