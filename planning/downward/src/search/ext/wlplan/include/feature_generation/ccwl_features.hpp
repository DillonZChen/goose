#ifndef FEATURE_GENERATION_CCWL_FEATURES_HPP
#define FEATURE_GENERATION_CCWL_FEATURES_HPP

#include "wl_features.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

#define NO_EDGE_COLOUR -1

namespace feature_generation {
  class CCWLFeatures : public WLFeatures {
   public:
    CCWLFeatures(const planning::Domain &domain,
                 std::string graph_representation,
                 int iterations,
                 std::string prune_features,
                 bool multiset_hash);

    CCWLFeatures(const std::string &filename);

    Embedding embed(const std::shared_ptr<graph::Graph> &graph) override;

    void set_weights(const std::vector<double> &weights);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_CCWL_FEATURES_HPP
