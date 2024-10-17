#ifndef EVALUATORS_SUM_EVALUATOR_H
#define EVALUATORS_SUM_EVALUATOR_H

#include "combining_evaluator.h"

#include <vector>

namespace options {
class Options;
}

namespace sum_evaluator {
class SumEvaluator : public combining_evaluator::CombiningEvaluator {
protected:
    virtual ap_float combine_values(const std::vector<ap_float> &values) override;
public:
    explicit SumEvaluator(const options::Options &opts);
    explicit SumEvaluator(const std::vector<ScalarEvaluator *> &evals);
    virtual ~SumEvaluator() override;
};
}

#endif
