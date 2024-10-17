#include "combining_evaluator.h"

#include "../globals.h"
#include "../evaluation_context.h"
#include "../evaluation_result.h"
#include <limits>

using namespace std;

namespace combining_evaluator {
CombiningEvaluator::CombiningEvaluator(
    const vector<ScalarEvaluator *> &subevaluators_)
    : subevaluators(subevaluators_) {
    all_dead_ends_are_reliable = true;
    for (const ScalarEvaluator *subevaluator : subevaluators)
        if (!subevaluator->dead_ends_are_reliable())
            all_dead_ends_are_reliable = false;
}

CombiningEvaluator::~CombiningEvaluator() {
}

bool CombiningEvaluator::dead_ends_are_reliable() const {
    return all_dead_ends_are_reliable;
}

EvaluationResult CombiningEvaluator::compute_result(
    EvaluationContext &eval_context) {
    // This marks no preferred operators.
    EvaluationResult result;
    vector<ap_float> values;
    values.reserve(subevaluators.size());

    // Collect component values. Return infinity if any is infinite.
    for (ScalarEvaluator *subevaluator : subevaluators) {
        ap_float h_val = eval_context.get_heuristic_value_or_infinity(subevaluator);
        if (h_val == EvaluationResult::INFTY) {
            result.set_h_value(h_val);
            return result;
        } else {
            values.push_back(h_val);
        }
    }

    // If we arrived here, all subevaluator values are finite.
    result.set_h_value(combine_values(values));
    return result;
}

void CombiningEvaluator::get_involved_heuristics(std::set<Heuristic *> &hset) {
    for (size_t i = 0; i < subevaluators.size(); ++i)
        subevaluators[i]->get_involved_heuristics(hset);
}
}
