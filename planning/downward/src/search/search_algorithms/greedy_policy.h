#ifndef SEARCH_ALGORITHMS_GREEDY_POLICY_H
#define SEARCH_ALGORITHMS_GREEDY_POLICY_H

#include "../open_list.h"
#include "../search_algorithm.h"

#include <memory>
#include <random>
#include <vector>

class Evaluator;
class PruningMethod;
class OpenListFactory;

namespace plugins {
class Feature;
}

namespace greedy_policy {
class GreedyPolicy : public SearchAlgorithm {
    const bool reopen_closed_nodes;

    std::unique_ptr<StateOpenList> open_list;
    std::shared_ptr<Evaluator> evaluator;
    std::set<Evaluator *> path_dependent_evaluators;

    EvaluationContext current_eval_context;

    std::mt19937 gen;

    std::shared_ptr<SearchNode> node;

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit GreedyPolicy(
        const std::shared_ptr<Evaluator> &h, const int s,
        OperatorCost cost_type, int bound, double max_time,
        const std::string &description, utils::Verbosity verbosity);

    virtual void print_statistics() const override;
};
}

#endif
