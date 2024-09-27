#ifndef HEURISTICS_BLIND_SEARCH_HEURISTIC_H
#define HEURISTICS_BLIND_SEARCH_HEURISTIC_H

#include "../heuristic.h"

namespace blind_search_heuristic {
class BlindSearchHeuristic : public Heuristic {
    ap_float min_operator_cost;
protected:
    virtual void initialize();
    virtual ap_float compute_heuristic(const GlobalState &global_state);
public:
    BlindSearchHeuristic(const options::Options &options);
    ~BlindSearchHeuristic();
};
}

#endif
