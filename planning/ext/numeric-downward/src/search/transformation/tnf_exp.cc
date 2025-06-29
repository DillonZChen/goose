#include "tnf_exp.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"
#include "../task_tools.h"

#include "../utils/collections.h"

#include <set>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace extra_tasks;



void TNFExpTask::create_operators(const TaskProxy &parent_task_proxy, vector<tasks::ExplicitOperator> &operators){
    // variables
   
    for (OperatorProxy op : parent_task_proxy.get_operators()) {
        int num_vars = parent_task_proxy.get_variables().size();
        vector<int> precondition_values = TransitionNormalFormTask::get_precondition_values(op, num_vars);
        vector<int> effect_values = TransitionNormalFormTask::get_effect_values(op, num_vars);
        
        vector<Fact> preconditions;
        vector<tasks::ExplicitEffect> effects;
        for (int var_id : TransitionNormalFormTask::get_ordered_mentioned_variables(op)) {
            int pre_value = precondition_values[var_id];
            if (pre_value != -1) {
                preconditions.emplace_back(var_id, pre_value);
            }
            int post_value = effect_values[var_id];
            if (post_value != -1) {
                effects.emplace_back(var_id, post_value, vector<Fact>());

            }

        }
        tasks::ExplicitOperator exp_op(
                                       move(preconditions),
                                       move(effects),
                                       op.get_cost(),
                                       op.get_name(),
                                       false, op.get_id());
        create_operator(parent_task_proxy, op, exp_op, operators);
    }
}

void TNFExpTask::create_axioms(const TaskProxy &parent_task_proxy, vector<tasks::ExplicitOperator> &axioms){
    
    // variables
    AxiomsProxy ax = parent_task_proxy.get_axioms();
    for (OperatorProxy op : ax){
        int num_vars = parent_task_proxy.get_variables().size();
        vector<int> precondition_values = TransitionNormalFormTask::get_precondition_values(op, num_vars);
        vector<int> effect_values = TransitionNormalFormTask::get_effect_values(op, num_vars);

        vector<Fact> preconditions;
        vector<tasks::ExplicitEffect> effects;
        for (int var_id : TransitionNormalFormTask::get_ordered_mentioned_variables(op)) {
            int pre_value = precondition_values[var_id];
            if (pre_value != -1) {
                preconditions.emplace_back(var_id, pre_value);
            }
            int post_value = effect_values[var_id];
            if (post_value != -1) {
                effects.emplace_back(var_id, post_value, vector<Fact>());

            }

        }
        tasks::ExplicitOperator exp_op(
                                       move(preconditions),
                                       move(effects),
                                       op.get_cost(),
                                       op.get_name(),
                                       true, op.get_id());
        axioms.push_back(exp_op);
    }
}


void TNFExpTask::create_operator(const TaskProxy &parent_task_proxy, const OperatorProxy &op, tasks::ExplicitOperator & exp_op,  vector<tasks::ExplicitOperator> &operators){
    // check if effects variable is also in precondition
    //cout << op.get_name() << " size " << exp_op.preconditions.size() << " \n";
    int num_vars = parent_task_proxy.get_variables().size();
    vector<int> precondition_values(num_vars,-1);
    
    vector<tasks::ExplicitEffect> effects = exp_op.effects;
    bool insert = true;
    for (Fact prec : exp_op.preconditions) {
        precondition_values[prec.var] = prec.value;
    }
    
//    for (int var_id : TransitionNormalFormTask::get_ordered_mentioned_variables(op)) {
//        cout << "\tpre " << var_id << " " <<   precondition_values[var_id] << " size "<< exp_op.preconditions.size() <<  endl;
//    }
    
    for (int var_id : TransitionNormalFormTask::get_ordered_mentioned_variables(op)) {
        int pre_value = precondition_values[var_id];
        if (pre_value == -1) {
            for (int val_id = 0; val_id < parent_task_proxy.get_variables()[var_id].get_domain_size(); ++val_id){
                vector<Fact> new_preconditions = exp_op.preconditions;
                vector<tasks::ExplicitEffect> new_effects = exp_op.effects;
                precondition_values[var_id] = val_id;
                bool add_pre = true;
                for (Fact & fact : new_preconditions){
                    if (fact.var == var_id) {
                        fact.value = val_id;
                        add_pre = false;
                    }
                }
                if (add_pre){
                    new_preconditions.emplace_back(var_id, val_id);
                }
//                for (Fact & fact : new_preconditions){
//                    cout << "\t\t" << fact.var << " " << fact.value << " size " << new_preconditions.size() << "\n";
//                }
                stringstream name;
                name << exp_op.name ;//<< "_" << var_id << "_"<< val_id;
                //cout << name.str() << " " << exp_op.copy_id << " - "
                 //    << exp_op.name << " " << op.get_name() << " " << op.get_id() << endl;
                tasks::ExplicitOperator new_exp_op(
                                                   move(new_preconditions),
                                                   move(new_effects),
                                                   op.get_cost(),
                                                   name.str(),
                                                   false, exp_op.copy_id);
                create_operator(parent_task_proxy, op, new_exp_op, operators);
            }
            insert = false;
        }
    }

    if (insert){
        //cout << "inserting" << endl;
        operators.push_back(exp_op);
    }
}

shared_ptr<AbstractTask> create_tnf_exp_task(const shared_ptr<AbstractTask> &parent) {
    // add operators
    TaskProxy parent_task_proxy(*parent);
    
    vector<bool> unknown_fact_needed(
                                     parent_task_proxy.get_variables().size(), false);
    vector<tasks::ExplicitVariable> variables = TransitionNormalFormTask::create_variables(parent_task_proxy,
                                                                                           unknown_fact_needed);
    vector<tasks::ExplicitOperator> operators;
    vector<tasks::ExplicitOperator> axioms;
    vector<Fact> goals;//= TransitionNormalFormTask::create_goals(parent_task_proxy);
    
    for (FactProxy goal : parent_task_proxy.get_goals()) {
        goals.emplace_back(goal.get_variable().get_id(),goal.get_value());
    }
    
    TNFExpTask::create_operators(parent_task_proxy,operators);
    TNFExpTask::create_axioms(parent_task_proxy,axioms);
    return make_shared<TNFExpTask>(
                                                 parent,
                                                 move(variables),
                                                 TransitionNormalFormTask::create_mutexes(parent_task_proxy, unknown_fact_needed),
                                                 move(operators),
                                                 move(axioms),
                                                 parent->get_initial_state_values(),
                                                 move(goals));
    
}

static shared_ptr<AbstractTask> _parse(OptionParser &parser) {
    //parser.document_language_support("conditional effects", "not supported");
    //parser.document_language_support("axioms", "not supported");
    
    parser.add_option<shared_ptr<AbstractTask>>(
                                                "transform",
                                                "Parent task transformation",
                                                "no_transform");
    
    Options opts = parser.parse();
    
    if (parser.dry_run())
        return nullptr;
    
    return create_tnf_exp_task(opts.get<shared_ptr<AbstractTask>>("transform"));
}

static PluginShared<AbstractTask> _plugin("tnf_exp", _parse);

