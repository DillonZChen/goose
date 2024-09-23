#ifndef PLANNING_PROBLEM_HPP
#define PLANNING_PROBLEM_HPP

#include "atom.hpp"
#include "domain.hpp"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace planning {
  using Assignment = std::vector<int>;

  class Problem {
   private:
    std::shared_ptr<Domain> domain;

    std::unordered_map<Object, int> object_to_id;
    std::unordered_map<Object, std::string> object_to_short_str;

    std::unordered_set<Object> constant_objects_set;

    std::vector<Object> problem_objects;
    std::vector<Object> constant_objects;

    const std::vector<Atom> positive_goals;
    const std::vector<Atom> negative_goals;

   public:

    Problem(const Domain &domain,
            const std::vector<std::string> &objects,
            const std::vector<Atom> &positive_goals,
            const std::vector<Atom> &negative_goals);

    Domain get_domain() const { return *domain; }

    std::vector<Object> get_problem_objects() const { return problem_objects; }
    std::vector<Object> get_constant_objects() const { return constant_objects; }
    std::vector<Atom> get_positive_goals() const { return positive_goals; }
    std::vector<Atom> get_negative_goals() const { return negative_goals; }

    bool is_constant_object(const Object &object) const { return constant_objects_set.count(object); }

    // short string representations are used for nodes in the graph
    std::string get_atom_short_str(const Atom &atom) const;

    std::string get_obj_short_str(const Object &object) const;
  };

}  // namespace planning

#endif  // PLANNING_PROBLEM_HPP
