#ifndef SEARCH_QB_PNWL_HEURISTIC_H_
#define SEARCH_QB_PNWL_HEURISTIC_H_

#include "../heuristics/datalog_transformation_options.h"
#include "qb_heuristic.h"
#include "../novelty/standard_novelty.h"

#include "../action.h"
#include "../options.h"
#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

#include "../ext/wlplan/include/feature_generator/feature_generator_loader.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"

#include <map>
#include <memory>


class QbPnWlHeuristic : public QbHeuristic {
protected:
    std::unordered_map<int, planning::Predicate> pwl_index_to_predicate;
    std::shared_ptr<feature_generator::Features> model;
    std::map<std::pair<int, int>, int> feat_to_lowest_h;

    std::map<int, int> nullary_mapping;
    std::vector<NoveltySet> atom_to_lowest_h;  // atom_to_lowest_h[relation][tuple] = lowest_h

public:
    QbPnWlHeuristic(const Options &opts, const Task &task, std::shared_ptr<Heuristic> heuristic);

    void print_statistics() override;

    int compute_heuristic(const DBState &s, const Task &task) override;
};

#endif  // SEARCH_QB_PNWL_HEURISTIC_H_
