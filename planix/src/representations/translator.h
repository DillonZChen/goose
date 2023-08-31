#ifndef SRC_REPRESENTATIONS_TRANSLATOR_H_
#define SRC_REPRESENTATIONS_TRANSLATOR_H_

#include <cassert>
#include <functional>
#include <vector>

#include "sasplus.h"

namespace SasPlus {

/**
 * CostTransformer is an std::function (i.e., a better version of a function pointer) that, given a
 * single objective SAS+ action, returns a vector<double> representing the multi-objective cost
 * to be associated to this action. It is used by SasPlus::transformToMultiObjective. See also in
 * examples how to create a CostTransformer.
 */
using CostTransformer = std::function<std::vector<double>(SasPlusAction const&)>;

/**
 * Given a single objective SAS+ problem and a CostTransformer, it returns a new problem that is
 * multi-objective and obtained by applying the CostTransformer to every action in the given
 * problem. All items of the original problem are **copied** so it is safe to modify or delete the
 * original problem
 */
SasPlusMultiObjDetPlanProb transformToMultiObjective(SasPlusDetPlanProb const& single_obj_problem,
                                                     CostTransformer cost_transformer);

}  // namespace SasPlus

#endif  // SRC_REPRESENTATIONS_TRANSLATOR_H_
