#include "../../include/planning/domain.hpp"

#include <algorithm>

namespace planning {
  Domain::Domain(const std::string &name,
                 const std::vector<Predicate> &predicates,
                 const std::vector<Object> &constant_objects)
      : name(name), predicates(predicates), constant_objects(constant_objects) {
    // sort items to ensure consistency when saving and loading models
    std::sort(this->constant_objects.begin(), this->constant_objects.end());
    std::sort(this->predicates.begin(), this->predicates.end());
    predicate_to_colour = std::unordered_map<std::string, int>();
    for (size_t i = 0; i < this->predicates.size(); i++) {
      predicate_to_colour[predicates[i].name] = i;
    }
  }

  Domain::Domain(const std::string &name, const std::vector<Predicate> &predicates)
      : Domain(name, predicates, std::vector<Object>()) {}

  int Domain::max_arity() const {
    int max_arity = 0;
    for (size_t i = 0; i < predicates.size(); i++) {
      max_arity = std::max(max_arity, predicates[i].arity);
    }
    return max_arity;
  }

  json Domain::to_json() const {
    json j;
    j["name"] = name;
    std::vector<std::pair<std::string, int>> predicates_raw;
    for (size_t i = 0; i < predicates.size(); i++) {
      predicates_raw.push_back(std::make_pair(predicates[i].name, predicates[i].arity));
    }
    j["predicates"] = predicates_raw;
    j["constant_objects"] = constant_objects;
    return j;
  }

  std::string Domain::to_string() const {
    std::string repr = "wlplan.Domain(name=" + name + ", predicates=[";
    for (size_t i = 0; i < predicates.size(); i++) {
      repr += predicates[i].to_string();
      if (i < predicates.size() - 1) {
        repr += ", ";
      }
    }
    repr += "], constant_objects=[";
    for (size_t i = 0; i < constant_objects.size(); i++) {
      repr += constant_objects[i];
      if (i < constant_objects.size() - 1) {
        repr += ", ";
      }
    }
    repr += "])";
    return repr;
  }
}  // namespace planning
