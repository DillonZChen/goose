#include "../../include/planning/predicate.hpp"

#include <stdexcept>

namespace planning {
  Predicate::Predicate(const std::string &name, const int arity) : name(name), arity(arity) {
    if (arity < 0) {
      std::string err_msg = "Predicate " + name + " has arity " + std::to_string(arity) + " < 0";
      throw std::invalid_argument(err_msg);
    }
  }

  std::string Predicate::to_string() const { return name + "/" + std::to_string(arity); }
}  // namespace planning
