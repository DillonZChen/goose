#ifndef FEATURE_GENERATION_WL_FEATURES_HPP
#define FEATURE_GENERATION_WL_FEATURES_HPP

#include "features.hpp"

#include <memory>
#include <string>
#include <vector>

namespace feature_generation {
  class WLFeatures : public Features {
   public:
    WLFeatures(const std::string wl_name,
               const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string prune_features,
               bool multiset_hash);

    WLFeatures(const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string prune_features,
               bool multiset_hash);

    WLFeatures(const std::string &filename);

    Embedding embed(const std::shared_ptr<graph::Graph> &graph) override;

   protected:
    void collect_main(const std::vector<graph::Graph> &graphs) override;
    void refine(const std::shared_ptr<graph::Graph> &graph,
                std::vector<int> &colours,
                std::vector<int> &colours_tmp);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_WL_FEATURES_HPP
