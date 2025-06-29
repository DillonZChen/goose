#ifndef NUMERIC_LANDMARKS_LANDMARK_FACTORY_SCALA_H
#define NUMERIC_LANDMARKS_LANDMARK_FACTORY_SCALA_H

#include "../landmarks/landmark_factory.h"
#include "../landmarks/landmark_graph.h"
#include "../numeric_operator_counting/numeric_helper.h"

#include "../globals.h"

#include "../utils/hash.h"

#include <unordered_set>
#include <utility>
#include <vector>
#include <stack>

namespace landmarks {

class LandmarkFactoryScala {
private:
    TaskProxy task;
    numeric_helper::NumericTaskProxy numeric_task;
    vector<set<int>> achieve; // index: action, value, set of conditions are achieved by the action // this may be useless, just get the post-conditions!
    
    vector<set<int>> possible_achievers; // index: action, value, set of numeric conditions that can be achieved by the action
    vector<set<int>> possible_achievers_inverted; // index: numeric condition, value, set actions that can modify the numeric achiever
    vector<vector<double>> net_effects; // index: action, index n_condition, value: net effect;

    vector<set<int>> condition_to_action;
    vector<double> cond_dist;
    vector<double> cond_num_dist;
    vector<bool> is_init_state; // TODO erease, for debug only
    vector<bool> set_lm; // this is created to check if the vector lm is initialised or it's actually empty;
    vector<bool> set_never_active;
    vector<set<int>> reach_achievers; // this is actually not essential at the moment TODO: add this to the cost-partitioning
    vector<FactProxy> facts_collection;
    vector<set<int>> lm;
    
    void update_actions_conditions(const State &s0, OperatorProxy &gr, stack<OperatorProxy> & a_plus, vector<bool> &never_active, vector<set<int> > &lm);
    void update_action_condition(OperatorProxy &gr, int comp, vector<set<int> > &lm, vector<bool> &never_active, stack<OperatorProxy> & a_plus);
    bool update_lm(int p, OperatorProxy &gr, vector<set<int> > &lm);
    set<int> metric_sensitive_intersection(set<int> & previous, set<int> & temp);
    bool check_conditions(int gr2);
    void generate_link_precondition_action();
    void generate_possible_achievers();
    bool check_if_smark_intersection_needed();
    set<int> reachable;
    set<int> goal_landmarks;
    set<int> newset;
    set<int> action_landmarks;
public:
    LandmarkFactoryScala(const std::shared_ptr<AbstractTask> t);
    ~LandmarkFactoryScala() {
        
    }
    
    set<int> & compute_landmarks(const State &state);
    set<int> & compute_action_landmarks(set<int> &fact_landmarks);
    //TODO, improve this, this can be done only if compute_landmarks(const State &state) has been called.
    vector<set<int>> & get_landmarks_table(){
        return lm;

    }
};
}

#endif
