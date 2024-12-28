#ifndef GRAPH_GRAPH_GENERATOR_HPP
#define GRAPH_GRAPH_GENERATOR_HPP

#include "../planning/atom.hpp"
#include "../planning/domain.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"
#include "graph.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph {
  class GraphGenerator {
   public:
    // Change the base graph based on the input problem
    virtual void set_problem(const planning::Problem &problem) = 0;

    virtual ~GraphGenerator() = default;

    // Makes a copy of the base graph and makes the necessary modifications
    // Assumes the state is from the problem that is set but does not check this.
    virtual std::shared_ptr<Graph> to_graph(const planning::State &state) = 0;

    virtual std::shared_ptr<Graph> to_graph_opt(const planning::State &state) = 0;
    virtual void reset_graph() const = 0;

    virtual int get_n_edge_labels() const = 0;

    virtual void print_init_colours() const = 0;

    virtual void dump_graph() const = 0;
  };
}  // namespace graph

#endif  // GRAPH_GRAPH_GENERATOR_HPP
