#ifndef SEARCH_QB_AT_HEURISTIC_H_
#define SEARCH_QB_AT_HEURISTIC_H_

#include "../heuristics/datalog_transformation_options.h"
#include "../novelty/standard_novelty.h"
#include "qb_heuristic.h"

#include "../action.h"
#include "../options.h"
#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

#include <map>
#include <memory>


using namespace wlplan;

class QbAtHeuristic : public QbHeuristic {
protected:
    std::map<int, int> nullary_mapping;
    std::vector<NoveltySet> atom_to_lowest_h;  // atom_to_lowest_h[relation][tuple] = lowest_h

public:
    QbAtHeuristic(const Options &opts, const Task &task, std::shared_ptr<Heuristic> heuristic);

    void print_statistics() override;

    int compute_heuristic(const DBState &s, const Task &task) override;
};

#endif  // SEARCH_QB_AT_HEURISTIC_H_
