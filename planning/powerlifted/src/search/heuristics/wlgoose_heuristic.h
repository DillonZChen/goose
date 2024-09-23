#ifndef SEARCH_HEURISTICS_WLGOOSE_HEURISTIC_H_
#define SEARCH_HEURISTICS_WLGOOSE_HEURISTIC_H_

#include "datalog_transformation_options.h"
#include "heuristic.h"

#include "../action.h"
#include "../options.h"
#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

#include "../ext/wlplan/include/feature_generation/wl_features.hpp"
#include "../ext/wlplan/include/planning/atom.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "../ext/wlplan/include/planning/problem.hpp"
#include "../ext/wlplan/include/planning/state.hpp"

#include <memory>


class WlGooseHeuristic : public Heuristic {
protected:
    std::unordered_map<int, planning::Predicate> pwl_index_to_predicate;
    std::shared_ptr<feature_generation::WLFeatures> model;

public:
    WlGooseHeuristic(const Options &opts, const Task &task);

    void print_statistics() override;

    int compute_heuristic(const DBState &s, const Task &task) override;
};

#endif  // SEARCH_HEURISTICS_WLGOOSE_HEURISTIC_H_
