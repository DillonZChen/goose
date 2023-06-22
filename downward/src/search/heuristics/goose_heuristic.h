#ifndef GOOSE_HEURISTIC_H
#define GOOSE_HEURISTIC_H

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include "../heuristic.h"

namespace goose_heuristic {
class GooseHeuristic : public Heuristic {

  bool lifted_state_input;

  // Required for pybind. Once this goes out of scope python interaction is no
  // longer possible.
  //
  // IMPORTANT: since member variables are destroyed in reverse order of
  // construction it is important that the scoped_interpreter_guard is listed as
  // first member variable (therefore destroyed last), otherwise calling the
  // destructor of this class will lead to a segmentation fault.
  pybind11::scoped_interpreter guard;

  pybind11::object model;

protected:
  virtual int compute_heuristic(const State &ancestor_state) override;
  
public:
  explicit GooseHeuristic(const plugins::Options &opts);

  // TODO compute_heuristic_batch
};

} // namespace goose_heuristic

#endif

