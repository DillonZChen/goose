#ifndef GOOSE_QB_WL_HEURISTIC_H
#define GOOSE_QB_WL_HEURISTIC_H

#include "qb_heuristic.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "wl_utils.hpp"

#include <memory>

namespace qb_heuristic {
  class QbWlHeuristic : public QbHeuristic {
   protected:
    std::shared_ptr<feature_generator::WLFeatures> model;
    wl_utils::DownwardToWlplanAtomMapper fd_fact_to_wlplan_atom;
    std::map<std::pair<int, int>, int> feat_to_lowest_h;

    virtual int compute_heuristic(const State &ancestor_state) override;

   public:
    explicit QbWlHeuristic(const std::shared_ptr<AbstractTask> &transform,
                           bool cache_estimates,
                           const std::string &description,
                           utils::Verbosity verbosity,
                           const std::shared_ptr<Heuristic> &heuristic);
  };
}  // namespace qb_heuristic

#endif
