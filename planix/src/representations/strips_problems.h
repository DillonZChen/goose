#ifndef SRC_REPRESENTATIONS_STRIPS_PROBLEMS_H_
#define SRC_REPRESENTATIONS_STRIPS_PROBLEMS_H_

#include <cassert>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Needed in order to define hash function for boost::dynamic_bitset
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#include <boost/dynamic_bitset.hpp>


#include "models/container_templates.h"

/// Hash function for boost::dynamic_bitset<>
namespace std {
template<> struct hash<boost::dynamic_bitset<>> {
  size_t operator()(boost::dynamic_bitset<> const& bs) const {
    size_t rv = 0;
    boost::hash_combine(rv, bs.m_num_bits);
    boost::hash_combine(rv, bs.m_bits);
    return rv;
  }
};
}


/**
 * Namespace for all the models and data-structures to represent STRIPS models.
 *
 * The focus of this implementation is a trade-off of simplicity and efficiency instead of absolute
 * efficiency
 */
namespace STRIPS {
using StripsState = boost::dynamic_bitset<>;
using PrState = ::ProbDist<StripsState>;
using Mask = boost::dynamic_bitset<>;


/**
 * Struct representing a STRIPS effect
 */
struct Effect {
 public:
  /// Create an empty effect for a problem with n propostions
  explicit Effect(size_t n) : del(n), add(n) { }
  Effect(Mask const& d, Mask const& a) : del(d), add(a) { }

  StripsState apply(StripsState const& s) const {
    return (s & ~del) | add;
  }

  bool operator==(Effect const& other) const {
    return del == other.del && add == other.add;
  }

  Mask del;
  Mask add;
};


/**
 * Hash function for STRIPS effects
 */
struct hashEffect {
  size_t operator()(Effect const& e) const {
    size_t rv = std::hash<Mask>()(e.del);
    boost::hash_combine(rv, std::hash<Mask>()(e.add));
    return rv;
  }
};

/**************************************************************************************************
                                Classical STRIPS Single-Objective
 **************************************************************************************************/
class StripsAction {
 public:
  StripsAction(std::string const& name, double const& cost,
      Mask const& prec, Effect const& effect)
    : name_(name), cost_(cost), prec_(prec), effect_(effect)
  { }

  bool isApplicable(StripsState const& s) const {
    return (prec_ & s) == prec_;
  }

  double const& cost() const {
    return cost_;
  }

  StripsState successor(StripsState const& s) const {
    return effect_.apply(s);
  }

  std::string const& name() const { return name_; }

  Mask const& precondition() const { return prec_; }

  Effect const& effect() const { return effect_; }

 private:
  std::string name_;
  double cost_;
  Mask prec_;
  Effect effect_;
};


class StripsProblem {
 public:
  using State = StripsState;
  using Action = StripsAction;

  StripsProblem(State s0, Mask goal_set, std::vector<Action> const& actions)
    : s0_(s0), goal_set_(goal_set), actions_(actions)
  {
    size_t n_prop = numPropositions();
    assert(s0_.size() == n_prop);
    assert(goal_set_.size() == n_prop);
    for (Action const& a : actions_) {
      assert(a.precondition().size() == n_prop);
      assert(a.effect().del.size() == n_prop);
      assert(a.effect().add.size() == n_prop);
    }
  }

  State initialState() const {
    return s0_;
  }

  std::vector<Action> allActions() const {
    return actions_;
  }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  double const& cost(Action const& a) const {
    return a.cost();
  }

  State successor(State const& s, Action const& a) const {
    return a.successor(s);
  }

  bool isGoal(State const& s) const {
    return (s | ~goal_set_).all();
  }

  size_t numPropositions() const {
    return s0_.size();
  }

  Mask goal_set() const {
    return goal_set_;
  }

 private:
  State s0_;
  Mask goal_set_;
  std::vector<Action> actions_;
};


/**************************************************************************************************
                                       STRIPS Multi-Objective
 **************************************************************************************************/
class StripsMultiCostAction {
 public:
  StripsMultiCostAction(std::string const& name, std::vector<double> const& cost,
      Mask const& prec, Effect const& effect)
    : name_(name), cost_(cost), prec_(prec), effect_(effect)
  { }

  bool isApplicable(StripsState const& s) const {
    return (prec_ & s) == prec_;
  }

  size_t numCostFunctions() const {
    return cost_.size();
  }

  std::vector<double> const& cost() const {
    return cost_;
  }

  StripsState successor(StripsState const& s) const {
    return effect_.apply(s);
  }

  std::string const& name() const { return name_; }

  Mask const& precondition() const { return prec_; }

  Effect const& effect() const { return effect_; }

 private:
  std::string name_;
  std::vector<double> cost_;
  Mask prec_;
  Effect effect_;
};


class StripsMultiObjectiveProblem {
 public:
  using State = StripsState;
  using Action = StripsMultiCostAction;

  StripsMultiObjectiveProblem(State s0, Mask goal_set, std::vector<Action> const& actions)
    : s0_(s0), goal_set_(goal_set), actions_(actions)
  {
    size_t n_prop = numPropositions();
    size_t n_cost_func = numCostFunctions();
    assert(s0_.size() == n_prop);
    assert(goal_set_.size() == n_prop);
    for (Action const& a : actions_) {
      assert(a.precondition().size() == n_prop);
      assert(a.effect().del.size() == n_prop);
      assert(a.effect().add.size() == n_prop);
      assert(a.numCostFunctions() == n_cost_func);
    }
  }

  State initialState() const {
    return s0_;
  }

  std::vector<Action> allActions() const {
    return actions_;
  }

  bool isApplicable(State const& s, Action const& a) const {
    return a.isApplicable(s);
  }

  size_t numCostFunctions() const {
    auto it = actions_.begin();
    if (it != actions_.end()) {
      return it->numCostFunctions();
    }
    return 0;
  }

  std::vector<double> const& cost(Action const& a) const {
    return a.cost();
  }

  State successor(State const& s, Action const& a) const {
    return a.successor(s);
  }

  Mask goalSet() const {
    return goal_set_;
  }

  bool isGoal(State const& s) const {
    return (s | ~goal_set_).all();
  }

  size_t numPropositions() const {
    return s0_.size();
  }

 private:
  State s0_;
  Mask goal_set_;
  std::vector<Action> actions_;
};


/**************************************************************************************************
                                        Probabilistic STRIPS
 **************************************************************************************************/
using PrState = ::ProbDist<StripsState>;
using PrEffect = std::unordered_map<Effect, double, hashEffect>;


/**
 * Class representing a probabilistic STRIPS action
 *
 * This probabilistic extension is obtained by using as effect a probability distribution of STRIPS
 * effects.
 */
class ProbStripsAction {
 public:
  friend class StripsSSP;

  ProbStripsAction(std::string const& name, double cost, Mask prec, PrEffect const& pr_effects)
    : name_(name), cost_(cost), prec_(prec), pr_effects_(pr_effects)
  { }

  bool isApplicable(StripsState const& s) const {
    return (prec_ & s) == prec_;
  }

  double cost(StripsState const& s) const {
    return cost_;
  }

  PrState successors(StripsState const& s) const {
    assert(isApplicable(s));
    PrState succ;
    for (auto const& p : pr_effects_) {
      succ[p.first.apply(s)] += p.second;
    }
    return succ;
  }

  std::string const& name() const { return name_; }

  Mask precondition() const { return prec_; }

 private:
  std::string name_;
  double cost_;
  Mask prec_;
  PrEffect pr_effects_;
};


/**
 * Class representing an STRIPS-based SSP
 */
class StripsSSP {
 public:
  using State = StripsState;
  using Action = ProbStripsAction;

  StripsSSP(State s0, Mask goal_set, std::vector<Action> const& actions)
    : s0_(s0), goal_set_(goal_set), actions_(actions)
  {
    size_t n_prop = numPropositions();
    assert(s0_.size() == n_prop);
    assert(goal_set_.size() == n_prop);
    for (Action const& a : actions_) {
      assert(a.precondition().size() == n_prop);
      for (auto const& p : a.pr_effects_) {
        assert(p.first.del.size() == n_prop);
        assert(p.first.add.size() == n_prop);
      }
    }
  }

  State initialState() const {
    return s0_;
  }

  std::vector<Action> allActions() const {
    return actions_;
  }

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
    return (s | ~goal_set_).all();
  }

  size_t numPropositions() const {
    return s0_.size();
  }

 private:
  State s0_;
  Mask goal_set_;
  std::vector<Action> actions_;
};
}  // namespace STRIPS

#endif  // SRC_REPRESENTATIONS_STRIPS_PROBLEMS_H_
