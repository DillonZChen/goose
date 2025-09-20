#ifndef PLANNING_STATE_HPP
#define PLANNING_STATE_HPP

#include "atom_packer.hpp"
#include "effects.hpp"
#include "utils.hpp"

#include <set>
#include <vector>

namespace wlplan {
namespace planning {

  class SGState {
   public:
    Atoms atoms;
    Values values;

    SGState(const Atoms &atoms, const Values &values);

    SGState get_copy() const;

    void add_atom(int atom) { atoms.insert(atom); }
    void del_atom(int atom) { atoms.erase(atom); }
    void set_value(int index, double value) { values[index] = value; }

    SGState apply_action(const Effects &action,
                       const std::vector<int> &instantiation,
                       AtomPacker &atom_packer,
                       const FluentIndexMap &nvars_map);

    bool operator==(const SGState &other) const;
  };

  class StateHash {
   public:
    unsigned operator()(const SGState &s) const;
  };

}  // namespace planning
} // namespace wlplan

#endif  // PLANNING_STATE_HPP
