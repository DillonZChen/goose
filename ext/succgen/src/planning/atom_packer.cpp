#include "../../include/planning/atom_packer.hpp"

// Code from Powerlifted

namespace wlplan {
namespace planning {
  AtomPacker::AtomPacker() { i = 0; }
  int AtomPacker::pack(const int predicate_index, const std::vector<int> &instantiation) {
    auto p = atom_to_index.try_emplace(make_pair(predicate_index, instantiation), i);
    int index = p.first->second;
    if (p.second) {
      index_to_atom.emplace(i, make_pair(predicate_index, instantiation));
      i++;
    }
    return index;
  }

}  // namespace planning
} // namespace wlplan
