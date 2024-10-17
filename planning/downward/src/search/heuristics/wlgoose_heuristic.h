#ifndef HEURISTICS_WLGOOSE_HEURISTIC_H
#define HEURISTICS_WLGOOSE_HEURISTIC_H

#include "../heuristic.h"

#include "../ext/wlplan/include/feature_generation/feature_generator_loader.hpp"
#include "../ext/wlplan/include/planning/atom.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "../ext/wlplan/include/planning/problem.hpp"
#include "../ext/wlplan/include/planning/state.hpp"

#include <memory>

namespace wlgoose_heuristic {
  std::pair<std::string, std::vector<std::string>> fd_fact_to_pred_args(std::string &name);

  class WlGooseHeuristic : public Heuristic {
   protected:
    std::shared_ptr<feature_generation::Features> model;

    std::map<FactPair, std::shared_ptr<planning::Atom>> fd_fact_to_wlplan_atom;

    virtual int compute_heuristic(const State &ancestor_state) override;

   public:
    explicit WlGooseHeuristic(const plugins::Options &opts);
  };
}  // namespace wlgoose_heuristic

#endif
