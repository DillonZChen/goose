#ifndef PLANNING_UTILS_HPP
#define PLANNING_UTILS_HPP

#include "../parallel_hashmap/phmap.h"
#include "../utils/hash.h"

#include <memory>
#include <set>
#include <vector>

namespace planning {

  using Atoms = std::set<int>;
  using Fluent = std::pair<int, std::vector<int>>;
  using Values = std::vector<double>;

  struct FluentValueMap {
    phmap::flat_hash_map<Fluent, double, utils::Hash<Fluent>> map;
    FluentValueMap(const std::vector<std::pair<std::pair<int, std::vector<int>>, double>> &fluent_value_map) {
      map = phmap::flat_hash_map<Fluent, double, utils::Hash<Fluent>>();
      for (const auto &pair : fluent_value_map) {
        map[pair.first] = pair.second;
      }
    }
  };

  struct FluentIndexMap {
    phmap::flat_hash_map<Fluent, double, utils::Hash<Fluent>> map;
    FluentIndexMap(const std::vector<std::pair<std::pair<int, std::vector<int>>, int>> &fluent_index_map) {
      map = phmap::flat_hash_map<Fluent, double, utils::Hash<Fluent>>();
      for (const auto &pair : fluent_index_map) {
        map[pair.first] = pair.second;
      }
    }
  };

}  // namespace planning

#endif  // PLANNING_UTILS_HPP