#ifndef GOOSE_HEURISTIC_H
#define GOOSE_HEURISTIC_H

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include "../heuristic.h"


/*
  Modified from Florian's FD-Hypernet c++ code

  Mainly used as a wrapper for Python models. Calls Python models during heuristic evaluation.
  Used for GNN and kernel heuristics. For optimised kernel heuristics, see goose_linear_regression.h
*/

namespace goose_heuristic {
class GooseHeuristic : public Heuristic {
  void initialise_model(const plugins::Options &opts);
  void initialise_grounded_facts();
  void initialise_lifted_facts();
  void initialise_facts();

  // Required for pybind. Once this goes out of scope python interaction is no
  // longer possible.
  //
  // IMPORTANT: since member variables are destroyed in reverse order of
  // construction it is important that the scoped_interpreter_guard is listed as
  // first member variable (therefore destroyed last), otherwise calling the
  // destructor of this class will lead to a segmentation fault.
  pybind11::scoped_interpreter guard;

  // Python object which computes the heuristic
  pybind11::object model;

  // Dictionary that maps FD proposition strings to (pred o_1 ... o_n) 
  // proposition strings in the case of grounded GOOSE, or (pred, args) tuples. 
  // We only want to do this translation once, 
  // hence we store it here. This could be ignored if we change the format of
  // propositions in GOOSE.
  std::map<FactPair, std::string> fact_to_grounded_goose_input;
  std::map<FactPair, std::pair<std::string, std::vector<std::string>>> fact_to_lifted_goose_input;

  bool lifted_goose;

  pybind11::list list_to_goose_state(const State &ancestor_state);

 protected:
  virtual int compute_heuristic(const State &ancestor_state) override;
  virtual std::vector<int> compute_heuristic_batch(
    const std::vector<State> &ancestor_states) override;
  
 public:
  explicit GooseHeuristic(const plugins::Options &opts);
};

}  // namespace goose_heuristic

#endif

