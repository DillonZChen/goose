#include "qb_heuristic.h"

#include <cassert>

using namespace std;

QbHeuristic::QbHeuristic(const Options &opts,
                             const Task &task,
                             std::shared_ptr<Heuristic> heuristic)
    : original_heuristic(heuristic)
{}

int QbHeuristic::return_cached_original_heuristic() { return cached_heuristic; }
