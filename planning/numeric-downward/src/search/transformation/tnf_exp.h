#ifndef TASKS_TNF_EXP_TASK_H
#define TASKS_TNF_EXP_TASK_H

#include "../utils/collections.h"
#include "../tasks/transition_normal_form_task.h"
#include "../tasks/explicit_task.h"
#include "../plugin.h"
#include "../task_proxy.h"
#include "../task_tools.h"

#include <cassert>
#include <set>
#include <vector>

#include <memory>

class AbstractTask;

namespace extra_tasks {
    
    
    //static std::shared_ptr<AbstractTask> create_tnf_exp_task(const std::shared_ptr<AbstractTask> &parent);
    class TNFExpTask : public tasks::ExplicitTask {
    public:
        TNFExpTask(
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
            
        }
        TNFExpTask(
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
            
        }
        const GlobalOperator *get_global_operator(int, bool) const override {
            ABORT("TNF tasks don't support retrieving GlobalOperators");
        }
        static void create_operators(const TaskProxy &parent_task_proxy, vector<tasks::ExplicitOperator> &operators);
        static void create_axioms(const TaskProxy &parent_task_proxy, vector<tasks::ExplicitOperator> &operators);
    private:
        static void create_operator(const TaskProxy &parent_task_proxy, const OperatorProxy &op, tasks::ExplicitOperator & exp_op,  vector<tasks::ExplicitOperator> &operators);
    };
};

#endif
