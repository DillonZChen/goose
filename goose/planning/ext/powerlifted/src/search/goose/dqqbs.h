#ifndef GOOSE_DQQBS_H_
#define GOOSE_DQQBS_H_

#include "../search_engines/search.h"
#include "../search_engines/search_space.h"

template <class PackedStateT>
class DualQueueQBSearch : public SearchBase {
protected:
    SearchSpace<PackedStateT> space;

    int heuristic_layer{};
public:
    using StatePackerT = typename PackedStateT::StatePackerT;

    utils::ExitCode search(const Task &task, SuccessorGenerator &generator, Heuristic &heuristic) override;

    void print_statistics() const override;
};

#endif //GOOSE_DQQBS_H_
