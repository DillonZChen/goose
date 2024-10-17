
#include "numeric_explicit_task.h"

#include "../utils/system.h"
#include "../global_operator.h"

#include <algorithm>
#include <functional>
#include <unordered_set>

using namespace std;
using namespace tasks;

namespace numeric_tasks {
#ifndef NDEBUG
    static bool check_fact(const Fact &fact) {
        /*
         We don't put this check into the Fact ctor to allow (-1, -1)
         facts. Here, we want to make sure that the fact is valid.
         */
        return fact.var >= 0 && fact.value >= 0;
    }
#endif
    
    NumericExplicitTask::NumericExplicitTask(
                               const shared_ptr<AbstractTask> &parent,
                               vector<NumericExplicitVariable> &&variables,
                               vector<NumericExplicitNumericVariable> &&num_variables,
                               vector<NumericExplicitComparison> &&num_comp,
                               vector<NumericExplicitAssignment> &&num_effects,
                               vector<vector<set<Fact>>> &&mutex_facts,
                               vector<NumericExplicitOperator> &&operators,
                               vector<NumericExplicitOperator> &&axioms,
                               vector<int> &&initial_state_values,
                               vector<Fact> &&goals, map<int,int> &&vars_map,
                                             map<int,int> &&num_vars_map, map<int,int> &&ops_map)
    : DelegatingTask(parent),
    variables(move(variables)),
    num_variables(move(num_variables)),
    num_comp(move(num_comp)),
    num_effects(move(num_effects)),
    mutexes(move(mutex_facts)),
    operators(move(operators)),
    axioms(move(axioms)),
    initial_state_values(move(initial_state_values)),
    goals(move(goals)),
    variables_map(move(vars_map)),
    num_variables_map(move(num_vars_map)),
    operator_map(move(ops_map))
    {
        for (auto &eff : this->num_variables){
            initial_state_numeric_values.push_back(eff.initial_value);
            numeric_state.push_back(eff.initial_value);
        }
        assert(run_sanity_check());
    }
    
    bool NumericExplicitTask::run_sanity_check() const {
        //cout << "initial " << initial_state_values.size() << " " << variables.size() << endl;
        assert(initial_state_values.size() == variables.size());
        
        function<bool(const ExplicitOperator &op)> is_axiom =
        [](const ExplicitOperator &op) {
            return op.is_an_axiom;
        };
        //assert(none_of(operators.begin(), operators.end(), is_axiom));
        //assert(all_of(axioms.begin(), axioms.end(), is_axiom));
        
        // Check that each variable occurs at most once in the goal.
        unordered_set<int> goal_vars;
        for (const Fact &goal: goals) {
            goal_vars.insert(goal.var);
        }
        assert(goal_vars.size() == goals.size());
        return true;
    }
    
    int NumericExplicitTask::get_num_variables() const {
        return variables.size();
    }
    
    const string &NumericExplicitTask::get_variable_name(int var) const {
        return get_variable(var).name;
    }
    
    int NumericExplicitTask::get_variable_domain_size(int var) const {
        return get_variable(var).domain_size;
    }
    
    //int NumericExplicitTask::get_variable_axiom_layer(int var) const {
    //    return get_variable(var).axiom_layer;
    //}
    
    //int NumericExplicitTask::get_variable_default_axiom_value(int var) const {
    //    return get_variable(var).axiom_default_value;
    //}
    
    const string &NumericExplicitTask::get_fact_name(const Fact &fact) const {
        assert(utils::in_bounds(fact.value, get_variable(fact.var).fact_names));
        return get_variable(fact.var).fact_names[fact.value];
    }
    
    bool NumericExplicitTask::are_facts_mutex(const Fact &fact1, const Fact &fact2) const {
        if (fact1.var == fact2.var) {
            // Same variable: mutex iff different value.
            return fact1.value != fact2.value;
        }
        assert(utils::in_bounds(fact1.var, mutexes));
        assert(utils::in_bounds(fact1.value, mutexes[fact1.var]));
        return bool(mutexes[fact1.var][fact1.value].count(fact2));
    }
    
    ap_float NumericExplicitTask::get_operator_cost(int index, bool is_axiom) const {
        return get_operator_or_axiom(index, is_axiom).cost;
    }
    
    const string &NumericExplicitTask::get_operator_name(int index, bool is_axiom) const {
        return get_operator_or_axiom(index, is_axiom).name;
    }
    
    int NumericExplicitTask::get_num_operators() const {
        return operators.size();
    }
    
    int NumericExplicitTask::get_num_operator_preconditions(
                                                     int index, bool is_axiom) const {
        return get_operator_or_axiom(index, is_axiom).preconditions.size();
    }
    
    Fact NumericExplicitTask::get_operator_precondition(
                                                 int op_index, int fact_index, bool is_axiom) const {
        const NumericExplicitOperator &op = get_operator_or_axiom(op_index, is_axiom);
        assert(utils::in_bounds(fact_index, op.preconditions));
        return op.preconditions[fact_index];
    }
    
    int NumericExplicitTask::get_num_operator_effects(int op_index, bool is_axiom) const {
        return get_operator_or_axiom(op_index, is_axiom).effects.size();
    }
    
    int NumericExplicitTask::get_num_operator_ass_effects(int op_index, bool is_axiom) const {
        const NumericExplicitOperator &op = get_operator_or_axiom(op_index, is_axiom);
        //cout << "operator has " << op_index << " " << is_axiom << " " << op.num_effects.size() << " effects and preconditions " << op.num_preconditions.size() << " " << op.preconditions.size() << " " << op.effects.size() << endl;
        return op.num_effects.size();
    }
    int NumericExplicitTask::get_num_operator_ass_effect_conditions(int op_index, int ass_eff_index, bool is_axiom) const {
        const NumericExplicitOperator &op = get_operator_or_axiom(op_index, is_axiom);
        if (op.copy_id == -1) return 0;
        return parent->get_num_operator_ass_effect_conditions(op.copy_id, ass_eff_index, is_axiom);
    }
    int NumericExplicitTask::get_num_operator_effect_conditions(
                                                         int op_index, int eff_index, bool is_axiom) const {
        return get_effect(op_index, eff_index, is_axiom).conditions.size();
    }
    
    Fact NumericExplicitTask::get_operator_effect_condition(
                                                     int op_index, int eff_index, int cond_index, bool is_axiom) const {
        const NumericExplicitEffect &effect = get_effect(op_index, eff_index, is_axiom);
        assert(utils::in_bounds(cond_index, effect.conditions));
        return effect.conditions[cond_index];
    }
    
    Fact NumericExplicitTask::get_operator_effect(
                                           int op_index, int eff_index, bool is_axiom) const {
        return get_effect(op_index, eff_index, is_axiom).fact;
    }
    
    AssEffect NumericExplicitTask::get_operator_ass_effect(int op_index,
                                                    int eff_index, bool is_axiom) const {
        const NumericExplicitOperator &op = get_operator_or_axiom(op_index, is_axiom);
        const NumericExplicitAssignment &ass = op.num_effects[eff_index];
        return AssEffect(ass.lhs, ass.op, ass.rhs);//parent->get_operator_ass_effect(op.copy_id,eff_index, is_axiom);
    }
    
    int NumericExplicitTask::get_num_axioms() const {
        return axioms.size();
    }
    
    int NumericExplicitTask::get_num_goals() const {
        return goals.size();
    }
    
    Fact NumericExplicitTask::get_goal_fact(int index) const {
        assert(utils::in_bounds(index, goals));
        return goals[index];
    }
    
    vector<int> NumericExplicitTask::get_initial_state_values() const {
        return initial_state_values;
    }
    
    vector<int> NumericExplicitTask::get_state_values(const GlobalState &global_state) const {
        int num_vars = variables.size();
        vector<int> values(num_vars,1);
        for (pair<int,int> vars : variables_map){ //} = 0; var < num_vars; ++var)}
            values[vars.second] = global_state[vars.first];
        }
        return values;
    }
    
    vector<ap_float> NumericExplicitTask::get_numeric_state_values(const GlobalState &global_state) const {
        return vector<ap_float>(num_variables.size(),0.);
    }
    
    int NumericExplicitTask::get_num_cmp_axioms() const {
        return num_comp.size();
    }
    
    int NumericExplicitTask::get_comparison_axiom_argument(int axiom_index, bool left) const {
        return left ? num_comp[axiom_index].lhs : num_comp[axiom_index].rhs;
    }

    comp_operator NumericExplicitTask::get_comparison_axiom_operator(int axiom_index) const {
        return num_comp[axiom_index].comp;
    }

    Fact NumericExplicitTask::get_comparison_axiom_effect(int axiom_index, bool evaluation_result) const{
        // TODO change this // this is wrong... always get the last variable inserted!
        return Fact(num_comp[axiom_index].id_var,1);
    }
    
    int NumericExplicitTask::get_num_numeric_variables() const{
        return num_variables.size();
    }
    
    const std::string &NumericExplicitTask::get_numeric_variable_name(int var) const {
        return num_variables[var].name;
    }
    
    numType NumericExplicitTask::get_numeric_var_type(int index) const {
        return num_variables[index].num_type;
    }

    std::vector<ap_float> NumericExplicitTask::get_initial_state_numeric_values() const {
        return initial_state_numeric_values;
    }
    
    NumericExplicitVariable::NumericExplicitVariable(
                                       int domain_size,
                                       string &&name,
                                       vector<string> &&fact_names,
                                       int axiom_layer,
                                       int axiom_default_value)
    : domain_size(domain_size),
    name(move(name)),
    fact_names(move(fact_names)),
    axiom_layer(axiom_layer),
    axiom_default_value(axiom_default_value) {
        assert(domain_size >= 1);
        assert(static_cast<int>(this->fact_names.size()) == domain_size);
        assert(axiom_layer >= -1);
        assert(axiom_default_value >= -1);
    }
    
    NumericExplicitNumericVariable::NumericExplicitNumericVariable(
                                                                   string &&name,
                                                                   numType   num_type,
                                                                   double initial_value)
    :
    name(move(name)), num_type(num_type), initial_value(initial_value) {
       
    }
    
    NumericExplicitComparison::NumericExplicitComparison( int l,
                                                         int r,
                                                         comp_operator c, int id_v): lhs(l), rhs(r), comp(c), id_var(id_v){
        
    }
    
    NumericExplicitAssignment::NumericExplicitAssignment( int l,
                                                         int r,
                                                         f_operator o
                                                         ): lhs(l), rhs(r), op(o) {
        
    }
    NumericExplicitEffect::NumericExplicitEffect(
                                   int var, int value, vector<Fact> &&conditions)
    : fact(var, value), conditions(move(conditions)) {
        assert(check_fact(Fact(var, value)));
        assert(all_of(
                      this->conditions.begin(), this->conditions.end(), check_fact));
    }
    
    
    NumericExplicitOperator::NumericExplicitOperator(
                                       vector<Fact> &&preconditions,
                                       vector<NumericExplicitEffect> &&effects,
                                       int cost,
                                       const string &name,
                                       bool is_an_axiom,
                                       int copy_id
                                       )
    : preconditions(move(preconditions)),
    effects(move(effects)),
    num_preconditions({}),
    num_effects({}),
    cost(cost),
    name(name),
    is_an_axiom(is_an_axiom),
    copy_id(copy_id){
        assert(all_of(
                      this->preconditions.begin(),
                      this->preconditions.end(),
                      check_fact));
        assert(cost >= 0);
    }
    
    NumericExplicitOperator::NumericExplicitOperator(
                                                     vector<Fact> &&preconditions,
                                                     vector<NumericExplicitEffect> &&effects,
                                                     std::vector<NumericExplicitComparison> &&num_preconditions,
                                                     std::vector<NumericExplicitAssignment> &&num_effects,
                                                     int cost,
                                                     const string &name,
                                                     bool is_an_axiom,
                                                     int copy_id
                                                     )
    : preconditions(move(preconditions)),
    effects(move(effects)),
    num_preconditions(move(num_preconditions)),
    num_effects(move(num_effects)),
    cost(cost),
    name(name),
    is_an_axiom(is_an_axiom),
    copy_id(copy_id){
        assert(all_of(
                      this->preconditions.begin(),
                      this->preconditions.end(),
                      check_fact));
        assert(cost >= 0);
    }
}
