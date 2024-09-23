#include "../../include/planning/atom.hpp"

namespace planning {
  Atom::Atom(const Predicate &predicate, const std::vector<Object> &objects)
      : predicate(std::make_shared<Predicate>(predicate)), objects(objects) {}

  std::string Atom::to_string() const {
    std::string repr = predicate->name + "(";
    for (size_t i = 0; i < objects.size(); i++) {
      repr += objects[i];
      if (i < objects.size() - 1) {
        repr += ", ";
      }
    }
    repr += ")";
    return repr;
  }
}  // namespace planning
