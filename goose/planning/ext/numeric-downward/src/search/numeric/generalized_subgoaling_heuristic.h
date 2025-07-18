#ifndef GENERALIZED_SUBGOALING_HEURISTIC_H
#define GENERALIZED_SUBGOALING_HEURISTIC_H

#include "../heuristic.h"
#include "../lp/lp_solver.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../priority_queue.h"
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;

namespace generalized_subgoaling_heuristic {

    //TODO do we really need the interval relaxation?

class GeneralizedSubgoalingHeuristic : public Heuristic {
protected:
	virtual void initialize();
	virtual ap_float compute_heuristic(const GlobalState &global_state);
	virtual ap_float update_cost(ap_float old_cost, ap_float new_cost) {return std::max(old_cost, new_cost);}
    
    void setup(const GlobalState &global_state);

    vector<shared_ptr<lp::LPSolver>> lps;
    vector<unordered_map<int, int>> action_to_variable_index;
    vector<unordered_map<int, int>> conjunct_to_constraint_index;
    unordered_map<vector<int>, int> preconditions_to_id;
    vector<int> action_to_preconditions_id;
    vector<set<int>> condition_to_action; // index condition, value set of action with that preconditions
    vector<double> dist;
    vector<bool> open;
    vector<bool> closed;
    vector<bool> active_actions;

    vector<set<int>> effect_of; // index: proposition, value, set actions that can achieved the proposition 
    vector<set<int>> possible_achievers; // index: action, value, set of numeric conditions that can be achieved by the action
    vector<set<int>> possible_achievers_inverted; // index: set of numeric conditions that can be achieved by the action, value, action
    vector<set<int>> possible_preconditions_achievers; // index: action, value, set of preconditions that can be achieved by the action
    vector<set<int>> possible_preconditions_achievers_inverted; // index: preconditions, value, actions that can achieve the preconditions
    numeric_helper::NumericTaskProxy numeric_task;
    vector<vector<double>> net_effects; // index: action, index n_condition, value: net effect;
    double max_float;
    void update_constraints(int preconditions_id, const State &state);
    double min_over_possible_achievers(int nc_id);
    void update_cost_if_necessary(int cond, HeapQueue<int> &q, double current_cost);
    void generate_possible_achievers();
    void generate_preconditions();
    void generate_linear_programs(lp::LPSolverType solver_type, lp::LPConstraintType constraint_type);
public:
	GeneralizedSubgoalingHeuristic(const options::Options &options);
	~GeneralizedSubgoalingHeuristic();
    
};
}

#endif
