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
                           const std::shared_ptr<Evaluator> base_heuristic)
      : Heuristic(transform, cache_estimates, description, verbosity),
        base_heuristic(base_heuristic),
        log(utils::get_log_for_verbosity(verbosity)),
        statistics(log) {}
}  // namespace qb_heuristic
