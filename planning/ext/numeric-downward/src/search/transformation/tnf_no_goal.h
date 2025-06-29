#ifndef TASKS_TNF_NO_GOAL_TASK_H
#define TASKS_TNF_NO_GOAL_TASK_H

#include <algorithm>
#include <memory>
#include "../tasks/transition_normal_form_task.h"
#include "../tasks/explicit_task.h"
#include "../task_proxy.h"
#include "../plugin.h"

class AbstractTask;

using namespace std;

namespace extra_tasks {
extern std::shared_ptr<AbstractTask> create_transition_normal_form_task(
    const std::shared_ptr<AbstractTask> &parent);
    
    /*
     Task transformation for TNF tasks.
     
     Since the constructor has to be called with the correct arguments,
     describing a task in TNF, we keep the class in the .cc file and
     provide a factory function in the header file for creating TNF tasks.
     */
    class TransitionNormalFormTaskNoGoal : public TransitionNormalFormTask {
    public:
        TransitionNormalFormTaskNoGoal(
                                 const shared_ptr<AbstractTask> &parent,
                                 vector<tasks::ExplicitVariable> &&variables,
                                 vector<vector<set<Fact>>> &&mutex_facts,
                                 vector<tasks::ExplicitOperator> &&operators,
                                 vector<int> &&initial_state_values,
                                 vector<Fact> &&goals)
        : TransitionNormalFormTask(
                       parent,
                       move(variables),
                       move(mutex_facts),
                       move(operators),
                       move(initial_state_values),
                       move(goals)) {
            assert(variables.size() == goals.size());
            assert(all_of(
                          this->operators.begin(), this->operators.end(),
                          is_in_transition_normal_form));
        }
        TransitionNormalFormTaskNoGoal(
                                       const shared_ptr<AbstractTask> &parent,
                                       vector<tasks::ExplicitVariable> &&variables,
                                       vector<vector<set<Fact>>> &&mutex_facts,
                                       vector<tasks::ExplicitOperator> &&operators,
                                       vector<tasks::ExplicitOperator> &&axioms,
                                       vector<int> &&initial_state_values,
                                       vector<Fact> &&goals)
        : TransitionNormalFormTask(
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

        static vector<vector<set<Fact>>> create_mutexes( const TaskProxy &parent_task_proxy,
                                                        const vector<bool> &unknown_fact_needed);
        static vector<Fact> create_goals(const TaskProxy &parent_task_proxy);

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
