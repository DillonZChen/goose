#ifndef PLANNING_PROBLEM_HPP
#define PLANNING_PROBLEM_HPP

#include "atom.hpp"
#include "domain.hpp"
#include "fluent.hpp"
#include "numeric_condition.hpp"

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
    std::unordered_set<Object> problem_objects_set;
    std::unordered_set<Object> constant_objects_set;
    std::vector<Object> problem_objects;
    std::vector<Object> constant_objects;

    std::vector<Atom> statics;
    std::vector<Fluent> fluents;
    std::vector<double> fluent_values;
    std::unordered_map<std::string, int> fluent_name_to_id;

    const std::vector<Atom> positive_goals;
    const std::vector<Atom> negative_goals;
    std::vector<NumericCondition> numeric_goals;

    void update_fluent_map();

   public:
    Problem(const Domain &domain,
            const std::vector<std::string> &objects,
            const std::vector<Atom> &statics,
            const std::vector<Fluent> &fluents,
            const std::vector<double> &fluent_values,
            const std::vector<Atom> &positive_goals,
            const std::vector<Atom> &negative_goals,
            const std::vector<NumericCondition> &numeric_goals);

    Problem(const Domain &domain,
            const std::vector<std::string> &objects,
            const std::vector<Fluent> &fluents,
            const std::vector<double> &fluent_values,
            const std::vector<Atom> &positive_goals,
            const std::vector<Atom> &negative_goals,
            const std::vector<NumericCondition> &numeric_goals);

    Problem(const Domain &domain,
            const std::vector<std::string> &objects,
            const std::vector<Atom> &positive_goals,
            const std::vector<Atom> &negative_goals);

    Domain get_domain() const { return *domain; }

    std::vector<Object> get_problem_objects() const { return problem_objects; }
    std::vector<Object> get_constant_objects() const { return constant_objects; }

    std::vector<Atom> get_statics() const { return statics; }
    std::vector<Fluent> get_fluents() const { return fluents; }
    std::vector<double> get_fluent_values() const { return fluent_values; }

    std::unordered_map<std::string, int> get_fluent_name_to_id() const { return fluent_name_to_id; }
    int get_fluent_id(const std::string &fluent_name) const {
      return fluent_name_to_id.at(fluent_name);
    }

    std::vector<Atom> get_positive_goals() const { return positive_goals; }
    std::vector<Atom> get_negative_goals() const { return negative_goals; }
    std::vector<NumericCondition> get_numeric_goals() const { return numeric_goals; }

    bool is_constant_object(const Object &object) const {
      return constant_objects_set.count(object);
    }

    void dump() const;
  };

}  // namespace planning

#endif  // PLANNING_PROBLEM_HPP
