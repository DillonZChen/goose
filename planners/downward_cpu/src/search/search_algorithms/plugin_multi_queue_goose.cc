#include "multi_queue_goose.h"
#include "search_common.h"

#include "../plugins/plugin.h"

using namespace std;

namespace plugin_eager_greedy {
class MultiQueueGooseFeature : public plugins::TypedFeature<SearchAlgorithm, multi_queue_goose::MultiQueueGoose> {
public:
    MultiQueueGooseFeature() : TypedFeature("mq_goose") {
        document_title("Multi queue goose for WL");
        document_synopsis("");

        add_list_option<shared_ptr<Evaluator>>("evals", "evaluators");
        add_option<bool>("symmetry", "use wl features as symmetry breaking", "false");
        multi_queue_goose::add_options_to_feature(*this);

    }

    virtual shared_ptr<multi_queue_goose::MultiQueueGoose> create_component(const plugins::Options &options, const utils::Context &context) const override {
        plugins::verify_list_non_empty<shared_ptr<Evaluator>>(context, options, "evals");
        plugins::Options options_copy(options);
        return make_shared<multi_queue_goose::MultiQueueGoose>(options_copy);
    }
};

static plugins::FeaturePlugin<MultiQueueGooseFeature> _plugin;
}
