#ifndef SEARCH_ENGINES_BATCH_EAGER_SEARCH_H
#define SEARCH_ENGINES_BATCH_EAGER_SEARCH_H

#include "../open_list.h"
#include "../search_engine.h"

#include <memory>
#include <vector>

class Evaluator;
class PruningMethod;

namespace plugins {
class Feature;
}

namespace batch_eager_search {
class BatchEagerSearch : public SearchEngine {
    const bool reopen_closed_nodes;

    std::unique_ptr<StateOpenList> open_list;

    std::vector<Evaluator *> path_dependent_evaluators;

    std::shared_ptr<Evaluator> heuristic;

    int best_h;

    void start_f_value_statistics(EvaluationContext &eval_context);
    void update_f_value_statistics(EvaluationContext &eval_context);
    void reward_progress();

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit BatchEagerSearch(const plugins::Options &opts);
    virtual ~BatchEagerSearch() = default;

    virtual void print_statistics() const override;

    void dump_search_space() const;
};

extern void add_options_to_feature(plugins::Feature &feature);
}

#endif
