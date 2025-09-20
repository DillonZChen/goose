#ifndef SEARCH_GOOSE_WL_UTILS_H_
#define SEARCH_GOOSE_WL_UTILS_H_

#include "../task.h"

#include "../ext/wlplan/include/feature_generator/feature_generator_loader.hpp"
#include "../ext/wlplan/include/planning/atom.hpp"
#include "../ext/wlplan/include/planning/predicate.hpp"
#include "../ext/wlplan/include/planning/problem.hpp"
#include "../ext/wlplan/include/planning/state.hpp"

#include <memory>


using namespace wlplan;

namespace wl_utils {
planning::Domain get_wlplan_domain(const Task &task);
planning::Problem get_wlplan_problem(const planning::Domain &domain, const Task &task);
std::unordered_map<int, planning::Predicate>
get_pwl_index_to_predicate(const planning::Domain &domain, const Task &task);
planning::State
to_wlplan_state(const DBState &s,
                const Task &task,
                const std::unordered_map<int, planning::Predicate> &pwl_index_to_predicate);
}  // namespace wl_utils

#endif  // SEARCH_GOOSE_WL_UTILS_H_
