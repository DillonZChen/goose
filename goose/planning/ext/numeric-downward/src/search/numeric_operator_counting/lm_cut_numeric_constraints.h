#ifndef OPERATOR_COUNTING_LM_CUT_NUMERIC_CONSTRAINTS_H
#define OPERATOR_COUNTING_LM_CUT_NUMERIC_CONSTRAINTS_H

#include "../operator_counting/constraint_generator.h"
#include "../numeric_landmarks/numeric_lm_cut_landmarks.h"

#include <memory>

namespace lm_cut_numeric_heuristic {
class LandmarkCutNumericLandmarks;
}

namespace operator_counting {
class LMCutNumericConstraints : public ConstraintGenerator {
    bool ceiling_less_than_one;
    bool ignore_numeric;
    bool use_random_pcf;
    bool use_irmax;
    bool disable_ma;
    bool use_second_order_simple;
    bool use_constant_assignment;
    bool use_bounds;
    ap_float precision;
    ap_float epsilon;
    std::unique_ptr<numeric_lm_cut_heuristic::LandmarkCutLandmarks> landmark_generator;
public:
    LMCutNumericConstraints(const Options &opts);
    virtual void initialize_constraints(
        const std::shared_ptr<AbstractTask> task,
        std::vector<lp::LPConstraint> &constraints,
        double infinity) override;
    virtual bool update_constraints(const State &state,
                                    lp::LPSolver &lp_solver) override;
};
}

#endif
