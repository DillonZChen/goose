#include "landmark_factory_scala.h"
#include "../landmarks/landmark_graph.h"
#include "../global_operator.h"
#include "../global_state.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../axioms.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;
using namespace numeric_helper;
using namespace landmarks;

double max_float = 999999;
bool smart_intersection = true;

LandmarkFactoryScala::LandmarkFactoryScala(const shared_ptr<AbstractTask> t) : task(TaskProxy(*t)) {
    numeric_task = NumericTaskProxy(task);
    lm.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),set<int>());
    generate_link_precondition_action();
    generate_possible_achievers();
    smart_intersection = check_if_smark_intersection_needed();
}

void LandmarkFactoryScala::generate_link_precondition_action(){
    condition_to_action.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),set<int>());
    //cout << "***********" << numeric_task.get_n_propositions() << " " << numeric_task.get_n_conditions() << endl;
    facts_collection.reserve(numeric_task.get_n_propositions());
    int n_propositions = numeric_task.get_n_propositions();
    for (size_t op_id = 0; op_id < task.get_operators().size(); ++op_id){
        const OperatorProxy &op = task.get_operators()[op_id];
        for (FactProxy precondition : op.get_preconditions()) {
            int var = precondition.get_variable().get_id();
            int val = precondition.get_value();
            int c = numeric_task.get_proposition(var,val);
            condition_to_action[c].insert(op_id);
            facts_collection[c] = precondition;
        }
        const set<int> &preconditions = numeric_task.get_action_num_list(op_id);
        for (size_t c_id : preconditions) {
            // numeric preconditions
            for (int nc_id : numeric_task.get_numeric_conditions_id(c_id)){
                condition_to_action[nc_id+n_propositions].insert(op_id);
            }
        }
    }
    // TODO: remove if it is a goal, this is just for debugging purpose
    for (size_t id_goal = 0; id_goal < task.get_goals().size(); ++id_goal) {
        FactProxy goal = task.get_goals()[id_goal];
        int var = goal.get_variable().get_id();
        int val = goal.get_value();
        int c = numeric_task.get_proposition(var,val);
        facts_collection[c] = goal;
    }
}

void LandmarkFactoryScala::generate_possible_achievers(){
    possible_achievers.assign(task.get_operators().size(),set<int>());
    possible_achievers_inverted.assign(numeric_task.get_n_conditions(),set<int>());
    net_effects.assign(task.get_operators().size(),vector<double>(numeric_task.get_n_conditions(),0.));
    size_t n_numeric_variables = numeric_task.get_n_numeric_variables();
    for (size_t nc_id = 0; nc_id < numeric_task.get_n_conditions(); ++nc_id){
        for (size_t op_id = 0; op_id < task.get_operators().size(); ++op_id){
            const LinearNumericCondition &nc = numeric_task.get_condition(nc_id);
            double cumulative_effect = 0;//nc.constant;
            for (size_t v = 0; v < n_numeric_variables; ++v){
                cumulative_effect+=(nc.coefficients[v]*numeric_task.get_action_eff_list(op_id)[v]);
            }
            net_effects[op_id][nc_id] = cumulative_effect;

            if (cumulative_effect>0){
                //cout << task.get_operators()[op_id].get_name() << " effect on " << numeric_task.get_condition(nc_id) << " is " << cumulative_effect << endl;
                //double cumulative_effect = 0;
//                for (size_t v = 0; v < n_numeric_variables; ++v){                                        cout << "\t" << v << " " << nc.coefficients[v]<< " " << numeric_task.get_action_eff_list(op_id)[v] << endl;
//                }
                possible_achievers[op_id].insert(nc_id);
                possible_achievers_inverted[nc_id].insert(op_id);
            }
        }
    }
//    ComparisonAxiomsProxy axioms = task.get_comparison_axioms();
//    for (size_t num_id = 0; num_id < axioms.size(); ++num_id){
//        ComparisonAxiomProxy ax = axioms[num_id];
//        cout << ax.get_true_fact().get_name() << " " << endl;
//        for (int ax_id : numeric_task.get_numeric_conditions_id(num_id))
//            cout << "\t"<< ax_id << " "<< numeric_task.get_condition(ax_id) << endl;
//    }
}

set<int> & LandmarkFactoryScala::compute_landmarks(const State &state) {
    //cout << "Generating landmarks using Scala propagation\n";
    // call proxy task ? how do you do it?
    // compute_estimate
    // priority queue
    //
    cond_dist.assign(numeric_task.get_n_propositions(),max_float);
    cond_num_dist.assign(numeric_task.get_n_conditions(),max_float);
    is_init_state.assign(numeric_task.get_n_conditions(),false);
    set_lm.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false);
    set_never_active.assign(task.get_operators().size()+numeric_task.get_n_conditions(),false);

    OperatorsProxy ops = task.get_operators();
    stack<OperatorProxy> a_plus;
    vector<bool> never_active(ops.size(),true);
    fill(lm.begin(),lm.end(),set<int>());
    reachable.clear();
    
    //update initial state
    
    for (size_t var = 0; var < numeric_task.get_n_vars(); ++var) {
        int val = state[var].get_value();
        int condition = numeric_task.get_proposition(var,val);
        cond_dist[condition] = 0;
    }
    
    for (size_t var = 0; var < numeric_task.get_n_conditions(); ++var) {
        const LinearNumericCondition &num_values = numeric_task.get_condition(var);
        double lower_bound = - num_values.constant;
        for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
            int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
            lower_bound -= (state.nval(id_num) * num_values.coefficients[i]);
        }
        if (lower_bound <= 0){
            cond_num_dist[var] = 0;
            is_init_state[var] = true;
        }
    }
            
    // fill a_plus
    //cout << "===" << endl;
    for (size_t op_id = 0; op_id < ops.size(); ++op_id){
        // check if condition is satisfied
        const OperatorProxy &op = ops[op_id];
        bool applicable = true;
        for (FactProxy condition : op.get_preconditions()) {
            if (state[condition.get_variable().get_id()].get_value() != condition.get_value()){
                applicable = false;
            }
        }
        // add there numeric actions
        const set<int> &preconditions = numeric_task.get_action_num_list(op_id);
        for (int pre : preconditions){
            for (int c : numeric_task.get_numeric_conditions_id(pre)){
                const LinearNumericCondition &lnc = numeric_task.get_condition(c);
                double value = lnc.constant;
                for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
                    int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
                    value += (state.nval(id_num) * lnc.coefficients[i]);
                }
                if (value < 0) applicable = false;
            }
        }
        
        if (applicable){
            //action is applicable
            a_plus.push(op);
            never_active[op_id] = false;
            set_never_active[op_id] = true;
            reachable.insert(op_id);
            //cout << "adding applicable action " << op.get_name() << " to queue " << endl;
        } else {
            //cout << "action " << op.get_name() << " not applicable " << endl;
        }
        
    }
    
    while(!a_plus.empty()){
        OperatorProxy gr = a_plus.top();
        a_plus.pop();
        update_actions_conditions(state, gr, a_plus, never_active, lm);
    }
//    cout << endl;
//    for (set<int> l : lm){
//        cout << "\nlandmarks for l :";
//        for (int s : l)
//            cout << " " << s;
//    }
    goal_landmarks.clear();
    for (size_t id_goal = 0; id_goal < task.get_goals().size(); ++id_goal) {
        FactProxy goal = task.get_goals()[id_goal];
        int var = goal.get_variable().get_id();
        int post = goal.get_value();
        int c = numeric_task.get_proposition(var,post);
        goal_landmarks.insert(lm[c].begin(), lm[c].end());
        if (!numeric_task.is_numeric_axiom(var))
            goal_landmarks.insert(c);
    }

    size_t n_propositions = numeric_task.get_n_propositions();
    for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
        list<int> goals = numeric_task.get_numeric_goals(id_goal);
        for (int id_n_con : goals){
            int c = id_n_con + n_propositions;
            goal_landmarks.insert(lm[c].begin(), lm[c].end());
            goal_landmarks.insert(c);
        }
    }
//    cout << "\n" << goal_landmarks.size() << " goal landmarks found" << endl;
//    for (int l : goal_landmarks){
//        if (l < n_propositions)
//            cout << facts_collection[l].get_name() << endl;
//        else{
//            if (!is_init_state[l - n_propositions])
//                cout << "condition " << numeric_task.get_condition(l - n_propositions) << endl;
//            //else
//            //    cout << "already satisfied " << numeric_task.get_condition(l - n_propositions) << endl;
//
//        }
//    }
    return goal_landmarks;
//    return set<int>();
}

void LandmarkFactoryScala::update_actions_conditions(const State &s0, OperatorProxy &gr, stack<OperatorProxy> & a_plus, vector<bool> &never_active, vector<set<int>> &lm){
    
    //cout << "action " << gr.get_name() << endl;
    for (EffectProxy effect_proxy : gr.get_effects()) {
        FactProxy effect = effect_proxy.get_fact();
        int var = effect.get_variable().get_id();
        int post = effect.get_value();
        int condition = numeric_task.get_proposition(var,post);
        if (cond_dist[condition] > 0) {
            //cout <<"\tprop effect " << effect.get_name() << " is now active" << endl;
            cond_dist[condition] = 1;
            update_action_condition(gr, condition, lm, never_active, a_plus);
        }
    }
    
    for (int nc_id : possible_achievers[gr.get_id()]){
        //LinearNumericCondition &lnc = numeric_task.get_condition(nc_id);
        //cout << "\tpossible achiever " << lnc << endl;
        if (cond_num_dist[nc_id] > 0){
            cond_num_dist[nc_id] = 1;
            //cout <<"\tnum condition " << nc_id+numeric_task.get_n_propositions() << " is now active" << " : " << numeric_task.get_condition(nc_id) << " " << cond_num_dist[nc_id] << endl;
            update_action_condition(gr, nc_id+numeric_task.get_n_propositions(), lm, never_active, a_plus);
        }
    }

//    cout << "after applying action " << gr.get_name() << " landmarks are : " << endl;
//    for (size_t i = 0; i < lm.size(); ++i){
//        //if (!set_lm[i]) continue;
//        if (lm[i].empty()) continue;
//        //cout << "\tFact " << facts_collection[i].get_name() << "\n\t\t";
//        cout << "\tFact " << i << " (" << lm[i].size() <<")\n\t\t";
//        for (int l : lm[i])
//            cout << " " << l;// facts_collection[l].get_name();
//        cout << "\n";
//    }
}

bool LandmarkFactoryScala::check_conditions(int gr2){
    const OperatorProxy &op = task.get_operators()[gr2];
    for (FactProxy precondition : op.get_preconditions()){
        int var = precondition.get_variable().get_id();
        int val = precondition.get_value();
        int c = numeric_task.get_proposition(var,val);
        // TODO: this can be done better (numeric tasks)
        if (cond_dist[c] == max_float && !numeric_task.is_numeric_axiom(var)) {
            //cout << "\t\t\t\taction not satified because " << precondition.get_name() << " not satisfied" << endl;
            return false;
            
        }
    }
    // add numeric precondition
    const set<int> &preconditions = numeric_task.get_action_num_list(gr2);
    for (int precondition : preconditions){
        for (int nc_id : numeric_task.get_numeric_conditions_id(precondition)){
            if (cond_num_dist[nc_id] == max_float)  {
                //cout << "\t\t\t\taction not satisfied because numeric precondition " << nc_id << " : " << numeric_task.get_condition(nc_id) << endl;
                return false;
            }
        }
    }
    return true;
}

void LandmarkFactoryScala::update_action_condition(OperatorProxy &gr, int comp, vector<set<int> > &lm, vector<bool> &never_active, stack<OperatorProxy> & a_plus){
    // TODO fill this
    bool changed = update_lm(comp, gr, lm);
    /// comment
//    cout << "\tafter applying condition ";
//    if (comp < numeric_task.get_n_propositions())
//        cout << numeric_task.get_proposition_name(comp);
//    else
//        cout << numeric_task.get_condition(comp-numeric_task.get_n_propositions());
//    cout << " landmarks are" << endl;
//    for (size_t i = 0; i < lm.size(); ++i){
//        //if (set_lm[i]) continue;
//        if (lm[i].empty()) continue;
//        //cout << "\tFact " << facts_collection[i].get_name() << "\n\t\t";
//        cout << "\t\tFact ";
//        if (i < numeric_task.get_n_propositions())
//            cout << numeric_task.get_proposition_name(i);
//        else
//            cout << numeric_task.get_condition(i-numeric_task.get_n_propositions());
//        
//        cout<< " (" << lm[i].size() <<")\n";
//        for (int l : lm[i]){
//            if (l < numeric_task.get_n_propositions())
//                cout << "\t\t\t" << numeric_task.get_proposition_name(l) << endl;
//            else
//                cout << "\t\t\tnumeric " << numeric_task.get_condition(l-numeric_task.get_n_propositions()) << endl;
//        }
//    }
    //cout << "\tchanged" << changed << endl;
    set<int> & set = condition_to_action[comp];
    for (int gr2 : set){
        if (gr2 == gr.get_id()) continue;
        //if (!set_never_active[gr2]) continue; // TODO this is useless
        if (never_active[gr2]){
            //cout << "\t\tcheck_conditions of " << task.get_operators()[gr2].get_name() << endl;
            if (check_conditions(gr2)){
                //cout << "\t\t\tis active" << endl;
                a_plus.push(task.get_operators()[gr2]);
                never_active[gr2] = false;
                set_never_active[gr2] = true;
            }
        } else if (changed) {
            a_plus.push(task.get_operators()[gr2]);
        }
    }
    // TODO: add this (line 450) I don't think this is important (it's for reachability)
}

set<int> & LandmarkFactoryScala::compute_action_landmarks(set<int> & fact_landmarks){
    action_landmarks.clear();
    int n = numeric_task.get_n_propositions();
    for (int f : fact_landmarks){ // facts that are not already satisfied
        if (f < n && cond_dist[f] == 0) continue;
        if (f >= n && cond_num_dist[f - n] == 0) continue;
        if (numeric_task.get_achievers(f).size()==1){
            action_landmarks.insert(*numeric_task.get_achievers(f).begin());
        }
    }
    return action_landmarks;
}

bool LandmarkFactoryScala::update_lm(int p, OperatorProxy &gr, vector<set<int> > &lm){
    set<int> & previous = lm[p];
    set<int> toAdd;
    set<int> temp;

    if ( !set_lm[p]){
        // this is the first lm
        // add precondition of the gr
        int var, val, c;
        for (FactProxy precondition : gr.get_preconditions()){
            var = precondition.get_variable().get_id();
            val = precondition.get_value();
            c = numeric_task.get_proposition(var,val);
            if (cond_dist[c] == 1 && !numeric_task.is_numeric_axiom(var)){ //TODO check if this is still valid for numeric preconditions
                // previous.addAll(lm.get(c.getCounter())); // from scala's code
                previous.insert(lm[c].begin(), lm[c].end());
                previous.insert(c);
            }
        }
        const set<int> &preconditions = numeric_task.get_action_num_list(gr.get_id());
        for (int precondition : preconditions){
            for (int nc_id : numeric_task.get_numeric_conditions_id(precondition)){
                if (cond_num_dist[nc_id] == 1)  {
                    previous.insert(lm[nc_id+numeric_task.get_n_propositions()].begin(), lm[nc_id+numeric_task.get_n_propositions()].end());
                    previous.insert(nc_id+numeric_task.get_n_propositions());
                }
            }
        }
        lm[p] = previous;
        set_lm[p]= true;
        return true;
    } else{
        // revise lm
        size_t previous_size = previous.size();
        toAdd.clear();

        if (previous_size==0)
            return false;
        
        if (smart_intersection){
            temp.clear();
            for (FactProxy precondition : gr.get_preconditions()){
                int var = precondition.get_variable().get_id();
                int val = precondition.get_value();
                int c = numeric_task.get_proposition(var,val);
                if (cond_dist[c] == 1 && !numeric_task.is_numeric_axiom(var)){ //TODO check if this is still valid for numeric preconditions
                    // previous.addAll(lm.get(c.getCounter())); // from scala's code
                    temp.insert(lm[c].begin(), lm[c].end());
                    temp.insert(c);
                }
            }
            const set<int> &preconditions = numeric_task.get_action_num_list(gr.get_id());
            for (int precondition : preconditions){
                for (int nc_id : numeric_task.get_numeric_conditions_id(precondition)){
                    if (cond_num_dist[nc_id] == 1)  {
                        int c = nc_id + numeric_task.get_n_propositions();
                        temp.insert(lm[c].begin(), lm[c].end());
                        temp.insert(c);
                    }
                }
            }
            // add metric sensitive intersection
            toAdd =  metric_sensitive_intersection(previous, temp);
        }else{
            // to insert here, also the add effect of the action
            for (int prev : previous){
                bool found = false;
                for (FactProxy precondition : gr.get_preconditions()){
                    int var = precondition.get_variable().get_id();
                    int val = precondition.get_value();
                    int c = numeric_task.get_proposition(var,val);
                    if (cond_dist[c] == 1 && !numeric_task.is_numeric_axiom(var)){ //TODO check if this is still valid for numeric preconditions
                        // line 352
                        if (c==prev){
                            found = true;
                            break;
                        }
                    }
                    // add condition here
                    for (int c1 : lm[c]){
                        if (prev == c1){
                            found = true;
                            break;
                        }
                    }
                    
                }
                const set<int> &preconditions = numeric_task.get_action_num_list(gr.get_id());
                for (int precondition : preconditions){
                    for (int nc_id : numeric_task.get_numeric_conditions_id(precondition)){
                        int c = nc_id + numeric_task.get_n_propositions();
                        if (cond_num_dist[nc_id]  == 1)  { // this is useless, they should be already satisfied
                            if (c == prev){
                                found = true;
                                break;
                            }
                        }
                        // add condition here
                        set<int> &lmc = lm[c];
                        for (int c1 : lmc){
                            if (prev == c1){
                                found = true;
                                break;
                            }
                        }
                    }
                }
                if (found){
                    toAdd.insert(prev);
                }
            }
        }
        lm[p] = toAdd;
        set_lm[p]= true;
        if (toAdd.size()!= previous_size)
            return true;
    }
    return false;
}

set<int> LandmarkFactoryScala::metric_sensitive_intersection(set<int> & previous, set<int> & temp){
    newset.clear();
    set<int>::iterator prev_end = previous.end();
    int n_propositions = numeric_task.get_n_propositions();
    for (int c : temp){
        // if predicates
        // TODO: change this for metric conditions
        if (c < n_propositions){ // it's a proposition
            if (previous.find(c)!= prev_end)
                newset.insert(c);
        } else { // it's a numeric condition
//            if (previous.find(c)!=previous.end())
//                newset.insert(c);
            for ( int c1 : previous) {
                if (c1 >= n_propositions) {
//                    if (numeric_task.get_condition(c - n_propositions).dominate(numeric_task.get_condition(c1 - n_propositions))){
//                        newset.insert(c1);
//                    }else if (numeric_task.get_condition(c1 - n_propositions).dominate(numeric_task.get_condition(c - n_propositions))) {
//                        newset.insert(c);
//                    }
                    if (numeric_task.get_dominance(c - n_propositions,c1 - n_propositions)){
                        newset.insert(c1);
                    }else if (numeric_task.get_dominance(c1 - n_propositions,c - n_propositions)){
                        newset.insert(c);
                    }
                    
                }
            }
            
        }
    }
    return newset;
}

bool LandmarkFactoryScala::check_if_smark_intersection_needed(){
    for (size_t nc_id = 0; nc_id < numeric_task.get_n_conditions(); ++nc_id ){
        const LinearNumericCondition &lnc = numeric_task.get_condition(nc_id);
        for (size_t nc2_id = 0; nc2_id < numeric_task.get_n_conditions(); ++nc2_id ){
            const LinearNumericCondition &lnc2 = numeric_task.get_condition(nc2_id);
            // if there is at least one domination, than it's needed
            if (numeric_task.get_dominance(nc_id,nc2_id)) return true;
            //if (lnc.dominate(lnc2)) return true;
        }
    }
    return false;
}

static LandmarkGraph *_parse(OptionParser &parser) {
    parser.document_synopsis(
                             "Scala Landmarks",
                             "The landmark generation method introduced by "
                             "Scala et al (IJCAI 2017).");
    parser.document_note("Relevant options", "reasonable_orders, no_orders");
    LandmarkGraph::add_options_to_parser(parser);
    Options opts = parser.parse();
    
    // TODO: Make sure that conditional effects are indeed supported.
    parser.document_language_support("conditional_effects",
                                     "We think they are supported, but this "
                                     "is not 100% sure.");
    opts.set<bool>("supports_conditional_effects", true);
    
    if (parser.dry_run()) {
        return 0;
    } else {
        //opts.set<Exploration *>("explor", new Exploration(opts));
        //LandmarkFactoryScala lm_graph_factory(opts);
        //LandmarkGraph *graph = lm_graph_factory.compute_lm_graph();
        //return graph;
        0;
    }
}

static Plugin<LandmarkGraph> _plugin("lm_scala", _parse);



