#include "invariant_constraints.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"

using namespace std;
using namespace numeric_helper;

namespace operator_counting {

    void InvariantConstraints::add_indices_to_constraint(lp::LPConstraint &constraint,
                                   const set<int> &indices,
                                   double coefficient) {
        for (int index : indices) {
            constraint.insert(index, coefficient);
        }
    }

    void InvariantConstraints::build_propositions(const TaskProxy &task_proxy) {
        VariablesProxy vars = task_proxy.get_variables();
        propositions.reserve(vars.size());
        index_constraints.reserve(vars.size());
        index_sometimes_deleted.reserve(vars.size());
        for (VariableProxy var : vars) {
            propositions.push_back(vector<Proposition>(var.get_domain_size()));
            index_constraints.push_back(vector<set<int>>(var.get_domain_size()));
            index_sometimes_deleted.push_back(vector<set<int>>(var.get_domain_size()));
        }
        OperatorsProxy ops = task_proxy.get_operators();
        for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
            const OperatorProxy &op = ops[op_id];
            vector<int> precondition(vars.size(), -1);
            for (FactProxy condition : op.get_preconditions()) {
                int pre_var_id = condition.get_variable().get_id();
                precondition[pre_var_id] = condition.get_value();
            }
            for (EffectProxy effect_proxy : op.get_effects()) {
                FactProxy effect = effect_proxy.get_fact();
                int var = effect.get_variable().get_id();
                int pre = precondition[var];
                int post = effect.get_value();
                assert(post != -1);
                assert(pre != post);
                
                if (pre != -1) {
                    propositions[var][post].always_produced_by.insert(op_id);
                    propositions[var][pre].always_consumed_by.insert(op_id);
                } else {
                    propositions[var][post].sometimes_produced_by.insert(op_id);
                    for (int del = 0; del < task_proxy.get_variables()[var].get_domain_size(); ++del){
                        if (del != post) {
                            index_sometimes_deleted[var][del].insert(op_id);
                        }
                    }
                }
            }
        }
    }
    
    void InvariantConstraints::add_constraints(
                                                   vector<lp::LPConstraint> &constraints, double infinity) {
        
        
        //for (vector<Proposition> &a_vars : propositions) {
        cout << "....................." << get_mutex_group().size() << " mutex groups" << endl;
        for (set<Fact> mutex : get_mutex_group()){
            lp::LPConstraint constraint(-infinity, 1);
            for (Fact f : mutex){
                int var = f.var;
                int val = f.value;
                Proposition &a = propositions[var][val];
                add_indices_to_constraint(constraint, a.always_produced_by, 1.0);
                add_indices_to_constraint(constraint, index_sometimes_deleted[var][val], -1.0);
                add_indices_to_constraint(constraint, a.always_consumed_by, -1.0);
                index_constraints[var][val].insert(constraints.size());
            }
            index_constraints_set.insert(constraints.size());
            constraints.push_back(constraint);
        }
//        for (size_t a_var = 0; a_var < propositions.size(); ++a_var){
//            for (size_t a_val = 0; a_val < propositions[a_var].size(); ++a_val){
//                Proposition &a = propositions[a_var][a_val];
//                lp::LPConstraint constraint(-infinity, 1);
//                add_indices_to_constraint(constraint, a.always_produced_by, 1.0);
//                add_indices_to_constraint(constraint, index_sometimes_deleted[a_var][a_val], -1.0);
//                add_indices_to_constraint(constraint, a.always_consumed_by, -1.0);
//                bool found_mutex = false;
//                set<pair<int,int>>to_add;
//                for (size_t b_var = 0; b_var < propositions.size(); ++b_var){
//                    for (size_t b_val = 0; b_val < propositions[b_var].size(); ++b_val){
//                        Proposition &b = propositions[b_var][b_val];
//                        // check if they are mutex
//                        if ( a_val == b_val && a_var == b_var) continue;
//                        if ( are_mutex(Fact(a_var,a_val),Fact(b_var,b_val))){
//                            // this is to enure that non-trivial mutex are inserted
//                            if (a_var != b_var) found_mutex = true;
//                            cout << " - " << b_var << " " << b_val;
//                            add_indices_to_constraint(constraint, b.always_produced_by, 1.0);
//                            add_indices_to_constraint(constraint, index_sometimes_deleted[b_var][b_val], -1.0);
//                            add_indices_to_constraint(constraint, b.always_consumed_by, -1.0);
//                            to_add.insert(make_pair(b_var,b_val));
//                        }
//                    }
//                }
//                if (found_mutex) {
//                    //cout << "............................................mutex found " << endl;
//                    cout << " - " << a_var << " " << a_val << " : mutex " << constraints.size() <<   endl;
//                    index_constraints[a_var][a_val].insert(constraints.size());
//                    for (pair<int,int> b : to_add){
//                        index_constraints[b.first][b.second].insert(constraints.size());
//                    }
//                    index_constraints_set.insert(constraints.size());
//                    constraints.push_back(constraint);
//                } else {
//                    cout << " null or trivial mutex" << endl;
//                }
//            }
//        }
    }
    
void InvariantConstraints::initialize_constraints(
    const shared_ptr<AbstractTask> task, vector<lp::LPConstraint> &constraints,
    double infinity) {

    TaskProxy task_proxy(*task);
    verify_no_conditional_effects(task_proxy);
    build_propositions(task_proxy);
    add_constraints(constraints, infinity);
}

    // no need to update the constraint
bool InvariantConstraints::update_constraints(const State &state,
                                                  lp::LPSolver &lp_solver) {
    // Compute the bounds for the rows in the LP.
    map<int,int> check;//(index_constraints_set.size(),1);
    //cout << "new state " << endl;
    for (int i : index_constraints_set){
        lp_solver.set_constraint_upper_bound(i, 1);
        check[i] = 1;
    }
    for (size_t var = 0; var < propositions.size(); ++var) {
        //int num_values = propositions[var].size();
        //for (int value = 0; value < num_values; ++value) {
        // Set row bounds.
        //if (state[var].get_value() == value) {
       // cout << var << " " << state[var].get_value() << " is in " << index_constraints[var][state[var].get_value()].size() << " constraints" << endl;
        for(int i : index_constraints[var][state[var].get_value()]){
            //cout << "\t" << i << endl;
            int ub = -1;
            if (check.find(i)!=check.end() && check.find(i)->second==0) ub = 0;
            //cout << var << " " << state[var].get_value() << " present in " << i << endl;
            if (ub == 0) cout << var << " " << state[var].get_value() << " already present in " << i << endl;
            lp_solver.set_constraint_upper_bound(i, 0);
            check[i] = 0;
        }
        //}
        //}
    }
    return false;
}

static shared_ptr<ConstraintGenerator> _parse(OptionParser &parser) {
    parser.document_synopsis(
                             "Delete relaxation constraints",
                             "For details, see" + utils::format_paper_reference(
                                                                                {"Chiara Piacentini", "Margarita P. Castro", "André Augusto Ciré", "J. Christopher Beck"},
                                                                                "Linear and Integer Programming-Based Heuristics for Cost-Optimal Numeric Planning.",
                                                                                "",
                                                                                "AAAI",
                                                                                "6254-6261",
                                                                                "2018"));
    
    if (parser.dry_run())
        return nullptr;
    return make_shared<InvariantConstraints>();

    if (parser.dry_run())
        return nullptr;
    return make_shared<InvariantConstraints>();
}

static PluginShared<ConstraintGenerator> _plugin("invariant_constraints", _parse);
}
