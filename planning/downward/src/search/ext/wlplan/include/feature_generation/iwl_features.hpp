#ifndef FEATURE_GENERATION_IWL_FEATURES_HPP
#define FEATURE_GENERATION_IWL_FEATURES_HPP

#include "features.hpp"

#include <memory>
#include <string>
#include <vector>

// use minimum int because negative values are occupied by constant objects
#define INDIVIDUALISE_COLOUR -2147483648

namespace feature_generation {
  class IWLFeatures : public Features {
   public:
    IWLFeatures(const std::string feature_name,
                const planning::Domain &domain,
                std::string graph_representation,
                int iterations,
                std::string prune_features,
                bool multiset_hash);

    IWLFeatures(const planning::Domain &domain,
                std::string graph_representation,
                int iterations,
                std::string prune_features,
                bool multiset_hash);

    IWLFeatures(const std::string &filename);

    Embedding embed(const std::shared_ptr<graph::Graph> &graph) override;

   protected:
    void collect_main(const std::vector<graph::Graph> &graphs) override;
    void refine(const std::shared_ptr<graph::Graph> &graph,
                std::vector<int> &colours,
                std::vector<int> &colours_tmp);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_IWL_FEATURES_HPP
