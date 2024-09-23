#ifndef GRAPH_ILG_GENERATOR_HPP
#define GRAPH_ILG_GENERATOR_HPP

#include "../planning/atom.hpp"
#include "../planning/domain.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define ILG_FACT_DESCRIPTIONS                                                                      \
  X(T_POS_GOAL, "T_POS_GOAL")                                                                      \
  X(F_POS_GOAL, "F_POS_GOAL")                                                                      \
  X(T_NEG_GOAL, "T_NEG_GOAL")                                                                      \
  X(F_NEG_GOAL, "F_NEG_GOAL")                                                                      \
  X(NON_GOAL, "NON_GOAL")                                                                          \
  X(_LAST, "_size_of_the_enum")

#define X(description, name) description,
enum class ILGFactDescription { ILG_FACT_DESCRIPTIONS };
#undef X

namespace graph {
  class ILGGenerator : public GraphGenerator {
   public:
    ILGGenerator(const planning::Domain &domain, bool differentiate_constant_objects);

    // Change the base graph based on the input problem
    void set_problem(const planning::Problem &problem) override;

    // Makes a copy of the base graph and makes the necessary modifications
    // Assumes the state is from the problem that is set but does not check this.
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;

    // Optimised variant of to_graph() but requires calling reset_graph() after. 
    // Does not make a copy of the base graph and instead modifies it directly,
    // and undoing the modifications with reset_graph().
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state);
    void reset_graph() const;

    int get_n_edge_labels() const override;

    std::vector<int> get_init_colours() const override;

    void print_init_colours() const override;

    void dump_graph() const override;

   private:
    /* The following variables remain constant for all problems */
    const planning::Domain &domain;
    const std::unordered_map<std::string, int> predicate_to_colour;
    bool differentiate_constant_objects;

    /* These variables get reset every time a new problem is set */
    std::shared_ptr<Graph> base_graph;
    std::unordered_set<std::string> positive_goal_names;
    std::unordered_set<std::string> negative_goal_names;
    std::shared_ptr<planning::Problem> problem;

    // Do not use a vector here because colours can be negative, i.e. constant objects
    std::map<int, std::string> colour_to_description;
    int fact_colour(const int predicate_idx, const ILGFactDescription &fact_description) const;
    int fact_colour(const planning::Atom &atom, const ILGFactDescription &fact_description) const;

    /* For modifying the base graph and redoing its changes */
    int n_nodes_added;
    std::vector<int> n_edges_added;
    std::vector<int> pos_goal_changed;
    std::vector<int> neg_goal_changed;
    std::vector<int> pos_goal_changed_pred;
    std::vector<int> neg_goal_changed_pred;
    std::shared_ptr<Graph> modify_graph_from_state(const planning::State &state,
                                                   const std::shared_ptr<Graph> graph,
                                                   bool store_changes);
  };

  inline int ILGGenerator::fact_colour(const int predicate_idx,
                                       const ILGFactDescription &fact_description) const {
    int pred_idx = predicate_idx * ((int)ILGFactDescription::_LAST);
    return 1 + pred_idx + (int)fact_description;
  }

  inline int ILGGenerator::fact_colour(const planning::Atom &atom,
                                       const ILGFactDescription &fact_description) const {
    return fact_colour(predicate_to_colour.at(atom.predicate->name), fact_description);
  }
}  // namespace graph

#endif  // GRAPH_ILG_GENERATOR_HPP
