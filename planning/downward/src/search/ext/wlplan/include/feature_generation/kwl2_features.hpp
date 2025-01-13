#ifndef FEATURE_GENERATION_KWL2_FEATURES_HPP
#define FEATURE_GENERATION_KWL2_FEATURES_HPP

#include "features.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

#define NO_EDGE_COLOUR -1

namespace feature_generation {
  class KWL2Features : public Features {
   public:
    KWL2Features(const planning::Domain &domain,
                 std::string graph_representation,
                 int iterations,
                 std::string pruning,
                 bool multiset_hash);

    KWL2Features(const std::string &filename);

    Embedding embed(const std::shared_ptr<graph::Graph> &graph) override;

   private:
    inline int get_initial_colour(int index,
                                  int u,
                                  int v,
                                  const std::shared_ptr<graph::Graph> &graph,
                                  const std::vector<int> &pair_to_edge_label);
    void collect_main(const std::vector<graph::Graph> &graphs) override;
    void refine(const std::shared_ptr<graph::Graph> &graph,
                std::vector<int> &colours,
                std::vector<int> &colours_tmp);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_KWL2_FEATURES_HPP
