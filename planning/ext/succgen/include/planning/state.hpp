#ifndef PLANNING_STATE_HPP
#define PLANNING_STATE_HPP

#include "atom_packer.hpp"
#include "effects.hpp"
#include "utils.hpp"

#include <set>
#include <vector>

namespace planning {

  class State {
   public:
    Atoms atoms;
    Values values;

    State(const Atoms &atoms, const Values &values);

    State get_copy() const;

    void add_atom(int atom) { atoms.insert(atom); }
    void del_atom(int atom) { atoms.erase(atom); }
    void set_value(int index, double value) { values[index] = value; }

    State apply_action(const Effects &action,
                       const std::vector<int> &instantiation,
                       AtomPacker &atom_packer,
                       const FluentIndexMap &nvars_map);

    bool operator==(const State &other) const;
  };

  class StateHash {
   public:
    unsigned operator()(const State &s) const;
  };

}  // namespace planning

#endif  // PLANNING_STATE_HPP
