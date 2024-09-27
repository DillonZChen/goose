#include "tnf_no_goal.h"

#include "../tasks/explicit_task.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"
#include "../task_tools.h"

#include "../utils/collections.h"
#include "tnf_exp.h"

#include <set>
#include <unordered_set>
#include <vector>

using namespace std;

namespace extra_tasks {
    
    
    /*
     TODO: This function runs in time O(|facts|^2). We could think about a
     faster way of passing mutex information between tasks (see issue661).
     */
    vector<vector<set<Fact>>> TransitionNormalFormTaskNoGoal::create_mutexes(
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
    
    
    // Create fully defined goal state.
    vector<Fact> TransitionNormalFormTaskNoGoal::create_goals(const TaskProxy &parent_task_proxy) {
        vector<Fact> goals;

        for (FactProxy goal : parent_task_proxy.get_goals()) {
            goals.push_back(Fact(goal.get_variable().get_id(),goal.get_value()));
        }
        return goals;
    }
    
    shared_ptr<AbstractTask> create_tnf_no_goal_task(
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
        
        vector<tasks::ExplicitOperator> axioms;
        TNFExpTask::create_axioms(parent_task_proxy,axioms);
        
        // Create TNF goals.
        vector<Fact> goals = TransitionNormalFormTaskNoGoal::create_goals(parent_task_proxy);
        
        // Variables missing in goal description need an "unknown" fact.
        for (const Fact &goal : goals) {
            if (goal.value == TransitionNormalFormTask::get_unknown_value(parent_task_proxy, goal.var)) {
                unknown_fact_needed[goal.var] = true;
            }
        }
        
        // Create "forget" operators for the variables with "unknown" facts.
        TransitionNormalFormTask::create_forget_operators(parent_task_proxy, unknown_fact_needed, operators);
        
        return make_shared<TransitionNormalFormTaskNoGoal>(
                                                           parent,
                                                           TransitionNormalFormTaskNoGoal::create_variables(parent_task_proxy, unknown_fact_needed),
                                                           TransitionNormalFormTaskNoGoal::create_mutexes(parent_task_proxy, unknown_fact_needed),
                                                           move(operators),
                                                           move(axioms),
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
        
        return create_tnf_no_goal_task(
                                       opts.get<shared_ptr<AbstractTask>>("transform"));
    }
    
    static PluginShared<AbstractTask> _plugin("tnf_ng", _parse);
}

