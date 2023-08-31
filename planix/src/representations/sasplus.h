#ifndef SRC_REPRESENTATIONS_SASPLUS_H_
#define SRC_REPRESENTATIONS_SASPLUS_H_

#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//#include "models/container_templates.h"


/**
 * Namespace for all the models and data-structures to represent SAS+ models.
 *
 * The focus of this implementation is a trade-off of simplicity and efficiency instead of absolute
 * efficiency
 */
namespace SasPlus {

/// Type to represent the size of the domain of a variable as well as the maximum value of a
/// variable (max_size - 1). Notice that uint8_t should also work but the memory savings is minimal
/// due to the / overhead of std::vector and std::map/unordered_map
using VariableDomSize = uint16_t;


/// A SAS+ variables is represented by its size of s domain (its possible values are
/// {0, ..., domain-size - 1}) and their name.
struct SasPlusVariable {
  std::string name;
  VariableDomSize domain;
};


/// The i-th position containts the value of the SAS+ variable v_i
using SasPlusState = std::vector<VariableDomSize>;


/// Map from i to a domain value representing the value of the SAS+ variable v_i. Map is used
/// instead of unordered_map because single-entry look-up usage is minimal, instead the common
/// usage is to iterate over all values
using SasPlusPartialState = std::map<size_t, VariableDomSize>;


/// Test if the set represented by the partial SAS+ state contains the SAS+ state s
inline bool contains(SasPlusPartialState const& partial_state, SasPlusState s) {
  for (auto const& it : partial_state) {
    size_t const& idx = it.first;
    VariableDomSize const& value = it.second;
    assert(idx < s.size());
    if (s[idx] != value) {
      return false;
    }
  }
  return true;
}


/**************************************************************************************************
 *
 * Single-objective classes
 *
 **************************************************************************************************/

/**
 * Class to represent a deterministic SAS+ action with arbritary (single) cost. Notice that the
 * prevail condition is represented together with the precondition in this class.
 */
class SasPlusAction {
 public:
  SasPlusAction(std::string const& name, SasPlusPartialState const& prec,
      SasPlusPartialState const& eff, double cost)
    : name_(name), prec_(prec), eff_(eff), cost_(cost)
  { }

  bool isApplicable(SasPlusState const& s) const {
    return contains(prec_, s);
  }

  double cost() const {
    return cost_;
  }

  SasPlusState successor(SasPlusState const& s) const {
    assert(isApplicable(s));
    SasPlusState sp(s);
    for (auto const& it : eff_) {
      sp[it.first] = it.second;
    }
    return sp;
  }

  std::string const& name() const { return name_; }

  /* SAS+ specific methods */
  SasPlusPartialState const& precondition() const {
    return prec_;
  }

  SasPlusPartialState const& effect() const {
    return eff_;
  }

 private:
  std::string name_;
  SasPlusPartialState prec_;
  SasPlusPartialState eff_;
  double cost_;
};


/**
 * Class to represent a deterministic SAS+ problem with arbritary (single) cost.
 */
class SasPlusDetPlanProb {
 public:
  using State = SasPlusState;
  using Action = SasPlusAction;

  SasPlusDetPlanProb(std::vector<SasPlusVariable> const& variables, State const& s0,
                             SasPlusPartialState const& goal, std::vector<Action> const& actions)
    : variables_(variables), s0_(s0), goal_(goal), actions_(actions)
  {
    assert(actions_.size() > 0);
  }

  /*
   * Implementing SingleSourceMultipleSinks Concept 
   */
  State initialState() const {
    return s0_;
  }

  bool isGoal(State const& s) const {
    return contains(goal_, s);
  }

  /*
   * Implementing DiscreteActions Concept
   */
  std::vector<Action> allActions() const {
    return actions_;
  }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  /*
   * Implementing SingleCost Concept
   */
  double cost(State const& s, Action const& a) const {
    return a.cost();
  }

  /*
   * Implementing DeterministicDynamics Concept
   */
  State successor(State const& s, Action const& a) const {
    return a.successor(s);
  }


  /*
   * SAS+ specific methods to handle variables and the goal formula
   */
  size_t numberOfVariables() const {
    return variables_.size();
  }

  SasPlusVariable variable(size_t i) const {
    assert(i < variables_.size());
    return variables_[i];
  }

  /// Use this method in a range-based for loop to iterate over all variables
  std::vector<SasPlusVariable> const& variables() const {
    return variables_;
  }

  SasPlusPartialState const& goal() const {
    return goal_;
  }

 private:
  std::vector<SasPlusVariable> variables_;
  State s0_;
  SasPlusPartialState goal_;
  std::vector<Action> actions_;
};


/**************************************************************************************************
 *
 * Multi-objective classes
 *
 **************************************************************************************************/

/**
 * Class to represent a deterministic SAS+ action with vector costs. Notice that the prevail
 * condition is represented together with the precondition in this class.
 */
class SasPlusMultiCostAction {
 public:
  SasPlusMultiCostAction(std::string const& name, SasPlusPartialState const& prec,
      SasPlusPartialState const& eff, std::vector<double> const& cost)
    : name_(name), prec_(prec), eff_(eff), cost_(cost)
  { }

  bool isApplicable(SasPlusState const& s) const {
    return contains(prec_, s);
  }

  size_t numCostFunctions() const {
    return cost_.size();
  }

  std::vector<double> const& cost() const {
    return cost_;
  }

  SasPlusState successor(SasPlusState const& s) const {
    assert(isApplicable(s));
    SasPlusState sp(s);
    for (auto const& it : eff_) {
      sp[it.first] = it.second;
    }
    return sp;
  }

  std::string const& name() const { return name_; }

  /* SAS+ specific methods */
  SasPlusPartialState const& precondition() const {
    return prec_;
  }

  SasPlusPartialState const& effect() const {
    return eff_;
  }

 private:
  std::string name_;
  SasPlusPartialState prec_;
  SasPlusPartialState eff_;
  std::vector<double> cost_;
};


/**
 * Class to represent a deterministic SAS+ problem with vector costs.
 */
class SasPlusMultiObjDetPlanProb {
 public:
  using State = SasPlusState;
  using Action = SasPlusMultiCostAction;

  SasPlusMultiObjDetPlanProb(std::vector<SasPlusVariable> const& variables, State const& s0,
                             SasPlusPartialState const& goal, std::vector<Action> const& actions)
    : variables_(variables), s0_(s0), goal_(goal), actions_(actions)
  {
    assert(actions_.size() > 0);
    num_cost_func_ = actions_[0].numCostFunctions();
  }

  /*
   * Implementing SingleSourceMultipleSinks Concept 
   */
  State initialState() const {
    return s0_;
  }

  bool isGoal(State const& s) const {
    return contains(goal_, s);
  }

  /*
   * Implementing DiscreteActions Concept
   */
  std::vector<Action> allActions() const {
    return actions_;
  }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  /*
   * Implementing MultiCost Concept
   */
  size_t numCostFunctions() const {
    return num_cost_func_;
  }

  std::vector<double> cost(State const& s, Action const& a) const {
    auto cost_vec = a.cost();
    assert(cost_vec.size() == numCostFunctions());
    return cost_vec;
  }

  /*
   * Implementing DeterministicDynamics Concept
   */
  State successor(State const& s, Action const& a) const {
    return a.successor(s);
  }


  /*
   * SAS+ specific methods to handle variables and the goal formula
   */
  size_t numberOfVariables() const {
    return variables_.size();
  }

  SasPlusVariable variable(size_t i) const {
    assert(i < variables_.size());
    return variables_[i];
  }

  /// Use this method in a range-based for loop to iterate over all variables
  std::vector<SasPlusVariable> const& variables() const {
    return variables_;
  }

  SasPlusPartialState const& goal() const {
    return goal_;
  }

 private:
  std::vector<SasPlusVariable> variables_;
  State s0_;
  SasPlusPartialState goal_;
  std::vector<Action> actions_;
  size_t num_cost_func_;
};

}  // namespace SasPlus


// Placing this here because SasPlusState is a std::vector so operator<< needs to be either in the
// current namespace or the std namespace
inline std::ostream& operator<<(std::ostream& os, SasPlus::SasPlusState const& state) {
  for (bool first_item = true; auto const& value : state) {
    os << (first_item ? first_item = false, "[" : " ") << value;
  }
  os << "]";
  return os;
}

// See  operator<<(std::ostream& os, SasPlus::SasPlusState) for placement explanation
inline std::ostream& operator<<(std::ostream& os, SasPlus::SasPlusPartialState const& partial_state) {
  for (bool first_item = true; auto const& idx_value : partial_state) {
    os << (first_item ? first_item = false, "[" : " ") << "{v_" << idx_value.first << " = "
       << idx_value.second << "}";
  }
  os << "]";
  return os;
}

#endif  // SRC_REPRESENTATIONS_SASPLUS_H_
