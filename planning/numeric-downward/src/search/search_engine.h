#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "global_operator.h"
#include "operator_cost.h"
#include "search_progress.h"
#include "search_space.h"
#include "search_statistics.h"

#include <vector>

class Heuristic;

namespace options {
class OptionParser;
class Options;
}

enum SearchStatus {IN_PROGRESS, TIMEOUT, FAILED, SOLVED};
class SearchEngine {
public:
    typedef std::vector<const GlobalOperator *> Plan;
private:
    SearchStatus status;
    bool solution_found;
    Plan plan;
protected:
    SearchSpace search_space;
    SearchProgress search_progress;
    SearchStatistics statistics;
    int bound;
    OperatorCost cost_type;
    double max_time;

    virtual void initialize() {}
    virtual SearchStatus step() = 0;

    void set_plan(const Plan &plan);
    bool check_goal_and_set_plan(const GlobalState &state);
    ap_float get_adjusted_cost(const GlobalOperator &op) const;
    bool violates_global_constraint(const GlobalState &state);
public:
    SearchEngine(const options::Options &opts);
    virtual ~SearchEngine();
    virtual void print_statistics() const;
    virtual void save_plan_if_necessary() const;
    bool found_solution() const;
    SearchStatus get_status() const;
    const Plan &get_plan() const;
    void search();
    const SearchStatistics &get_statistics() const {return statistics; }
    void set_bound(int b) {bound = b; }
    int get_bound() {return bound; }
    static void add_options_to_parser(options::OptionParser &parser);
};

/*
  Print heuristic values of all heuristics evaluated in the evaluation context.
*/
void print_initial_h_values(const EvaluationContext &eval_context);
std::vector<ap_float> get_initial_h_values(const EvaluationContext &eval_context);

#endif
