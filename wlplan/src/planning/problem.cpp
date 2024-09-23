#include "../../include/planning/problem.hpp"

#include <iostream>

namespace planning {
  Problem::Problem(const Domain &domain,
                   const std::vector<std::string> &objects,
                   const std::vector<Atom> &positive_goals,
                   const std::vector<Atom> &negative_goals)
      : domain(std::make_shared<Domain>(domain)),
        positive_goals(positive_goals),
        negative_goals(negative_goals) {

    int cnt = 0;
    for (const auto &object : domain.constant_objects) {
      object_to_id[object] = cnt;
      object_to_short_str[object] = "o" + std::to_string(cnt);
      constant_objects_set.insert(object);
      constant_objects.push_back(object);
      cnt++;
    }

    for (const auto &object : objects) {
      if (constant_objects_set.count(object)) {
        continue;
      }
      object_to_id[object] = cnt;
      object_to_short_str[object] = "o" + std::to_string(cnt);
      problem_objects.push_back(object);
      cnt++;
    }
  }

  std::string Problem::get_atom_short_str(const Atom &atom) const {
    std::string repr = std::to_string(domain->predicate_to_colour.at(atom.predicate->name));
    for (const auto &object : atom.objects) {
      repr += "." + std::to_string(object_to_id.at(object));
    }
    return repr;
  }

  std::string Problem::get_obj_short_str(const Object &object) const {
    return object_to_short_str.at(object);
  }
}  // namespace planning
