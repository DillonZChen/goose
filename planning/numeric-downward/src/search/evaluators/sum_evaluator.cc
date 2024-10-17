#include "sum_evaluator.h"

#include "../option_parser.h"
#include "../plugin.h"

#include <cassert>
#include <limits>

using namespace std;

namespace sum_evaluator {
SumEvaluator::SumEvaluator(const Options &opts)
    : CombiningEvaluator(opts.get_list<ScalarEvaluator *>("evals")) {
}

SumEvaluator::SumEvaluator(const std::vector<ScalarEvaluator *> &evals)
    : CombiningEvaluator(evals) {
}

SumEvaluator::~SumEvaluator() {
}

ap_float SumEvaluator::combine_values(const vector<ap_float> &values) {
    ap_float result = 0;
    for (int value : values) {
        //assert(value >= 0);
        result += value;
        //assert(result >= 0); // Check against overflow.
    }
    return result;
}



static ScalarEvaluator *_parse(OptionParser &parser) {
    parser.document_synopsis("Sum evaluator",
                             "Calculates the sum of the sub-evaluators.");

    parser.add_list_option<ScalarEvaluator *>("evals",
                                              "at least one scalar evaluator");
    Options opts = parser.parse();

    opts.verify_list_non_empty<ScalarEvaluator *>("evals");

    if (parser.dry_run())
        return 0;
    else
        return new SumEvaluator(opts);
}

static Plugin<ScalarEvaluator> _plugin("sum", _parse);
}
