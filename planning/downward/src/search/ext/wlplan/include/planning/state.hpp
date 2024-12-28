#ifndef PLANNING_STATE_HPP
#define PLANNING_STATE_HPP

#include "atom.hpp"

#include <vector>

namespace planning {
  class State {
   public:
    std::vector<std::shared_ptr<planning::Atom>> atoms;
    std::vector<double> values;

    State(const std::vector<std::shared_ptr<planning::Atom>> &atoms,
          const std::vector<double> &values);
    State(const std::vector<std::shared_ptr<planning::Atom>> &atoms);
    State(const std::vector<planning::Atom> &atoms, const std::vector<double> &values);
    State(const std::vector<planning::Atom> &atoms);

    // for Python bindings
    std::vector<planning::Atom> get_atoms() const;

    std::string to_string() const;

    bool operator==(const State &other) const;

    std::size_t hash() const;
  };
}  // namespace planning

#endif  // PLANNING_STATE_HPP
