#ifndef GOOSE_GNN_HEURISTIC_H
#define GOOSE_GNN_HEURISTIC_H

#include <string>
#include <vector>

#include "goose_heuristic.h"

/*
  Modified from Florian's FD-Hypernet c++ code

  Mainly used as a wrapper for Python models. Calls Python models during heuristic evaluation.
  Used for GNN and kernel heuristics. For optimised kernel heuristics, see goose_linear_regression.h
*/

namespace goose_gnn_heuristic {
class GooseHeuristic : public goose_heuristic::GooseHeuristic {
  void initialise_model(const plugins::Options &opts);

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

 protected:
  virtual int compute_heuristic(const State &ancestor_state) override;

 public:
  explicit GooseHeuristic(const plugins::Options &opts);
};

}  // namespace goose_gnn_heuristic

#endif
