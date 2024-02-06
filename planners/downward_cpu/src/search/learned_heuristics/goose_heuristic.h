#ifndef GOOSE_HEURISTIC_H
#define GOOSE_HEURISTIC_H

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../heuristic.h"

typedef pybind11::list GooseState;

namespace goose_heuristic {

typedef std::string GroundedInput;
typedef std::pair<std::string, std::vector<std::string>> LiftedInput;

class GooseHeuristic : public Heuristic {
 protected:
  void initialise_grounded_facts();
  void initialise_lifted_facts();
  void initialise_facts();

  // Dictionary that maps FD proposition strings to (pred o_1 ... o_n) proposition strings in the
  // case of grounded GOOSE, or (pred, args) tuples. We only want to do this translation once, hence
  // we store it here. This could be ignored if we change the format of propositions in GOOSE.
  // Unordered map with hash seems to be slower and inaccurate
  std::map<FactPair, GroundedInput> fact_to_g_input;
  std::map<FactPair, LiftedInput> fact_to_l_input;

  bool lifted_goose;

  GooseState fd_state_to_goose_state(const State &ancestor_state);
  GooseState fact_pairs_list_to_goose_state(const std::vector<FactPair> &fact_pairs);

 public:
  explicit GooseHeuristic(const plugins::Options &opts);
};

}  // namespace goose_heuristic

#endif
