#ifndef HEURISTICS_LM_CUT_NUMERIC_HEURISTIC_H
#define HEURISTICS_LM_CUT_NUMERIC_HEURISTIC_H

#include "numeric_lm_cut_landmarks.h"

#include "../heuristic.h"

#include <memory>

class GlobalState;

namespace options {
    class Options;
}

namespace lm_cut_numeric_heuristic {
    class LandmarkCutNumericHeuristic : public Heuristic {
        std::unique_ptr<numeric_lm_cut_heuristic::LandmarkCutLandmarks> landmark_generator;
        //std::unique_ptr<lm_cut_repetition_heuristic::LandmarkCutLandmarks> landmark_generator;
        //std::unique_ptr<LandmarkCutNumericLandmarks> landmark_generator;
        bool ceiling_less_than_one;
        bool ignore_numeric;
        bool use_random_pcf;
        bool use_irmax;
        bool disable_ma;
        bool use_second_order_simple;
        bool use_constant_assignment;
        int bound_iterations;
        ap_float precision;
        ap_float epsilon;
        virtual void initialize() override;
        virtual ap_float compute_heuristic(const GlobalState &global_state) override;
        ap_float compute_heuristic(const State &state);
    public:
        explicit LandmarkCutNumericHeuristic(const options::Options &opts);
        virtual ~LandmarkCutNumericHeuristic() override;
    };
}

#endif
