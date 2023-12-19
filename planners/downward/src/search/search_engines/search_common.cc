#include "search_common.h"

#include "../open_list_factory.h"

#include "../plugins/options.h"
#include "../open_lists/alternation_open_list.h"
#include "../open_lists/best_first_open_list.h"

#include <memory>

using namespace std;

namespace search_common {

shared_ptr<OpenListFactory> create_standard_scalar_open_list_factory(
    const shared_ptr<Evaluator> &eval, bool pref_only) {
    plugins::Options options;
    options.set("eval", eval);
    options.set("pref_only", pref_only);
    return make_shared<standard_scalar_open_list::BestFirstOpenListFactory>(options);
}

static shared_ptr<OpenListFactory> create_alternation_open_list_factory(
    const vector<shared_ptr<OpenListFactory>> &subfactories, int boost) {
    plugins::Options options;
    options.set("sublists", subfactories);
    options.set("boost", boost);
    return make_shared<alternation_open_list::AlternationOpenListFactory>(options);
}

shared_ptr<OpenListFactory> create_greedy_open_list_factory(
    const plugins::Options &options) {
    const vector<shared_ptr<Evaluator>> &evals = options.get_list<shared_ptr<Evaluator>>("evals");
    const vector<shared_ptr<Evaluator>> &preferred_evaluators = options.get_list<shared_ptr<Evaluator>>("preferred");
    int boost = options.get<int>("boost");
    if (evals.size() == 1 && preferred_evaluators.empty()) {
        return create_standard_scalar_open_list_factory(evals[0], false);
    } else {
        vector<shared_ptr<OpenListFactory>> subfactories;
        for (const shared_ptr<Evaluator> &evaluator : evals) {
            subfactories.push_back(
                create_standard_scalar_open_list_factory(
                    evaluator, false));
            if (!preferred_evaluators.empty()) {
                subfactories.push_back(
                    create_standard_scalar_open_list_factory(
                        evaluator, true));
            }
        }
        return create_alternation_open_list_factory(subfactories, boost);
    }
}
}
