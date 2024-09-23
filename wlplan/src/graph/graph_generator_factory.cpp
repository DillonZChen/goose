#include "../../include/graph/graph_generator_factory.hpp"

#include "../../include/graph/ilg_generator.hpp"

namespace graph {
  std::shared_ptr<GraphGenerator> create_graph_generator(const std::string &name,
                                                         const planning::Domain &domain) {
    if (name == "ilg") {
      return std::make_shared<ILGGenerator>(domain, false);
    } else if (name == "custom") {
      return NULL;
    } else {
      throw std::runtime_error("Unknown graph generator: " + name);
    }
  }
}  // namespace graph
