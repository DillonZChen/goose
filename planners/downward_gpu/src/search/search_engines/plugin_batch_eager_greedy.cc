#include "batch_eager_search.h"
#include "search_common.h"

#include "../plugins/plugin.h"

using namespace std;

namespace plugin_batch_eager_greedy {
class BatchEagerGreedySearchFeature : public plugins::TypedFeature<SearchEngine, batch_eager_search::BatchEagerSearch> {
public:
    BatchEagerGreedySearchFeature() : TypedFeature("batch_eager_greedy") {
        document_title("Batched greedy search (eager)");
        document_synopsis("");

        add_list_option<shared_ptr<Evaluator>>("evals", "evaluators");
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators", "[]");
        add_option<int>(
            "boost",
            "boost value for preferred operator open lists", "0");
        batch_eager_search::add_options_to_feature(*this);

        document_note(
            "Open list",
            "In most cases, eager greedy best first search uses "
            "an alternation open list with one queue for each evaluator. "
            "If preferred operator evaluators are used, it adds an extra queue "
            "for each of these evaluators that includes only the nodes that "
            "are generated with a preferred operator. "
            "If only one evaluator and no preferred operator evaluator is used, "
            "the search does not use an alternation open list but a "
            "standard open list with only one queue.");
        document_note(
            "Closed nodes",
            "Closed node are not re-opened");
        document_note(
            "Preferred operators",
            "Not implemented yet");
    }

    virtual shared_ptr<batch_eager_search::BatchEagerSearch> create_component(const plugins::Options &options, const utils::Context &context) const override {
        plugins::verify_list_non_empty<shared_ptr<Evaluator>>(context, options, "evals");
        plugins::Options options_copy(options);
        options_copy.set("open", search_common::create_greedy_open_list_factory(options_copy));
        options_copy.set("reopen_closed", false);

        return make_shared<batch_eager_search::BatchEagerSearch>(options_copy);
    }
};

static plugins::FeaturePlugin<BatchEagerGreedySearchFeature> _plugin;
}
