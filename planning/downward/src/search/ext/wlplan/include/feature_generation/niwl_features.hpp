#ifndef FEATURE_GENERATION_NIWL_FEATURES_HPP
#define FEATURE_GENERATION_NIWL_FEATURES_HPP

#include "iwl_features.hpp"

#include <memory>
#include <string>
#include <vector>

namespace feature_generation {
  class NIWLFeatures : public IWLFeatures {
   public:
    NIWLFeatures(const planning::Domain &domain,
                 std::string graph_representation,
                 int iterations,
                 std::string pruning,
                 bool multiset_hash);

    NIWLFeatures(const std::string &filename);

    Embedding embed(const std::shared_ptr<graph::Graph> &graph) override;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NIWL_FEATURES_HPP
