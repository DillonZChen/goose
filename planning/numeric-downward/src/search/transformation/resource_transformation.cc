#include "resource_transformation.h"

#include "../tasks/explicit_task.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"
#include "../task_tools.h"

#include "../utils/collections.h"

#include <set>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace rd;

namespace extra_tasks {

    
    
    int ResourceTask::get_unknown_value(
                                                    const TaskProxy &parent_task_proxy, int var_id) {
        return parent_task_proxy.get_variables()[var_id].get_domain_size();
    }
    
    set<int> ResourceTask::get_ordered_mentioned_variables(const OperatorProxy &op) {
        set<int> vars;
        for (FactProxy precondition : op.get_preconditions()) {
            vars.insert(precondition.get_variable().get_id());
        }
        for (EffectProxy effect : op.get_effects()) {
            vars.insert(effect.get_fact().get_variable().get_id());
        }
        return vars;
    }
    
    vector<int> ResourceTask::get_precondition_values(const OperatorProxy &op, int num_vars) {
        vector<int> precondition_values(num_vars, -1);
        for (FactProxy precondition : op.get_preconditions()) {
            //const Fact fact = precondition.get_pair();
            precondition_values[precondition.get_variable().get_id()] = precondition.get_value();
        }
        return precondition_values;
    }
    
    vector<int> ResourceTask::get_effect_values(const OperatorProxy &op, int num_vars) {
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
    vector<vector<set<Fact>>> ResourceTask::create_mutexes(
                                                                       const TaskProxy &parent_task_proxy) {
        VariablesProxy parent_variables = parent_task_proxy.get_variables();
        
        // Initialize structure.
        vector<vector<set<Fact>>> mutexes(parent_variables.size());
        for (VariableProxy var : parent_variables) {
            int tnf_domain_size = var.get_domain_size();
        
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
    
    vector<numeric_tasks::NumericExplicitVariable> ResourceTask::create_variables(
                                                                               const TaskProxy &parent_task_proxy,
                                                                                rd::ResourceDetection &resource_detection) {
        vector<numeric_tasks::NumericExplicitVariable> variables;
        set<int> &needed_variables = resource_detection.variables_unchanged();
        variables.reserve(needed_variables.size());
        for (int var_id : needed_variables){
        //for (VariableProxy var : parent_task_proxy.get_variables()) {
            VariableProxy var = parent_task_proxy.get_variables()[var_id];
            //int var_id = var.get_id();
            //if (needed_variables.find(var_id) == needed_variables.end()) continue;
            int parent_domain_size = var.get_domain_size();
            
            string var_name = var.get_name();
            
            vector<string> fact_names;
            fact_names.reserve(parent_domain_size);
            for (int value = 0; value < parent_domain_size; ++value) {
                FactProxy fact = var.get_fact(value);
                fact_names.push_back(fact.get_name());
            }
            
            
            variables.emplace_back(
                                   parent_domain_size, move(var_name), move(fact_names), -1, -1);
        }
        
        return variables;
    }
    
    vector<numeric_tasks::NumericExplicitNumericVariable> ResourceTask::create_numeric_variables( const TaskProxy &parent_task_proxy,
                                                                                                 rd::ResourceDetection &resource_detection, map<int,int> &map_num_vars, std::map<int,int> &map_num_max) {
        vector<numeric_tasks::NumericExplicitNumericVariable> variables;
        set<int> &resource_variables = resource_detection.variables_resource();
        variables.reserve(resource_variables.size() * 2  + 1);
        int n_var = 1;
        variables.emplace_back(move("zero"),numType::constant,0);
        for (int id_var : resource_variables){
            VariableProxy var= parent_task_proxy.get_variables()[id_var];
            int value = parent_task_proxy.get_initial_state()[id_var].get_value() ;
            string var_name = var.get_name();
            variables.emplace_back(move(var_name),numType::regular,resource_detection.get_mu(id_var,value));
            map_num_vars[id_var] = n_var++;
            
            string var_name_max = var.get_name() + "_max";
            variables.emplace_back(move(var_name_max),numType::constant,resource_detection.get_max(id_var));
            map_num_max[id_var] = n_var++;

        }
        return variables;
    }
    
    numeric_tasks::NumericExplicitOperator ResourceTask::create_unchanged_operator(
                                                                             const TaskProxy &parent_task_proxy,
                                                                             const OperatorProxy &op, map<int,int> &map_variables) {
        int num_vars = parent_task_proxy.get_variables().size();
        vector<int> precondition_values = ResourceTask::get_precondition_values(op, num_vars);
        vector<int> effect_values = ResourceTask::get_effect_values(op, num_vars);
        
        vector<Fact> preconditions;
        vector<numeric_tasks::NumericExplicitEffect> effects;
        for (FactProxy precondition : op.get_preconditions()) {
            if (map_variables.find(precondition.get_variable().get_id()) != map_variables.end()){ preconditions.emplace_back(map_variables[precondition.get_variable().get_id()],precondition.get_value());
            } else {
                cout << op.get_name() << " has a numeric precondition" << endl;
                cout <<precondition.get_variable().get_id() << " " << map_variables[precondition.get_variable().get_id()] << endl;
            }
            
        }
        for (EffectProxy effect : op.get_effects()) {
            effects.emplace_back(map_variables[effect.get_fact().get_variable().get_id()],effect.get_fact().get_value(),vector<Fact>());
        }
        return numeric_tasks::NumericExplicitOperator(
                                       move(preconditions),
                                       move(effects),
                                       op.get_cost(),
                                       op.get_name(),
                                       false, op.get_id());
    }
    
    int ResourceTask::add_constant_if_necessary(double value, map<double,int> &map_constant_value, vector<numeric_tasks::NumericExplicitNumericVariable> &num_vars, rd::ResourceDetection &resource_detection,string &var_name, int id_op, int var){
        int id_delta = -1;
        map<double,int>::iterator it = map_constant_value.find((double)(int)value);
        if ( it == map_constant_value.end()){
            NumericExplicitNumericVariable sum(move(var_name),numType::constant,value);
            num_vars.push_back(sum);
            map_constant_value[value] = num_vars.size() - 1;
            id_delta = num_vars.size() - 1;
        } else {
            id_delta = it->second;
        }
        return id_delta;
    }
    
    NumericExplicitComparison & ResourceTask::add_comparison_if_necessary(int var,int value, map<int,NumericExplicitComparison> &map_comparison, comp_operator comp, std::vector<numeric_tasks::NumericExplicitVariable> &variables,vector<int> &initial_state,vector<numeric_tasks::NumericExplicitComparison>& comps){
        map<int,NumericExplicitComparison>::iterator it = map_comparison.find(value);
        if ( it == map_comparison.end()){
            // add new variable and new comparator
            //cout << "adding new variable " << var << " " << value << " comp " << comp << " " << variables.size() << endl;
            vector<string> fact_names(2,"");
            variables.push_back(NumericExplicitVariable(2, "var", move(fact_names), -1, -1));
            initial_state.push_back(1);
            // add comparison
            NumericExplicitComparison comparison(var,value,comp,variables.size()-1);
            comps.push_back(comparison);
            map_comparison.insert({value,comparison});
        } else {
            return it->second;
        }
        return map_comparison.find(value)->second;
    }
    numeric_tasks::NumericExplicitOperator ResourceTask::create_numeric_operator(
                                                                    const TaskProxy &parent_task_proxy,
                                                                  ResourceDetection &resource_detection,
                                                                    const OperatorProxy &op, map<int,int> &map_variables, map<int,int> &map_numeric_variables, map<int,int> &map_numeric_max ,vector<numeric_tasks::NumericExplicitComparison>& comps, vector<numeric_tasks::NumericExplicitAssignment> & ass, vector<numeric_tasks::NumericExplicitNumericVariable> &exp_num_vars,
                                                                                 map<double,int> &constant_values,
                                                                                 std::vector<int> &initial_state, std::vector<numeric_tasks::NumericExplicitVariable> &variables,map<int,map<int,numeric_tasks::NumericExplicitComparison>> &min_comp, map<int,map<int,numeric_tasks::NumericExplicitComparison>> &max_comp) {
        //cout << " -- " << op.get_name() << " operator numeric " <<  op.get_id() << endl;
        int num_vars = parent_task_proxy.get_variables().size();
        vector<int> precondition_values = ResourceTask::get_precondition_values(op, num_vars);
        vector<int> effect_values = ResourceTask::get_effect_values(op, num_vars);
        vector<Fact> preconditions;
        vector<numeric_tasks::NumericExplicitComparison> num_prec;
        vector<numeric_tasks::NumericExplicitAssignment> num_eff;
        vector<numeric_tasks::NumericExplicitEffect> effects;
//        cout << "\tmap numeric variables" << endl;
//        for (auto m : map_numeric_variables){
//            cout << "\t" << m.first << " " << m.second << endl;
//        }
        int id_delta = -1;
        for (FactProxy precondition : op.get_preconditions()) {
            int var = precondition.get_variable().get_id();
            if (resource_detection.is_propositional_variable(var)){
                preconditions.emplace_back(map_variables[var],precondition.get_value());
               // if (map_variables.find(var)==map_variables.end());
            } else{
              // add here numeric preconditions
                //preconditions.emplace_back(map_variables[var],1);
                //cout << "adding action " << op.get_name() << endl;
                VariableProxy var_proxy= parent_task_proxy.get_variables()[var];
                string var_name = var_proxy.get_name();
                
                double value = abs(resource_detection.get_delta(op.get_id(),var));
                
                id_delta = add_constant_if_necessary(value,constant_values,exp_num_vars,resource_detection,var_name,op.get_id(),var);
               
                if (resource_detection.get_action_behaviour(op.get_id(),var) == ActionTypeRD::CONSUMER){
                    // create new preconditions
                    //cout << "\tconsumer " << var << le << exp_num_vars[map_numeric_max[var]].initial_value << endl;
                    NumericExplicitComparison &less = add_comparison_if_necessary(map_numeric_variables[var],map_numeric_max[var],max_comp[map_numeric_variables[var]],le,variables,initial_state,comps);
                    preconditions.emplace_back(less.id_var,1);
                    num_prec.push_back(less);
                    
                    // create new preconditions
                    //cout << "\tconsumer " <<var << ge << exp_num_vars[id_delta].initial_value << endl;

                    NumericExplicitComparison &greater = add_comparison_if_necessary(map_numeric_variables[var],id_delta,min_comp[map_numeric_variables[var]],ge,variables,initial_state,comps); ;//(map_numeric_variables[var],id_delta,ge,variables.size()-1);
                    preconditions.emplace_back(greater.id_var,1);
                    num_prec.push_back(greater);
                    
                } else if (resource_detection.get_action_behaviour(op.get_id(),var) == ActionTypeRD::PRODUCER){
                    
                    VariableProxy var_proxy= parent_task_proxy.get_variables()[var];
                    string var_name = var_proxy.get_name();
                    double value = resource_detection.get_max(var) - resource_detection.get_delta(op.get_id(),var);
                    int id_num = add_constant_if_necessary(value,constant_values,exp_num_vars,resource_detection,var_name,op.get_id(),var);
                    
                    //cout << "\tproducer " <<var << le << exp_num_vars[map_numeric_max[var]].initial_value << endl;
                    NumericExplicitComparison &less = add_comparison_if_necessary(map_numeric_variables[var],id_num,max_comp[map_numeric_variables[var]],le,variables,initial_state,comps);// (map_numeric_variables[var],id_num ,le,variables.size()-2);
                    preconditions.emplace_back(less.id_var,1);
                    num_prec.push_back(less);
                    
                    // create new preconditions
                    //cout << "\tproducer " << var << ge << exp_num_vars[0].initial_value << endl;
                    NumericExplicitComparison &greater = add_comparison_if_necessary(map_numeric_variables[var],0,min_comp[map_numeric_variables[var]],ge,variables,initial_state,comps);
                    preconditions.emplace_back(greater.id_var,1);
                    num_prec.push_back(greater);
                } else{
                    assert(false);
                }
            }
        }
        for (EffectProxy effect : op.get_effects()) {
            int var = effect.get_fact().get_variable().get_id();
            if (resource_detection.is_propositional_variable(var)){
                effects.emplace_back(map_variables[var],effect.get_fact().get_value(),vector<Fact>());
            } else {
                if (id_delta == -1) assert(false);
                //cout << "\t\tnumeric variable " << var << endl;
                if (resource_detection.get_action_behaviour(op.get_id(),var) == ActionTypeRD::CONSUMER){
//                    // create new effect
                    NumericExplicitAssignment eff(map_numeric_variables[var],id_delta,decrease);
                    ass.push_back(eff);
                    num_eff.push_back(eff);
                    
                } else if (resource_detection.get_action_behaviour(op.get_id(),var) == ActionTypeRD::PRODUCER){
                    NumericExplicitAssignment eff(map_numeric_variables[var],id_delta,increase);
                    ass.push_back(eff);
                    num_eff.push_back(eff);
                } else{
                    assert(false);
                }
            }
        }
        //cout << "num effect size " << num_eff.size() << endl;
        return numeric_tasks::NumericExplicitOperator(
                                       move(preconditions),
                                       move(effects),
                                       move(num_prec),
                                       move(num_eff),
                                       op.get_cost(),
                                       op.get_name(),
                                       false, op.get_id());
    }
    
    void ResourceTask::create_unchanged_operators( const TaskProxy &parent_task_proxy,
                                                  set<int> &id_op,
                                                  vector<numeric_tasks::NumericExplicitOperator> &operators, map<int,int> &map_variables,  map<int,int> &map_operators) {
//        for (OperatorProxy op : parent_task_proxy.get_operators()) {
            for (int op : id_op) {
            operators.push_back(
                                ResourceTask::create_unchanged_operator(parent_task_proxy, parent_task_proxy.get_operators()[op],map_variables));
                map_operators[operators.size() - 1] = op;
        }
    }
    
    void ResourceTask::create_numeric_operators( TaskProxy &parent_task_proxy,
                                                rd::ResourceDetection &resource_detection,                                     vector<numeric_tasks::NumericExplicitOperator> &operators, map<int,int> &map_variables, map<int,int> &map_numeric_variables,  map<int,int> &map_numeric_max, vector<numeric_tasks::NumericExplicitComparison>& comps, vector<numeric_tasks::NumericExplicitAssignment> & ass, vector<numeric_tasks::NumericExplicitNumericVariable> &num_vars,std::vector<int> &initial_state, std::vector<numeric_tasks::NumericExplicitVariable> &variables,  map<int,int> &map_operators) {
        std::map<double, int> constant_values;
        map<int,map<int,numeric_tasks::NumericExplicitComparison>> min_comp;
        map<int,map<int,numeric_tasks::NumericExplicitComparison>> max_comp;
        for (int id_var : resource_detection.variables_resource()){

            list<set<int>> ea = resource_detection.get_equivalent_actions(parent_task_proxy,id_var);
            //cout << id_var << " " << endl;
            for (set<int> & group : ea){
                if (group.empty()) continue;
                int id_op = *group.begin();
                OperatorProxy op = parent_task_proxy.get_operators()[id_op];
                operators.push_back(
                                    ResourceTask::create_numeric_operator(parent_task_proxy, resource_detection, op, map_variables,map_numeric_variables,map_numeric_max,comps,ass,num_vars,constant_values,initial_state,variables,min_comp,max_comp));
                map_operators[operators.size() - 1] = id_op;
            }
        }
    }
    
    // Create initial state
    vector<int> ResourceTask::create_initial_state(const TaskProxy &parent_task_proxy, ResourceDetection &resource_detection, map<int,int>& map_vars) {
        set<int> &needed_variables = resource_detection.variables_unchanged();
        vector<int> initial_state(needed_variables.size());
        int id = 0;
        for (int id_var : needed_variables){
            initial_state[id] = parent_task_proxy.get_initial_state()[id_var].get_value();
            map_vars[id_var] = id;
            id++;
        }
        return initial_state;
    }
    
    // Create fully defined goal state.
    vector<Fact> ResourceTask::create_goals(const TaskProxy &parent_task_proxy, map<int,int> & map_variables) {
        vector<Fact> goals(parent_task_proxy.get_goals().size(),Fact(-1,-1));
        int n_goal = 0;
        for (FactProxy goal : parent_task_proxy.get_goals()) {
            int id_var = map_variables[goal.get_variable().get_id()];
            goals[n_goal] = Fact(id_var,goal.get_value());
            n_goal++;
        }
        return goals;
    }
    
    shared_ptr<AbstractTask> create_resource_task(
                                                                const shared_ptr<AbstractTask> &parent, bool single_action) {
        
        
        TaskProxy parent_task_proxy(*parent);
        //verify_no_axioms(parent_task_proxy);
        verify_no_conditional_effects(parent_task_proxy);
        ResourceDetection resource_detection(parent_task_proxy,single_action);
        map<int,int> map_variables;
        map<int,int> map_num_variables;
        map<int,int> map_num_max;
        map<int,int> operator_map;

        vector<int> initial_state =  ResourceTask::create_initial_state(parent_task_proxy,resource_detection,map_variables);
        

        vector<numeric_tasks::NumericExplicitVariable> variables = ResourceTask::create_variables(parent_task_proxy, resource_detection);
        
        vector<numeric_tasks::NumericExplicitNumericVariable> num_variables = ResourceTask::create_numeric_variables(parent_task_proxy, resource_detection, map_num_variables,map_num_max);
        vector<numeric_tasks::NumericExplicitComparison> num_comparators;
        vector<numeric_tasks::NumericExplicitAssignment> num_assignments;
//        cout << "after creation variables" << endl;
//        for (NumericExplicitNumericVariable & vars : num_variables){
//            cout << vars.name << " " << vars.num_type << " " << vars.initial_value << " - ";
//        }
//        cout << endl;

        
        vector<numeric_tasks::NumericExplicitOperator> operators;
        
        ResourceTask::create_unchanged_operators(parent_task_proxy, resource_detection.operators_unchanged(), operators,map_variables,operator_map);
        
        ResourceTask::create_numeric_operators(parent_task_proxy, resource_detection, operators,map_variables,map_num_variables,map_num_max,num_comparators,num_assignments,num_variables,initial_state,variables,operator_map);
        

        vector<Fact> goals = ResourceTask::create_goals(parent_task_proxy,map_variables);
        
//        cout << "after creation variables" << endl;
//        int id_n = 0;
//        for (NumericExplicitNumericVariable & vars : num_variables){
//            cout << id_n++ << " " << vars.name << " " << vars.num_type << " " << vars.initial_value << "\n";
//        }
        return make_shared<ResourceTask>(
                                                     parent,
                                                     move(variables),
                                                     move(num_variables),
                                                     move(num_comparators),
                                                     move(num_assignments),
                                                    ResourceTask::create_mutexes(parent_task_proxy),
                                                     move(operators),
                                                     move(initial_state),
                                                     move(goals), move(map_variables), move(map_num_variables), move(operator_map), resource_detection);
    }
    
    vector<ap_float> ResourceTask::get_numeric_state_values(const GlobalState &global_state) const {
        vector<ap_float> values = numeric_state;
        for (int var : variables_resource){
            int id_var = num_variables_map.find(var)->second;
            values[id_var] = resource_detection.get_mu(var,global_state[var]);
        }
        return values;
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
        
        return create_resource_task(
                                                  opts.get<shared_ptr<AbstractTask>>("transform"), true);
    }
    
    static PluginShared<AbstractTask> _plugin("resource", _parse);
}

namespace extra_tasks_variation {
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
        
        return extra_tasks::create_resource_task(
                                    opts.get<shared_ptr<AbstractTask>>("transform"), false);
    }
    
    static PluginShared<AbstractTask> _plugin("resource_nosingle", _parse);
}
