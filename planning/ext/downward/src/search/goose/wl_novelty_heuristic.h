#ifndef GOOSE_WL_NOVELTY_HEURISTIC
#define GOOSE_WL_NOVELTY_HEURISTIC

#include "../ext/wlplan/include/feature_generator/feature_generator_loader.hpp"
#include "../ext/wlplan/include/planning/atom.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "../ext/wlplan/include/planning/problem.hpp"
#include "../ext/wlplan/include/planning/state.hpp"
#include "../heuristic.h"
#include "wl_utils.hpp"

#include <memory>
#include <vector>

namespace wl_novelty_heuristic {
  class WlNoveltyHeuristic : public Heuristic {
   protected:
    std::shared_ptr<feature_generator::Features> model;
    wl_utils::DownwardToWlplanAtomMapper fd_fact_to_wlplan_atom;
    virtual int compute_heuristic(const State &ancestor_state) override;

   public:
    WlNoveltyHeuristic(const std::shared_ptr<AbstractTask> &transform,
                       bool cache_estimates,
                       const std::string &description,
                       utils::Verbosity verbosity,
                       const int iterations,
                       const std::string &graph_representation);
  };
}  // namespace wl_novelty_heuristic

#endif
