#include "../../include/planning/state.hpp"

#include <algorithm>

namespace wlplan {
namespace planning {
  unsigned StateHash::operator()(const SGState &s) const {
    // Start with a hash seed
    unsigned hash = 17;

    // Hash each atom in the set
    for (const auto &atom : s.atoms) {
      // Standard hash combining technique
      hash = hash * 31 + std::hash<int>{}(atom);
    }

    // Hash each value in the vector of doubles
    for (const auto &value : s.values) {
      // Use std::hash for double values
      hash = hash * 31 + std::hash<double>{}(value);
    }

    return hash;
  }

  SGState::SGState(const Atoms &atoms, const Values &values) : atoms(atoms), values(values) {}

  SGState SGState::get_copy() const { return SGState(atoms, values); }

  SGState SGState::apply_action(const Effects &action,
                            const std::vector<int> &instantiation,
                            AtomPacker &atom_packer,
                            const FluentIndexMap &nvars_map) {
    Atoms new_atoms = atoms;
    Values new_values = values;

    for (const auto &del : action.dels) {
      std::vector<int> grounding(del.second.size());
      std::transform(del.second.begin(), del.second.end(), grounding.begin(), [&instantiation](int index) {
        return instantiation[index];
      });
      new_atoms.erase(atom_packer.pack(del.first, grounding));
    }

    for (const auto &add : action.adds) {
      std::vector<int> grounding(add.second.size());
      std::transform(add.second.begin(), add.second.end(), grounding.begin(), [&instantiation](int index) {
        return instantiation[index];
      });
      new_atoms.insert(atom_packer.pack(add.first, grounding));
    }

    for (const auto &effect : action.numeric_effects) {
      std::vector<int> grounding(effect->row.size());
      std::transform(effect->row.begin(), effect->row.end(), grounding.begin(), [&instantiation](int index) {
        return instantiation[index];
      });

      int i = nvars_map.map.at({effect->table, grounding});
      double value = effect->expression->evaluate(instantiation, nvars_map, values);
      new_values[i] = effect->apply(new_values[i], value);
    }

    return SGState(new_atoms, new_values);
  }

  bool SGState::operator==(const SGState &other) const { return (atoms == other.atoms) && (values == other.values); }
}  // namespace planning
} // namespace wlplan
