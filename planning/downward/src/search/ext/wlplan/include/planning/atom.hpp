#ifndef PLANNING_ATOM_HPP
#define PLANNING_ATOM_HPP

#include "object.hpp"
#include "predicate.hpp"

#include <memory>
#include <string>
#include <vector>

namespace planning {
  class Atom {
   public:
    const std::shared_ptr<Predicate> predicate;
    const std::vector<Object> objects;

    Atom(const Predicate &predicate, const std::vector<Object> &objects);

    std::string to_string() const;

    bool operator==(const Atom &other) const { return to_string() == other.to_string(); }
  };

}  // namespace planning

#endif  // PLANNING_ATOM_HPP
