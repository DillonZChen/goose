#include "max_evaluator.h"

#include "../option_parser.h"
#include "../plugin.h"

#include <cassert>

using namespace std;

namespace max_evaluator {
MaxEvaluator::MaxEvaluator(const Options &opts)
    : CombiningEvaluator(opts.get_list<ScalarEvaluator *>("evals")) {
}

MaxEvaluator::~MaxEvaluator() {
}

ap_float MaxEvaluator::combine_values(const vector<ap_float> &values) {
    //ap_float result = values[1];
    static int count = 0;
    //cout << "count " << count << endl;
    if (values[1] < values[0]) cout << "here " << count << " " << values[0] << " " << values[1] << endl;
//    for (int value : values) {
//        assert(value >= 0);
//        if (value < result) cout << "here " << value << " " << result << endl;
//        result = max(result, value);
//    }
    count++;
    return values[0];
}

static ScalarEvaluator *_parse(OptionParser &parser) {
    parser.document_synopsis(
        "Max evaluator",
        "Calculates the maximum of the sub-evaluators.");
    parser.add_list_option<ScalarEvaluator *>(
        "evals",
        "at least one scalar evaluator");

    Options opts = parser.parse();

    opts.verify_list_non_empty<ScalarEvaluator *>("evals");

    if (parser.dry_run()) {
        return nullptr;
    }
    return new MaxEvaluator(opts);
}

static Plugin<ScalarEvaluator> plugin("max", _parse);
}
