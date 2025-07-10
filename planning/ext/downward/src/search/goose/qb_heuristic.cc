#include "qb_heuristic.h"

#include "../plugins/plugin.h"
#include "../utils/logging.h"

#include <iostream>
using namespace std;

namespace qb_heuristic {
  QbHeuristic::QbHeuristic(const std::shared_ptr<AbstractTask> &transform,
                           bool cache_estimates,
                           const std::string &description,
                           utils::Verbosity verbosity,
                           const std::shared_ptr<Heuristic> &heuristic)
      : Heuristic(transform, cache_estimates, description, verbosity),
        original_heuristic(heuristic) {}
}  // namespace goal_count_heuristic
