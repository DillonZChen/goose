#ifndef GOOSE_WL_UTILS_H
#define GOOSE_WL_UTILS_H

#include "../ext/wlplan/include/planning/atom.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "../ext/wlplan/include/planning/problem.hpp"
#include "../ext/wlplan/include/planning/state.hpp"
#include "../task_proxy.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wl_utils {
  using PredArgsString = std::pair<std::string, std::vector<std::string>>;

  PredArgsString fd_fact_to_pred_args(std::string &name);
  std::map<FactPair, std::pair<std::string, bool>> get_pddl_facts(FactsProxy facts);

  std::map<FactPair, PredArgsString>
  get_fd_fact_to_pred_args_map(const std::shared_ptr<AbstractTask> task);

  std::pair<std::map<FactPair, std::shared_ptr<planning::Atom>>, planning::Problem>
  construct_wlplan_problem(const planning::Domain &domain,
                           const std::map<FactPair, PredArgsString> &mapper,
                           const TaskProxy &task_proxy);

}  // namespace wl_utils

#endif  // GOOSE_WL_UTILS_H
