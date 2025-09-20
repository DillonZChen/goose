#ifndef SEARCH_QB_HEURISTIC_H_
#define SEARCH_QB_HEURISTIC_H_

#include "../heuristics/datalog_transformation_options.h"
#include "../heuristics/heuristic.h"

#include "../action.h"
#include "../options.h"
#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

#include <map>
#include <memory>


using namespace wlplan;


class QbHeuristic : public Heuristic {
protected:
    std::shared_ptr<Heuristic> original_heuristic;
    int cached_heuristic;

public:
    QbHeuristic(const Options &opts, const Task &task, std::shared_ptr<Heuristic> heuristic);

    virtual void print_statistics(){};

    virtual int compute_heuristic(const DBState &s, const Task &task) = 0;

    int return_cached_original_heuristic();
};

#endif  // SEARCH_QB_WL_HEURISTIC_H_
