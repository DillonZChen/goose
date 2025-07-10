#ifndef GOOSE_QB_PN_HEURISTIC_H
#define GOOSE_QB_PN_HEURISTIC_H

#include "qb_heuristic.h"

#include <memory>

namespace qb_heuristic {
  class QbPnHeuristic : public QbHeuristic {
   protected:
    virtual int compute_heuristic(const State &ancestor_state) override;

   public:
    explicit QbPnHeuristic(const std::shared_ptr<AbstractTask> &transform,
                           bool cache_estimates,
                           const std::string &description,
                           utils::Verbosity verbosity,
                           const std::shared_ptr<Heuristic> &heuristic);
  };
}  // namespace qb_heuristic

#endif
