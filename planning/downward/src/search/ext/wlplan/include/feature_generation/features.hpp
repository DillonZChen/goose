#ifndef FEATURE_GENERATION_FEATURES_HPP
#define FEATURE_GENERATION_FEATURES_HPP

#include "../data/dataset.hpp"
#include "../graph/graph.hpp"
#include "../graph/graph_generator.hpp"
#include "../planning/domain.hpp"
#include "../planning/state.hpp"
#include "neighbour_container.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#define UNSEEN_COLOUR -1

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
};

namespace feature_generation {
  using Embedding = std::vector<double>;

  class Features {
   protected:
    // configurations [saved]
    std::string package_version;
    std::string feature_name;
    std::string graph_representation;
    int iterations;  // equivalently, layers
    std::string pruning;
    bool multiset_hash;

    // colouring [saved]
    std::unordered_map<std::vector<int>, int, int_vector_hasher> colour_hash;
    std::unordered_map<int, std::vector<int>> reverse_hash;
    std::unordered_map<int, int> colour_to_layer;
    std::vector<std::set<int>> layer_to_colours;

    // optional linear weights [saved]
    bool store_weights;
    std::vector<double> weights;

    // helper variables
    std::shared_ptr<planning::Domain> domain;
    std::shared_ptr<graph::GraphGenerator> graph_generator;
    bool collected;
    bool collapse_pruned;
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

    // convert to ILG
    std::vector<graph::Graph> convert_to_graphs(const data::Dataset dataset);

    // get hashed colour if it exists, and constructs it if it doesn't
    int get_colour_hash(const std::vector<int> &colour);

    // reformat colour hash based on remap input, throwing out colours that are mapped to -1
    void reformat_colour_hash(const std::map<int, int> &remap);

    // main collection body
    virtual void collect_main(const std::vector<graph::Graph> &graphs) = 0;

   public:
    Features(const std::string feature_name,
             const planning::Domain &domain,
             std::string graph_representation,
             int iterations,
             std::string pruning,
             bool multiset_hash);

    Features(const std::string &filename);

    virtual ~Features() = default;

    /* Feature generation functions */

    // collect training colours
    void collect_from_dataset(const data::Dataset dataset);
    void collect(const std::vector<graph::Graph> &graphs);

    // embedding assumes training is done, and returns a feature matrix X
    std::vector<Embedding> embed_dataset(const data::Dataset &dataset);
    std::vector<Embedding> embed_graphs(const std::vector<graph::Graph> &graphs);
    Embedding embed_graph(const graph::Graph &graph);
    Embedding embed_state(const planning::State &state);
    virtual Embedding embed(const std::shared_ptr<graph::Graph> &graph) = 0;

    /* Prediction functions */

    double predict(const std::shared_ptr<graph::Graph> &graph);
    double predict(const graph::Graph &graph);
    double predict(const planning::State &state);

    void set_weights(const std::vector<double> &weights);
    std::vector<double> get_weights() const;

    /* Configuration functions */
    std::string get_feature_name() const { return feature_name; }
    std::string get_graph_representation() const { return graph_representation; }
    int get_iterations() const { return iterations; }
    std::string get_pruning() { return pruning; }
    void set_pruning(const std::string &pruning) { this->pruning = pruning; }

    /* Util functions */
    // get string representation of WL colours agnostic to the number of collected colours
    std::string get_string_representation(const Embedding &embedding);
    std::string get_string_representation(const planning::State &state);

    // set problem for graph generator if it exists
    void set_problem(const planning::Problem &problem);

    // conversion between vectors and strings
    std::unordered_map<std::vector<int>, int, int_vector_hasher>
    str_to_int_colour_hash(std::unordered_map<std::string, int> str_colour_hash) const;
    std::unordered_map<std::string, int> int_to_str_colour_hash(
        std::unordered_map<std::vector<int>, int, int_vector_hasher> int_colour_hash) const;

    // statistics functions
    int get_n_features() const { return colour_hash.size(); }
    std::vector<long> get_seen_counts() const { return seen_colour_statistics[1]; };
    std::vector<long> get_unseen_counts() const { return seen_colour_statistics[0]; };
    int get_n_seen_graphs() const { return n_seen_graphs; }
    int get_n_seen_nodes() const { return n_seen_nodes; }
    int get_n_seen_edges() const { return n_seen_edges; }
    int get_n_seen_initial_colours() const { return seen_initial_colours.size(); }
    int get_n_seen_refined_colours() const { return (int)colour_hash.size(); }
    std::vector<long> get_layer_to_n_colours() const;
    void print_init_colours() const;

    std::shared_ptr<planning::Domain> get_domain() const { return domain; }

    void save(const std::string &filename);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURES_HPP
