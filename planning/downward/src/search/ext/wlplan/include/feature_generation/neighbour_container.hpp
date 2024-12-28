#ifndef FEATURE_GENERATION_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATION_NEIGHBOUR_CONTAINER_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

namespace feature_generation {
  class NeighbourContainer {
   public:
    NeighbourContainer(bool multiset_hash);

    void clear();

    void insert(const int node_colour, const int edge_label);

    std::string to_string() const;

    std::vector<int> to_vector() const;

   private:
    const bool multiset_hash;
    // profiling showed that using pairs is faster than vector of maps/sets, and that ordered
    // containers are faster than unordered containers given that pair does not have a hash
    std::set<std::pair<int, int>> neighbours_set;
    std::map<std::pair<int, int>, int> neighbours_mset;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NEIGHBOUR_CONTAINER_HPP
