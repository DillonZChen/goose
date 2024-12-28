#ifndef GRAPH_NILG_GENERATOR_HPP
#define GRAPH_NILG_GENERATOR_HPP

#include "ilg_generator.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph {
  class NILGGenerator : public ILGGenerator {
   public:
    NILGGenerator(const planning::Domain &domain, bool differentiate_constant_objects);

    // Change the base graph based on the input problem
    void set_problem(const planning::Problem &problem) override;

    // Extends ILG methods
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override;

   protected:
    std::unordered_map<std::string, int> fluent_to_colour;
    int UNACHIEVED_GT_GOAL;
    int UNACHIEVED_GTEQ_GOAL;
    int UNACHIEVED_EQ_GOAL;
    int ACHIEVED_GT_GOAL;
    int ACHIEVED_GTEQ_GOAL;
    int ACHIEVED_EQ_GOAL;

    // Fluent values are given in every state.
    std::shared_ptr<Graph> modify_graph_from_numerics(const planning::State &state,
                                                      const std::shared_ptr<Graph> graph);
  };
}  // namespace graph

#endif  // GRAPH_NILG_GENERATOR_HPP
