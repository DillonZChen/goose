#ifndef RMAX_HEURISTIC_H
#define RMAX_HEURISTIC_H

#include "interval_relaxation_heuristic.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../priority_queue.h"
#include <vector>
#include <set>

using namespace std;

namespace rmax_heuristic {

    //TODO do we really need the interval relaxation?

class RMaxHeuristic : public interval_relaxation_heuristic::IntervalRelaxationHeuristic {
protected:
	virtual void initialize();
	virtual ap_float compute_heuristic(const GlobalState &global_state);
	virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {return std::max(old_cost, new_cost);}
    
    void setup(const GlobalState &global_state);
    
    //
    bool restrict_achievers;
    bool ceiling_less_than_one;
    vector<set<int>> condition_to_action; // index condition, value set of action with that preconditions
    vector<double> cond_dist;
    vector<double> cond_num_dist;
    vector<double> action_dist;
    vector<bool> is_init_state; // TODO erease, for debug only
    vector<bool> closed;
    
    vector<set<int>> possible_achievers; // index: action, value, set of numeric conditions that can be achieved by the action
    vector<set<int>> possible_achievers_inverted; // index: numeric condition, value, set actions that can modify the numeric achiever
    vector<set<int>> all_achievers;
    numeric_helper::NumericTaskProxy numeric_task;
    void update_reachable_conditions_actions(const State &s_0, int gr, HeapQueue<int>& a_plus);
    void update_reachable_actions(int gr, int cond, HeapQueue<int>& a_plus);
    vector<vector<double>> net_effects; // index: action, index n_condition, value: net effect;
    vector<vector<double>> action_comp_number_execution;
    double check_conditions(int gr_id);
    double max_float;
    void generate_possible_achievers();
    void generate_preconditions();
    double check_goal();
    double get_number_of_execution(int gr, const State &s_0, int n_condition);
    double min_over_possible_achievers(int nc_id);
public:
	RMaxHeuristic(const options::Options &options);
	~RMaxHeuristic();
    
};
}

#endif
