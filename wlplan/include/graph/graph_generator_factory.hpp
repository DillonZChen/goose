#ifndef GRAPH_GRAPH_GENERATOR_FACTORY_HPP
#define GRAPH_GRAPH_GENERATOR_FACTORY_HPP

#include "../planning/domain.hpp"
#include "graph_generator.hpp"

#include <memory>
#include <string>

namespace graph {
  std::shared_ptr<GraphGenerator> create_graph_generator(const std::string &name,
                                                         const planning::Domain &domain);
}  // namespace graph

#endif  // GRAPH_GRAPH_GENERATOR_FACTORY_HPP
