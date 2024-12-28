#ifndef PLANNING_FUNCTION_HPP
#define PLANNING_FUNCTION_HPP

#include <string>

namespace planning {
  class Function {
   public:
    std::string name;
    int arity;

    Function() = default;

    Function(const std::string &name, const int arity);

    std::string to_string() const;

    bool operator==(const Function &other) const { return to_string() == other.to_string(); }

    bool operator<(const Function &other) const { return to_string() < other.to_string(); }
  };

}  // namespace planning

template <> class std::hash<planning::Function> {
  std::size_t operator()(const planning::Function &k) const {
    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return (std::hash<std::string>()(k.name) ^ (std::hash<int>()(k.arity) << 1)) >> 1;
  }
};

#endif  // PLANNING_FUNCTION_HPP
