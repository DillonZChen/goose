#ifndef FEATURE_GENERATION_WL_FEATURES_HPP
#define FEATURE_GENERATION_WL_FEATURES_HPP

#include "../data/dataset.hpp"
#include "../graph/graph.hpp"
#include "../graph/graph_generator.hpp"
#include "../planning/domain.hpp"
#include "../planning/state.hpp"
#include "neighbour_container.hpp"

#include <memory>
#include <string>
#include <vector>

class int_vector_hasher {
 public:
  // https://stackoverflow.com/a/27216842
  std::size_t operator()(std::vector<int> const &vec) const {
    std::size_t seed = vec.size();
    for (auto &i : vec) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }

  // https://stackoverflow.com/a/72073933
  // std::size_t operator()(std::vector<int> const &vec) const {
  //   std::size_t seed = vec.size();
  //   for (auto x : vec) {
  //     x = ((x >> 16) ^ x) * 0x45d9f3b;
  //     x = ((x >> 16) ^ x) * 0x45d9f3b;
  //     x = (x >> 16) ^ x;
  //     seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  //   }
  //   return seed;
  // }
};

namespace feature_generation {
  using Embedding = std::vector<int>;

  class WLFeatures {
   private:
    // configurations [saved]
    std::string package_version;
    std::string graph_representation;
    int iterations;  // equivalently, layers
    std::string prune_features;
    bool multiset_hash;

    // colouring [saved]
    std::unordered_map<std::vector<int>, int, int_vector_hasher> colour_hash;
    std::vector<int> colour_to_layer;
    std::vector<int> colours_to_keep;

    // optional linear weights [saved]
    bool store_weights;
    std::vector<double> weights;

    // helper variables
    std::shared_ptr<planning::Domain> domain;
    std::shared_ptr<graph::GraphGenerator> graph_generator;
    bool collected;
    bool collecting;
    int cur_collecting_layer;
    std::shared_ptr<NeighbourContainer> neighbour_container;

    // runtime statistics; int is faster than long but could cause overflow
    // [i][j] denotes seen count if i=1, and unseen count if i=0
    // for iteration j = 0, ..., iterations - 1
    std::vector<std::vector<long>> seen_colour_statistics;

    // training statistics
    int n_seen_graphs;
    int n_seen_nodes;
    int n_seen_edges;
    std::set<int> seen_initial_colours;

   public:
    WLFeatures(const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string prune_features,
               bool multiset_hash);

    WLFeatures(const std::string &filename);

    /* Feature generation functions */

    // collect training colours
    void collect(const data::Dataset dataset);

    void collect(const planning::State state);

    void collect(const std::vector<graph::Graph> &graphs);

    // set problem for graph generator if it exists
    void set_problem(const planning::Problem &problem);

    // get string representation of WL colours agnostic to the number of collected colours
    std::string get_string_representation(const Embedding &embedding);

    std::string get_string_representation(const planning::State &state);

    // assumes training is done, and returns a feature matrix X
    std::vector<Embedding> embed(const data::Dataset &dataset);

    std::vector<Embedding> embed(const std::vector<graph::Graph> &graphs);

    Embedding embed(const std::shared_ptr<graph::Graph> &graph);

    Embedding embed(const graph::Graph &graph);

    Embedding embed(const planning::State &state);

    /* Prediction functions */

    void set_weights(const std::vector<double> &weights);

    std::vector<double> get_weights() const;

    double predict(const std::shared_ptr<graph::Graph> &graph);

    double predict(const graph::Graph &graph);

    double predict(const planning::State &state);

    /* Statistics functions */

    int get_n_features() const { return colours_to_keep.size(); }

    std::vector<long> get_seen_counts() const { return seen_colour_statistics[1]; };

    std::vector<long> get_unseen_counts() const { return seen_colour_statistics[0]; };

    int get_n_seen_graphs() const { return n_seen_graphs; }

    int get_n_seen_nodes() const { return n_seen_nodes; }

    int get_n_seen_edges() const { return n_seen_edges; }

    int get_n_seen_initial_colours() const { return seen_initial_colours.size(); }

    int get_n_seen_refined_colours() const { return (int)colour_hash.size(); }

    /* Other useful functions */

    std::unordered_map<std::vector<int>, int, int_vector_hasher>
    str_to_int_colour_hash(std::unordered_map<std::string, int> str_colour_hash) const;

    std::unordered_map<std::string, int> int_to_str_colour_hash(
        std::unordered_map<std::vector<int>, int, int_vector_hasher> int_colour_hash) const;

    void save(const std::string &filename);

    std::shared_ptr<planning::Domain> get_domain() const { return domain; }

   private:
    // get hashed colour if it exists, and constructs it if it doesn't
    int get_colour_hash(const std::vector<int> &colour);

    // main WL iteration, updates colours and uses colours_tmp as extra memory for refining
    void refine(const std::shared_ptr<graph::Graph> &graph,
                std::vector<int> &colours,
                std::vector<int> &colours_tmp);

    // convert to ILG
    std::vector<graph::Graph> convert_to_graphs(const data::Dataset dataset);
  };

}  // namespace feature_generation

#endif  // FEATURE_GENERATION_WL_FEATURES_HPP
