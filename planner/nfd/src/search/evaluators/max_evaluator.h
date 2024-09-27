#ifndef EVALUATORS_MAX_EVALUATOR_H
#define EVALUATORS_MAX_EVALUATOR_H

#include "combining_evaluator.h"

#include <vector>

namespace options {
class Options;
}

namespace max_evaluator {
class MaxEvaluator : public combining_evaluator::CombiningEvaluator {
protected:
    virtual ap_float combine_values(const std::vector<ap_float> &values) override;

public:
    explicit MaxEvaluator(const options::Options &opts);
    virtual ~MaxEvaluator() override;
};
}

#endif
