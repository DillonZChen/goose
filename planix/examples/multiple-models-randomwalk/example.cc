#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "representations/enumerative_problems.h"
#include "representations/strips_problems.h"
#include "representations/sasplus.h"
#include "algs/reachable_states.h"

#include "random_walk.h"

namespace Enum = EnumerativeRepr;

using Enum::EnumerativeSSP;
using Enum::EnumMultiObjDetPlanProb;


/** Builds a small chain-like SSP with 4 states (s0->s1->s2->sG) and two actions, one deterministic
 * and one probabilistic (.9 self-loop / .1 advance). Not all actions are applicable in all states
 */
EnumerativeSSP buildSimpleSSP() {
  using Enum::EnumState;
  using Enum::CostPrMap;

  std::vector<EnumState> state_space{"s0", "s1", "s2", "sG"};
  std::unordered_set<EnumState> goals{"sG"};

  CostPrMap a_det;
  CostPrMap a_prob;

  for (size_t i=0; i < state_space.size()-1; ++i) {
    if (i != 2) {
      a_det.insert({state_space[i],                // state
                     {1+i,                         // cost
                        {{state_space[i+1], 1.0}}  // transitions
                  }});
    }
    if (i != 1) {
      a_prob.insert({state_space[i],           // state
                  {1,                          // cost
                    {{state_space[i],   0.9},  // transitions
                     {state_space[i+1], 0.1}}
                 }});
    }
  }

  EnumerativeSSP essp("s0", state_space, goals, {
      {"a-det", a_det}, {"a-prob", a_prob}
      });
  return essp;
}


/** Builds a small multi-objective deterministic problem with 4 states (s0->s1->s2->sG) and two
 * actions, one cheap-expensive and one expensive-cheap (dim1-dim2 of cost vector). Not all actions
 * are applicable in all states.
 */
EnumMultiObjDetPlanProb buildMultiObjDetProb() {
  using Enum::EnumState;
  using Enum::MultiCostSuccMap;

  std::vector<EnumState> state_space{"s0", "s1", "s2", "sG"};
  std::unordered_set<EnumState> goals{"sG"};

  MultiCostSuccMap a_cheap;
  MultiCostSuccMap a_costly;

  for (size_t i=0; i < state_space.size()-1; ++i) {
    if (i != 2) {
      a_cheap.insert({state_space[i],       // state
                       {{1, 10},            // cost vector
                        state_space[i+1]}   // successor
                     });
    }
    if (i != 1) {
      a_costly.insert({state_space[i],        // state
                        {{2.0*i, 1.0/(i+1)},  // cost vector
                         state_space[i+1]}    // successor
                   });
    }
  }

  EnumMultiObjDetPlanProb edp("s0", state_space, goals, {{"a-cheap", a_cheap},
                                                         {"a-costly", a_costly}});
  return edp;
}


using STRIPS::StripsSSP;

/**
 * Builds an STRIPS-based SSP with n propositions and non-unit cost.
 *
 * Initially all propositions are false and the goal is to make the last proposition true. There are
 * n deterministic actions and the i-th action requires the (i-1)-th proposition to be true and adds
 * the i-th proposition while deleting the (i-1)-th proposition. There is one probabilistic action
 * that deletes all propositions and has (n+1) probabilistic effects each of which with probability
 * 1/(n+1). Each effect adds just one the propositions and the extra effects adds nothing.
 */
StripsSSP buildStripsSSP(size_t n) {
  using STRIPS::StripsState;
  using STRIPS::ProbStripsAction;
  using STRIPS::PrEffect;
  using STRIPS::Mask;

  StripsState s0(n, 0);

  /// Goal is to have the highest bit on
  Mask goal_set = s0;
  goal_set.flip(n-1);

  std::vector<ProbStripsAction> actions;

  PrEffect prob_a;
  Mask del_all(n, 0);
  del_all.set();

  /// Delete all and adds nothing
  prob_a[{del_all, s0}] = 1 / static_cast<double>(n+1);

  StripsState sliding_window(n, 1);
  for (size_t i = 0; i < n; ++i) {
    /// Delete all and set only the i-th bit
    prob_a[{del_all, sliding_window}] = 1 / static_cast<double>(n+1);
    sliding_window <<= 1;
  }
  actions.push_back(ProbStripsAction({"prob-a", 1, s0, prob_a}));

  sliding_window.reset();
  sliding_window.set(0);

  /// det-0 is always applicable
  actions.push_back(ProbStripsAction({"det-0", 5, s0, {{{s0, sliding_window}, 1.0}}}));

  for (size_t i = 1; i < n; ++i) {
    Mask prec = sliding_window;
    Mask del = prec;
    sliding_window <<= 1;
    Mask add = sliding_window;
    PrEffect effs{{{del, add}, 1.0}};
    actions.push_back(ProbStripsAction({"det-" + std::to_string(i), 5, prec, effs}));
  }

  return StripsSSP(s0, goal_set, actions);
}



using MoSasPlusProb = SasPlus::SasPlusMultiObjDetPlanProb;
using SasPlus::VariableDomSize;
/**
 * Builds a SAS+ deterministic multi-objective problem representing a navigation problem in an
 * n-by-m grid.
 *
 * The initial and goal positions are given as parameters as well as 3 vectors
 */
MoSasPlusProb buildGridMoSasPlusProb(VariableDomSize n, VariableDomSize m,
                            std::pair<VariableDomSize, VariableDomSize> s0,
                            std::pair<VariableDomSize, VariableDomSize> goal,
                            std::vector<double> move_slow,
                            std::vector<double> move_normal,
                            std::vector<double> move_fast)
{
  using SasPlus::SasPlusVariable;
  using SasPlus::SasPlusState;
  using SasPlus::SasPlusPartialState;
  using SasPlus::SasPlusMultiCostAction;

  std::vector<SasPlusVariable> variables{{"x", n}, {"y", m}};

  assert(s0.first < n);
  assert(s0.second < m);
  assert(goal.first < n);
  assert(goal.second < m);

  SasPlusState sas_s0{s0.first, s0.second};
  SasPlusPartialState sas_goal{{0, goal.first}, {1, goal.second}};

  std::vector<SasPlusMultiCostAction> actions;
  // Small lambda function to add the 3 speeds of actions
  auto addActions = [&] (std::string name_prefix,
                         SasPlusPartialState prec,
                         SasPlusPartialState eff)
  {
    actions.push_back({name_prefix + "-slow", prec, eff, move_slow});
    actions.push_back({name_prefix + "-normal", prec, eff, move_normal});
    actions.push_back({name_prefix + "-fast", prec, eff, move_fast});
  };


  // Creating the East and West actions
  for (VariableDomSize i = 0; i < n; ++i) {
    SasPlusPartialState prec{{0, i}};
    SasPlusPartialState east_eff{{0, i+1}};
    SasPlusPartialState west_eff{{0, i-1}};
    if (i < n-1) {
      addActions("east-" + std::to_string(i), prec, east_eff);
    }
    if (i > 0) {
      addActions("west-" + std::to_string(i), prec, west_eff);
    }
  }
  // Creating the North and South actions
  for (VariableDomSize i = 0; i < m; ++i) {
    SasPlusPartialState prec{{0, i}};
    SasPlusPartialState north_eff{{1, i+1}};
    SasPlusPartialState south_eff{{1, i-1}};
    if (i < m-1) {
      addActions("north-" + std::to_string(i), prec, north_eff);
    }
    if (i > 0) {
      addActions("south-" + std::to_string(i), prec, south_eff);
    }
  }

  return MoSasPlusProb(variables, sas_s0, sas_goal, actions);
}


int main(int argc, char** argv) {
  srand48(time(NULL));

  using RandomWalk::randomWalkFrom;

  std::cout << "Testing the Enumerative Multi-Objective Deterministic Planning Problem\n";
  auto enum_mo_dp = buildMultiObjDetProb();
  std::vector<double> mo_acc_cost = randomWalkFrom(enum_mo_dp, enum_mo_dp.initialState());
  std::cout << "Total accumulated cost: [";
  for (auto const& c : mo_acc_cost) {
    std::cout << c << ", ";
  }
  std::cout << "]" << std::endl;


  std::cout << "\n\nTesting the Enumerative SSP\n";
  auto enum_ssp = buildSimpleSSP();
  auto reachable_enum_ssp = reachableStatesFrom(enum_ssp, enum_ssp.initialState());
  for (auto const& s : reachable_enum_ssp) {
    std::cout << s << std::endl;
  }
  double ssp_acc_cost = randomWalkFrom(enum_ssp, enum_ssp.initialState());
  std::cout << "Total accumulated cost: " << ssp_acc_cost << std::endl;


  std::cout << "\n\nTesting the STRIPS based SSP\n";
  size_t n = 4;
  std::cout << "-> Using n = " << n << std::endl;
  auto strips_ssp = buildStripsSSP(n);
  auto reachable_strips_ssp = reachableStatesFrom(strips_ssp, strips_ssp.initialState());
  for (auto const& s : reachable_strips_ssp) {
    std::cout << s << std::endl;
  }
  double strips_acc_cost = randomWalkFrom(strips_ssp, strips_ssp.initialState());
  std::cout << "Total accumulated cost: " << strips_acc_cost << std::endl;


  std::cout << "\n\nTesting the Multi-Objective Deterministic SAS+ Problem\n";
  auto mo_sas = buildGridMoSasPlusProb(5, 5, {0,1}, {4,4},
                                     // Time, Fuel, Safety
                                       {   4,    1,     3}, // Slow
                                       {   2,    4,     1}, // Normal
                                       {   1,    8,    10}  // Fast
      );
  std::cout << "The SAS+ problem has " << mo_sas.numberOfVariables() << " variables:\n";
  for (auto const& sas_var : mo_sas.variables()) {
    std::cout << "  " << sas_var.name << " in {0, ..., " << (sas_var.domain - 1) << "}\n";
  }
  std::cout << "The initial state is: " << mo_sas.initialState() << std::endl;
  std::cout << "The goal formula is: " << mo_sas.goal() << std::endl;
  std::cout << "Performing the random walk...\n";

  std::vector<double> mo_sas_accumulated_cost = randomWalkFrom(mo_sas, mo_sas.initialState());
  std::cout << "Total accumulated cost: [";
  for (auto const& c : mo_sas_accumulated_cost) {
    std::cout << c << ", ";
  }
  std::cout << "]" << std::endl;

  return 0;
}
