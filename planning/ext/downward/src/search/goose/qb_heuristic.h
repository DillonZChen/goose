#ifndef GOOSE_QB_HEURISTIC_H
#define GOOSE_QB_HEURISTIC_H

#include "../heuristic.h"

#include <memory>

namespace qb_heuristic {
  class QbHeuristic : public Heuristic {
   protected:
    const std::shared_ptr<Heuristic> original_heuristic;
    virtual int compute_heuristic(const State &ancestor_state) = 0;

   public:
    explicit QbHeuristic(const std::shared_ptr<AbstractTask> &transform,
                         bool cache_estimates,
                         const std::string &description,
                         utils::Verbosity verbosity,
                         const std::shared_ptr<Heuristic> &heuristic);
  };
}  // namespace qb_heuristic

#endif
