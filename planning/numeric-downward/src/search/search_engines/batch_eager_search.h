#ifndef SEARCH_ENGINES_BATCH_EAGER_SEARCH_H
#define SEARCH_ENGINES_BATCH_EAGER_SEARCH_H

#include "../search_engine.h"

#include "../open_lists/open_list.h"

#include <memory>
#include <vector>

class GlobalOperator;
class Heuristic;
class PruningMethod;
class ScalarEvaluator;

namespace options {
class Options;
}

namespace batch_eager_search {
class BatchEagerSearch : public SearchEngine {
    std::unique_ptr<StateOpenList> open_list;

    ScalarEvaluator *heuristic;

    ap_float best_h;

    std::pair<SearchNode, bool> fetch_next_node();
    void print_checkpoint_line(int g) const;

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;
    virtual void save_plan_if_necessary() const override;

public:
    explicit BatchEagerSearch(const options::Options &opts);
    virtual ~BatchEagerSearch() = default;

    virtual void print_statistics() const override;

    void dump_search_space() const;
};
}

#endif
