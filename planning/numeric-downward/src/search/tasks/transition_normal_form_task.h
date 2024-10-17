#ifndef TASKS_TRANSITION_NORMAL_FORM_TASK_H
#define TASKS_TRANSITION_NORMAL_FORM_TASK_H

#include <algorithm>
#include <memory>
#include "explicit_task.h"
#include "../task_proxy.h"
#include "../plugin.h"

class AbstractTask;

using namespace std;
using namespace tasks;

namespace extra_tasks {
extern std::shared_ptr<AbstractTask> create_transition_normal_form_task(
    const std::shared_ptr<AbstractTask> &parent);
    
    /*
     Task transformation for TNF tasks.
     
     Since the constructor has to be called with the correct arguments,
     describing a task in TNF, we keep the class in the .cc file and
     provide a factory function in the header file for creating TNF tasks.
     */
    class TransitionNormalFormTask : public tasks::ExplicitTask {
    public:
        TransitionNormalFormTask(
                                 const shared_ptr<AbstractTask> &parent,
                                 vector<tasks::ExplicitVariable> &&variables,
                                 vector<vector<set<Fact>>> &&mutex_facts,
                                 vector<tasks::ExplicitOperator> &&operators,
                                 vector<int> &&initial_state_values,
                                 vector<Fact> &&goals)
        : ExplicitTask(
                       parent,
                       move(variables),
                       move(mutex_facts),
                       move(operators),
                       {},
                       move(initial_state_values),
                       move(goals)) {
            assert(variables.size() == goals.size());
            assert(all_of(
                          this->operators.begin(), this->operators.end(),
                          is_in_transition_normal_form));
        }
        TransitionNormalFormTask(
                                 const shared_ptr<AbstractTask> &parent,
                                 vector<tasks::ExplicitVariable> &&variables,
                                 vector<vector<set<Fact>>> &&mutex_facts,
                                 vector<tasks::ExplicitOperator> &&operators,
                                 vector<tasks::ExplicitOperator> &&axioms,
                                 vector<int> &&initial_state_values,
                                 vector<Fact> &&goals)
        : ExplicitTask(
                       parent,
                       move(variables),
                       move(mutex_facts),
                       move(operators),
                       move(axioms),
                       move(initial_state_values),
                       move(goals)) {
            assert(variables.size() == goals.size());
            assert(all_of(
                          this->operators.begin(), this->operators.end(),
                          is_in_transition_normal_form));
        }
        const GlobalOperator *get_global_operator(int, bool) const override {
            ABORT("TNF tasks don't support retrieving GlobalOperators");
        }
        static std::vector<tasks::ExplicitVariable> create_variables(
                                                                     const TaskProxy &parent_task_proxy,
                                                                     const std::vector<bool> &unknown_fact_needed);
        static vector<int> get_precondition_values(const OperatorProxy &op, int num_vars);
        static vector<int> get_effect_values(const OperatorProxy &op, int num_vars);
        static set<int> get_ordered_mentioned_variables(const OperatorProxy &op);
        static vector<vector<set<Fact>>> create_mutexes( const TaskProxy &parent_task_proxy,
                                                        const vector<bool> &unknown_fact_needed);
        static vector<Fact> create_goals(const TaskProxy &parent_task_proxy);
        static void create_normal_operators( const TaskProxy &parent_task_proxy,
                                            vector<bool> &unknown_fact_needed,
                                            vector<tasks::ExplicitOperator> &operators);
        static ExplicitOperator create_normal_operator(
                                                       const TaskProxy &parent_task_proxy,
                                                       vector<bool> &unknown_fact_needed,
                                                       const OperatorProxy &op);
        static ExplicitOperator create_forget_operator(
                                                              const FactProxy &fact, int post_value);
        static int get_unknown_value( const TaskProxy &parent_task_proxy, int var_id);
        static void create_forget_operators(
                                                               const TaskProxy &parent_task_proxy,
                                                               const vector<bool> &unknown_fact_needed,
                                                               vector<tasks::ExplicitOperator> &operators);

    #ifndef NDEBUG
        static bool is_in_transition_normal_form(const tasks::ExplicitOperator &op);
    #endif

        /*
         Since TNF tasks only *extend* variable domains, no changes are
         needed to convert a given state into a TNF state.
         */
        //    virtual void convert_state_values_from_parent(
        //        vector<int> &) const override {
        //    }
    };

}

#endif
