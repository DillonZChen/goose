#include "goose_gnn_heuristic.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

#include <fstream>
#include <iostream>

namespace py = pybind11;

using std::string;

namespace goose_gnn_heuristic {
GooseHeuristic::GooseHeuristic(const plugins::Options &opts)
    : goose_heuristic::GooseHeuristic(opts) {
  initialise_model(opts);
  initialise_facts();
}

void GooseHeuristic::initialise_model(const plugins::Options &opts) {
  // Add GOOSE submodule to the python path
  auto gnn_path = std::getenv("GOOSE");
  if (!gnn_path) {
    std::cout << "GOOSE env variable not found. Aborting." << std::endl;
    exit(-1);
  }
  std::string path(gnn_path);
  std::cout << "GOOSE path is " << path << std::endl;
  if (access(path.c_str(), F_OK) == -1) {
    std::cout << "GOOSE points to non-existent path. Aborting." << std::endl;
    exit(-1);
  }

  // Append python module directory to the path
  py::module sys = py::module::import("sys");
  sys.attr("path").attr("append")(path);

  // Force all output being printed to stdout. Otherwise INFO logging from
  // python will be printed to stderr, even if it is not an error.
  sys.attr("stderr") = sys.attr("stdout");

  // Read paths
  std::string model_type = opts.get<string>("model_type");
  std::string model_path = opts.get<string>("model_path");
  std::string domain_file = opts.get<string>("domain_file");
  std::string instance_file = opts.get<string>("instance_file");

  // Throw everything into Python code depending on model type
  std::cout << "Trying to load model from file " << model_path << " ...\n";
  py::module util_module = py::module::import("models.save_load");
  if (model_type == "gnn") {
    model = util_module.attr("load_gnn_model_and_setup")(model_path, domain_file, instance_file);
    std::cout << "Loaded model!" << std::endl;
    model.attr("dump_model_stats")();
  } else if (model_type == "kernel") {
    model = util_module.attr("load_ml_model_and_setup")(model_path, domain_file, instance_file);
    std::cout << "Loaded model!" << std::endl;
  } else {
    std::cout << "Model type " << model_type << " not supported\n";
    exit(-1);
  }

  lifted_goose = model.attr("lifted_state_input")().cast<bool>();
}

int GooseHeuristic::compute_heuristic(const State &ancestor_state) {
  // Convert state into Python object and feed into Goose.
  GooseState goose_state = fd_state_to_goose_state(ancestor_state);
  int h = model.attr("h")(goose_state).cast<int>();
  return h;
}

class GooseHeuristicFeature : public plugins::TypedFeature<Evaluator, GooseHeuristic> {
 public:
  GooseHeuristicFeature() : TypedFeature("goose") {
    document_title("GOOSE heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<string>("model_type", "gnn or kernel", "default_value");
    add_option<string>("model_path", "path to trained model or model weights", "default_value");
    add_option<string>("domain_file", "Path to the domain file.", "default_file");
    add_option<string>("instance_file", "Path to the instance file.", "default_file");

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

static plugins::FeaturePlugin<GooseHeuristicFeature> _plugin;

}  // namespace goose_gnn_heuristic
