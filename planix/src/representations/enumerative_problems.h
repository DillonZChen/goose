#ifndef SRC_REPRESENTATIONS_ENUMERATIVE_PROBLEMS_H_
#define SRC_REPRESENTATIONS_ENUMERATIVE_PROBLEMS_H_

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <utility>
#include <string>

#include "models/container_templates.h"


/** 
 * Namespace for all the models and data-structures to represent non-factored models.
 *
 * The focus is on simplicity and easy to debug and not efficiency.
 */
namespace EnumerativeRepr {

/*
 * TODO(fwt): remove code repetition. I'm still weighting the options but right now it seems the
 * only meaning alternative is a inheritance approach.
 */

using EnumState = std::string;


/*************************************************************************************************
                               Single Objective Deterministic Problem
 ************************************************************************************************/
using CostState = std::pair<double, EnumState>;
using CostStateMap = std::unordered_map<EnumState, CostState>;

/**
 * Class representing a non-factored single-cost deterministic action
 */
class EnumDetAction {
 public:
  EnumDetAction(std::string const& name, CostStateMap const& cost_and_successor)
    : name_(name), cost_and_successor_(cost_and_successor)
  { }

  bool isApplicable(EnumState const& s) const {
    return cost_and_successor_.find(s) != cost_and_successor_.end();
  }

  double cost(EnumState const& s) const {
    auto const it = cost_and_successor_.find(s);
    assert(it != cost_and_successor_.end());
    return it->second.first;
  }

  EnumState successor(EnumState const& s) const {
    auto const it = cost_and_successor_.find(s);
    assert(it != cost_and_successor_.end());
    return it->second.second;
  }

  std::string const& name() const { return name_; }

 private:
  std::string name_;
  CostStateMap cost_and_successor_;
};


/**
 * Class representing a non-factored single-objective deterministic planning problem
 */
class EnumDetPlanProb {
 public:
  using State = EnumState;
  using Action = EnumDetAction;

  EnumDetPlanProb(State const& s0, std::vector<State> const& states,
                         std::unordered_set<State> const& goal_set,
                         std::vector<Action> const& actions)
    : s0_(s0), states_(states), goal_set_(goal_set), actions_(actions)
  { }

  State initialState() const { return s0_; }

  std::vector<Action> allActions() const { return actions_; }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  double cost(State const& s, Action const& a) const {
    return a.cost(s);
  }

  State successor(State const& s, Action const& a) const {
    return a.successor(s);
  }

  bool isGoal(State const& s) const {
    return goal_set_.find(s) != goal_set_.end();
  }

 private:
  State s0_;
  std::vector<State> states_;
  std::unordered_set<State> goal_set_;
  std::vector<Action> actions_;
};


/*************************************************************************************************
                            Single Multi-Objective Deterministic Problem
 ************************************************************************************************/
using VecCostState = std::pair<std::vector<double>, EnumState>;
using MultiCostSuccMap = std::unordered_map<EnumState, VecCostState>;


/**
 * Class representing a non-factored multiple-cost deterministic action
 */
class EnumMultiCostDetAction {
 public:
  EnumMultiCostDetAction(std::string const& name, MultiCostSuccMap const& cost_and_successor)
    : name_(name), cost_and_successor_(cost_and_successor)
  { }

  bool isApplicable(EnumState const& s) const {
    return cost_and_successor_.find(s) != cost_and_successor_.end();
  }

  size_t numCostFunctions() const {
    auto it = cost_and_successor_.begin();
    if (it != cost_and_successor_.end()) {
      return it->second.first.size();
    }
    return 0;
  }

  std::vector<double> const& cost(EnumState const& s) const {
    auto const it = cost_and_successor_.find(s);
    assert(it != cost_and_successor_.end());
    return it->second.first;
  }

  EnumState successor(EnumState const& s) const {
    auto const it = cost_and_successor_.find(s);
    assert(it != cost_and_successor_.end());
    return it->second.second;
  }

  std::string const& name() const { return name_; }

 private:
  std::string name_;
  MultiCostSuccMap cost_and_successor_;
};


/**
 * Class representing a non-factored multi-objective deterministic planning problem.
 *
 * This class assumes that all objectives should be minimized to avoid conflicts with the goal
 * reachability.
 */
class EnumMultiObjDetPlanProb {
 public:
  using State = EnumState;
  using Action = EnumMultiCostDetAction;

  EnumMultiObjDetPlanProb(State const& s0, std::vector<State> const& states,
                          std::unordered_set<State> const& goal_set,
                          std::vector<Action> const& actions)
    : s0_(s0), states_(states), goal_set_(goal_set), actions_(actions), num_cost_func_(0)
  {
    if (actions.size() > 0) {
      auto it = actions.begin();
      num_cost_func_ = it->numCostFunctions();
    }
  }

  State initialState() const { return s0_; }

  std::vector<Action> allActions() const { return actions_; }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  size_t numCostFunctions() const {
    return num_cost_func_;
  }

  std::vector<double> cost(State const& s, Action const& a) const {
    auto cost_vec = a.cost(s);
    assert(cost_vec.size() == numCostFunctions());
    return cost_vec;
  }

  State successor(State const& s, Action const& a) const {
    return a.successor(s);
  }

  bool isGoal(State const& s) const {
    return goal_set_.find(s) != goal_set_.end();
  }

 private:
  State s0_;
  std::vector<State> states_;
  std::unordered_set<State> goal_set_;
  std::vector<Action> actions_;
  size_t num_cost_func_;
};


/*************************************************************************************************
                                 Stochastic Shortest Path Problems
 ************************************************************************************************/
using PrState = ::ProbDist<EnumState>;
using CostPrMap = std::unordered_map<EnumState, std::pair<double, PrState>>;


/**
 * Class representing a non-factored single-cost probabilistic action
 */
class EnumPrAction {
 public:
  EnumPrAction(std::string const& name, CostPrMap const& cost_and_pr_matrix)
    : name_(name), cost_and_pr_matrix_(cost_and_pr_matrix)
  { }

  bool isApplicable(EnumState const& s) const {
    return cost_and_pr_matrix_.find(s) != cost_and_pr_matrix_.end();
  }

  double cost(EnumState const& s) const {
    auto const it = cost_and_pr_matrix_.find(s);
    assert(it != cost_and_pr_matrix_.end());
    return it->second.first;
  }

  PrState successors(EnumState const& s) const {
    auto const it = cost_and_pr_matrix_.find(s);
    assert(it != cost_and_pr_matrix_.end());
    return it->second.second;
  }

  std::string const& name() const { return name_; }

 private:
  std::string name_;
  CostPrMap cost_and_pr_matrix_;
};


/**
 * Class representing a non-factored SSP, i.e., single-objective probabilistic planning problem.
 */
class EnumerativeSSP {
 public:
  using State = EnumState;
  using Action = EnumPrAction;

  EnumerativeSSP(State const& s0,
                 std::vector<State> const& states,
                 std::unordered_set<State> const& goal_set,
                 std::vector<Action> const& actions)
    : s0_(s0), states_(states), goal_set_(goal_set), actions_(actions)
  { }

  State initialState() const { return s0_; }

  std::vector<Action> allActions() const { return actions_; }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  double cost(State const& s, Action const& a) const {
    return a.cost(s);
  }

  PrState successors(State const& s, Action const& a) const {
    return a.successors(s);
  }

  bool isGoal(State const& s) const {
    return goal_set_.find(s) != goal_set_.end();
  }

 private:
  State s0_;
  std::vector<State> states_;
  std::unordered_set<State> goal_set_;
  std::vector<Action> actions_;
};

}  // namespace EnumerativeRepr

#endif  // SRC_REPRESENTATIONS_ENUMERATIVE_PROBLEMS_H_
