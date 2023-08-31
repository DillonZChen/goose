#include "translator.h"

namespace SasPlus {

SasPlusMultiObjDetPlanProb transformToMultiObjective(SasPlusDetPlanProb const& single_obj_problem,
                                                     CostTransformer cost_transformer)
{
  std::vector<SasPlusMultiCostAction> mo_actions;
  size_t num_cost_func = 0;
  for (auto const& so_action : single_obj_problem.allActions()) {
    std::vector<double> mo_cost = cost_transformer(so_action);
    if (num_cost_func == 0) {
      num_cost_func = mo_cost.size();
    }
    assert(mo_cost.size() > 0);
    assert(mo_cost.size() == num_cost_func);
    mo_actions.push_back({so_action.name(), so_action.precondition(), so_action.effect(), mo_cost});
  }

  return SasPlusMultiObjDetPlanProb(single_obj_problem.variables(),
                                    single_obj_problem.initialState(),
                                    single_obj_problem.goal(),
                                    mo_actions);
}

}  // namespace SasPlus
