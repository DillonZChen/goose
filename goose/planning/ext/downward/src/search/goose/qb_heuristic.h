#ifndef GOOSE_QB_HEURISTIC_H
#define GOOSE_QB_HEURISTIC_H

#include "../evaluation_context.h"
#include "../heuristic.h"
#include "../search_statistics.h"
#include "../utils/logging.h"

#include <map>
#include <memory>

namespace qb_heuristic {
  class QbHeuristic : public Heuristic {
   protected:
    std::shared_ptr<Evaluator> base_heuristic;
    utils::LogProxy log;
    SearchStatistics statistics;

    std::map<std::pair<int, int>, int> feat_to_lowest_h;

    virtual int compute_heuristic(const State &ancestor_state) = 0;

   public:
    explicit QbHeuristic(const std::shared_ptr<AbstractTask> &transform,
                         bool cache_estimates,
                         const std::string &description,
                         utils::Verbosity verbosity,
                         const std::shared_ptr<Evaluator> base_heuristic);
  };
}  // namespace qb_heuristic

#endif
