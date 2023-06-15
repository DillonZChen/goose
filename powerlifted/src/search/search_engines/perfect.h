//
// Created by dillon on 9/04/23.
//

#ifndef SEARCH_PREFECT_H
#define SEARCH_PREFECT_H

#include "search.h"
#include "search_space.h"

template <class PackedStateT>
class PerfectSearch : public SearchBase {
protected:
    SearchSpace<PackedStateT> space;

    int heuristic_layer{};
public:
    using StatePackerT = typename PackedStateT::StatePackerT;

    utils::ExitCode search(const Task &task, SuccessorGenerator &generator, Heuristic &heuristic) override;

    void print_statistics() const override;
};


#endif  // SEARCH_PREFECT_H
