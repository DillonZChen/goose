#ifndef SEARCH_GOOSE_WLGOOSE_HEURISTIC_H_
#define SEARCH_GOOSE_WLGOOSE_HEURISTIC_H_

#include "../heuristics/datalog_transformation_options.h"
#include "../heuristics/heuristic.h"

#include "../action.h"
#include "../options.h"
#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

#include "../ext/wlplan/include/feature_generator/feature_generator_loader.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"

#include <memory>


class WlGooseHeuristic : public Heuristic {
protected:
    std::unordered_map<int, planning::Predicate> pwl_index_to_predicate;
    std::shared_ptr<feature_generator::Features> model;

public:
    WlGooseHeuristic(const Options &opts, const Task &task);

    void print_statistics() override;

    int compute_heuristic(const DBState &s, const Task &task) override;
};

#endif  // SEARCH_GOOSE_WLGOOSE_HEURISTIC_H_
