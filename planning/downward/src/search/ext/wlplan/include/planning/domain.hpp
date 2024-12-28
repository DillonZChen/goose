#ifndef PLANNING_DOMAIN_HPP
#define PLANNING_DOMAIN_HPP

#include "../../include/utils/nlohmann/json.hpp"
#include "function.hpp"
#include "predicate.hpp"
#include "state.hpp"

#include <string>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

namespace planning {
  class Domain {
   public:
    std::string name;
    std::vector<Predicate> predicates;
    std::vector<Function> functions;
    std::vector<Object> constant_objects;

    // maps a predicate to an ID in {0, ..., n_pred - 1}, sorted by predicate name and arity
    std::unordered_map<std::string, int> predicate_to_colour;

    Domain(const std::string &name,
           const std::vector<Predicate> &predicates,
           const std::vector<Function> &functions,
           const std::vector<Object> &constant_objects);

    Domain(const std::string &name,
           const std::vector<Predicate> &predicates,
           const std::vector<Object> &constant_objects);

    Domain(const std::string &name, const std::vector<Predicate> &predicates);

    Domain(const std::string &name,
           const std::vector<Predicate> &predicates,
           const std::vector<Function> &functions);

    std::unordered_map<std::string, Predicate> get_name_to_predicate() const;
    std::unordered_map<std::string, Function> get_name_to_function() const;

    int get_max_arity() const;

    json to_json() const;

    std::string to_string() const;

    bool operator==(const Domain &other) const {
      return name == other.name && predicates == other.predicates &&
             constant_objects == other.constant_objects;
    }
  };
}  // namespace planning

#endif  // PLANNING_DOMAIN_HPP
