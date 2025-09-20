#ifndef PLANNING_ATOM_PACKER_HPP
#define PLANNING_ATOM_PACKER_HPP

// Same idea as Powerlifted's StatePacker

#include "../parallel_hashmap/phmap.h"
#include "../utils/hash.h"

#include <algorithm>
#include <map>
#include <vector>

namespace wlplan {
namespace planning {

  using GroundAtom = std::vector<int>;

  class AtomPacker {

    phmap::flat_hash_map<std::pair<int, GroundAtom>, int, utils::Hash<std::pair<int, GroundAtom>>>
        atom_to_index;
    phmap::flat_hash_map<int, std::pair<int, GroundAtom>> index_to_atom;
    int i;

   public:
    AtomPacker();

    int pack(const int predicate_index, const std::vector<int> &instantiation);

    std::pair<int, std::vector<int>> unpack(const int index) { return index_to_atom.at(index); }
  };
}  // namespace planning
} // namespace wlplan

#endif  // PLANNING_ATOM_PACKER_HPP
