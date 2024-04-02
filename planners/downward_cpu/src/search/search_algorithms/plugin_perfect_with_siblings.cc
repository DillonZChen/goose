#include "perfect_with_siblings.h"
#include "search_common.h"

#include "../plugins/plugin.h"

using namespace std;

namespace plugin_perfect {
class PerfectSiblingsFeature : public plugins::TypedFeature<SearchAlgorithm, perfectSiblings::PerfectSiblings> {
public:
    PerfectSiblingsFeature() : TypedFeature("perfect_with_siblings") {
        document_title("perfect with siblings");
        document_synopsis("convert plan to states with siblings");

        add_list_option<shared_ptr<Evaluator>>("evals", "evaluators");
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators", "[]");
        add_option<int>(
            "boost",
            "boost value for preferred operator open lists", "0");
        perfectSiblings::add_options_to_feature(*this);

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

    virtual shared_ptr<perfectSiblings::PerfectSiblings> create_component(const plugins::Options &options, const utils::Context &context) const override {
        plugins::verify_list_non_empty<shared_ptr<Evaluator>>(context, options, "evals");
        plugins::Options options_copy(options);
        options_copy.set("open", search_common::create_greedy_open_list_factory(options_copy));
        options_copy.set("reopen_closed", false);

        return make_shared<perfectSiblings::PerfectSiblings>(options_copy);
    }
};

static plugins::FeaturePlugin<PerfectSiblingsFeature> _plugin;
}
