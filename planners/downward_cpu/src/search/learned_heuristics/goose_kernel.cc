#include "goose_kernel.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

using std::string;

namespace goose_kernel {

GooseKernel::GooseKernel(const plugins::Options &opts) : goose_wl::WLGooseHeuristic(opts) {}

int GooseKernel::predict(const std::vector<int> &feature) {
  int h = std::round(model.attr("predict_h")(feature).cast<double>());
  return h;
}

int GooseKernel::compute_heuristic(const State &ancestor_state) {
  // step 1.
  std::shared_ptr<CGraph> graph = state_to_graph(ancestor_state);
  // step 2.
  std::vector<int> feature = wl_feature(graph);
  // step 3.
  int h = predict(feature);
  return h;
}

class GooseKernelFeature : public plugins::TypedFeature<Evaluator, GooseKernel> {
 public:
  GooseKernelFeature() : TypedFeature("kernel_model") {
    document_title("GOOSE optimised WL kernel heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<std::string>("model_file", "path to trained python model", "default_value");
    add_option<std::string>("domain_file", "Path to the domain file.", "default_file");
    add_option<std::string>("instance_file", "Path to the instance file.", "default_file");

    Heuristic::add_options_to_feature(*this);

    document_language_support("action costs", "not supported");
    document_language_support("conditional effects", "not supported");
    document_language_support("axioms", "not supported");

    document_property("admissible", "no");
    document_property("consistent", "no");
    document_property("safe", "yes");
    document_property("preferred operators", "no");
  }
};

static plugins::FeaturePlugin<GooseKernelFeature> _plugin;

}  // namespace goose_kernel
