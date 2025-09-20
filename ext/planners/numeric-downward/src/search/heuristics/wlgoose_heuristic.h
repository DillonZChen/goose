#ifndef HEURISTICS_WLGOOSE_HEURISTIC_H
#define HEURISTICS_WLGOOSE_HEURISTIC_H

#include "../ext/wlplan/include/feature_generator/feature_generator_loader.hpp"
#include "../ext/wlplan/include/planning/atom.hpp"
#include "../ext/wlplan/include/planning/function.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "../ext/wlplan/include/planning/problem.hpp"
#include "../ext/wlplan/include/planning/state.hpp"
#include "../heuristic.h"

#include <memory>
#include <pybind11/embed.h>

namespace py = pybind11;
using namespace wlplan;

namespace wlgoose_heuristic {
  std::pair<std::string, std::vector<std::string>> fd_fact_to_pred_args(std::string &name);

  class WlGooseHeuristic : public Heuristic {
   protected:
    py::scoped_interpreter guard{};

    std::shared_ptr<feature_generator::Features> model;
    std::vector<std::vector<std::shared_ptr<planning::Atom>>> fdr_pair_to_wlplan_atom;

    // some fluent values are static
    std::vector<double> values;
    // the indices of fluents in NFD state registry (as opposed to derived fluents)
    std::vector<int> nfd_fluent_indices;
    // map nfd fluent indices (whose length is equal to nfd_fluent_indices) to wlplan nfd index
    std::vector<int> nfd_to_wlplan_fluent_index;

    planning::State nfd_to_wlplan_state(const GlobalState &global_state);

    ap_float compute_heuristic(const GlobalState &global_state) override;

   public:
    explicit WlGooseHeuristic(const options::Options &opts);
  };
}  // namespace wlgoose_heuristic

#endif
