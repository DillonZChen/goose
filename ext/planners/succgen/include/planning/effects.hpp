#ifndef PLANNING_ACTION_HPP
#define PLANNING_ACTION_HPP

#include "numeric_effect.hpp"

#include <memory>
#include <set>
#include <vector>

namespace planning {

  using Atom = std::pair<int, std::vector<int>>;

  struct Effects {  // storing only the effects of the action
    const std::vector<Atom> adds;
    const std::vector<Atom> dels;
    const std::vector<std::shared_ptr<NumericEffect>> numeric_effects;

    Effects(const std::vector<Atom> &adds,
            const std::vector<Atom> &dels,
            const std::vector<std::shared_ptr<NumericEffect>> &numeric_effects)
        : adds(adds), dels(dels), numeric_effects(numeric_effects) {}
  };

}  // namespace planning

#endif  // PLANNING_ACTION_HPP
