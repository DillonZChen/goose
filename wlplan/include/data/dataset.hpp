#ifndef DATA_DATASET_HPP
#define DATA_DATASET_HPP

#include "../graph/graph.hpp"
#include "../graph/ilg_generator.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"

#include <memory>
#include <string>
#include <vector>

namespace data {
  class ProblemStates {
   public:
    const planning::Problem problem;
    const std::vector<planning::State> states;

    ProblemStates(const planning::Problem &problem, const std::vector<planning::State> &states)
        : problem(problem), states(states){};
  };

  class Dataset {
   public:
    const planning::Domain &domain;
    const std::vector<ProblemStates> data;

    Dataset(const planning::Domain &domain, const std::vector<ProblemStates> &data);

    size_t get_size() const;

   private:
    std::unordered_map<std::string, int> predicate_to_arity;

    void check_good_atom(const planning::Atom &atom,
                         const std::unordered_set<planning::Object> &objects) const;
  };
}  // namespace data

#endif  // DATA_DATASET_HPP
