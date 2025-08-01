#ifndef GOOSE_QB_PNWL_HEURISTIC_H
#define GOOSE_QB_PNWL_HEURISTIC_H

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "qb_heuristic.h"
#include "wl_utils.hpp"

#include <memory>

namespace qb_heuristic {
  class QbPnWlHeuristic : public QbHeuristic {
   protected:
    std::shared_ptr<feature_generator::WLFeatures> model;
    wl_utils::DownwardToWlplanAtomMapper fd_fact_to_wlplan_atom;

    virtual int compute_heuristic(const State &ancestor_state) override;

   public:
    explicit QbPnWlHeuristic(const std::shared_ptr<AbstractTask> &transform,
                             bool cache_estimates,
                             const std::string &description,
                             utils::Verbosity verbosity,
                             const std::shared_ptr<Evaluator> base_heuristic,
                             int wl_iterations,
                             const std::string &graph_representation);
  };
}  // namespace qb_heuristic

#endif
