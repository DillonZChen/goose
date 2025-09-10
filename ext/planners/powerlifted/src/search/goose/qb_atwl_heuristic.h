#ifndef SEARCH_QB_ATWL_HEURISTIC_H_
#define SEARCH_QB_ATWL_HEURISTIC_H_

#include "../heuristics/datalog_transformation_options.h"
#include "qb_heuristic.h"
#include "../novelty/standard_novelty.h"

#include "../action.h"
#include "../options.h"
#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"

#include <map>
#include <memory>


class QbAtWlHeuristic : public QbHeuristic {
protected:
    std::unordered_map<int, planning::Predicate> pwl_index_to_predicate;
    std::shared_ptr<feature_generator::WLFeatures> model;
    std::map<std::pair<int, int>, int> feat_to_lowest_h;

    std::map<int, int> nullary_mapping;
    std::vector<NoveltySet> atom_to_lowest_h;  // atom_to_lowest_h[relation][tuple] = lowest_h

public:
    QbAtWlHeuristic(const Options &opts, const Task &task, std::shared_ptr<Heuristic> heuristic);

    void print_statistics() override;

    int compute_heuristic(const DBState &s, const Task &task) override;
};

#endif  // SEARCH_QB_ATWL_HEURISTIC_H_
