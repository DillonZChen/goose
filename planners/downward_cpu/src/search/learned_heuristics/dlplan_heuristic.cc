#include "dlplan_heuristic.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

namespace py = pybind11;

using std::string;

namespace dlplan_heuristic {
DLPlanHeuristic::DLPlanHeuristic(const plugins::Options &opts)
    : Heuristic(opts) {
  // 1. initialise dlplan objects
  string domain_file = opts.get<string>("domain_file");
  string instance_file = opts.get<string>("instance_file");
  auto result = dlplan::state_space::generate_state_space(
      domain_file, instance_file, nullptr, 0, 2147483646, 1);
  _instance_info = (*result.state_space).get_instance_info();
  _factory_info = std::make_shared<dlplan::core::SyntacticElementFactory>(
      dlplan::core::SyntacticElementFactory(
          _instance_info->get_vocabulary_info()));

  // 2. initialise map from FD facts to dlplan atom indices
  std::map<std::string, int> atom_name_to_idx;
  auto atoms = _instance_info->get_atoms();
  for (size_t i = 0; i < atoms.size(); i++) {
    atom_name_to_idx[atoms[i].get_name()] = atoms[i].get_index();
  }

  FactsProxy facts(*task);
  for (FactProxy fact : facts) {
    string name = fact.get_name();

    // Convert from FDR var-val pairs back to propositions
    if (name == "<none of those>") {
      continue;
    } else if (name.substr(0, 5) == "Atom ") {
      name = name.substr(5);
    } else if (name.substr(0, 12) == "NegatedAtom ") {
      // ignore NegatedAtom under open world assumption
      // they only show up due to FDR translation
      continue;
    } else {
      std::cout << "Substring of downward fact does not start with 'Atom ' "
                << "or 'NegatedAtom ': " << name << std::endl;
      exit(-1);
    }

    // erase spaces
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

    _fact_pair_to_atom_idx[fact.get_pair()] = atom_name_to_idx[name];
  }

  // 3. load python model and read weights
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
  std::string model_file = opts.get<string>("model_file");

  // Throw everything into Python code
  std::cout << "Trying to load model from file " << model_file << " ...\n";
  py::module util_module = py::module::import("models.save_load");
  model = util_module.attr("load_ml_model_and_setup")(model_file, domain_file,
                                                      instance_file);

  std::vector<double> weights =
      model.attr("get_weights")().cast<std::vector<double>>();
  std::vector<string> features =
      model.attr("get_features")().cast<std::vector<string>>();

  for (size_t i = 0; i < weights.size(); i++) {
    double weight = weights[i];
    std::string feature = features[i];
    if (feature[0] == 'b') {
      std::shared_ptr<const dlplan::core::Boolean> bool_feature =
          _factory_info->parse_boolean(feature);
      _b_features.push_back(bool_feature);
      _b_weights.push_back(weight);
    } else if (feature[0] == 'n') {
      std::shared_ptr<const dlplan::core::Numerical> numerical_feature =
          _factory_info->parse_numerical(feature);
      _n_features.push_back(numerical_feature);
      _n_weights.push_back(weight);
    } else {
      std::cout << "invalid feature in model\n"
                << "features should start with b_ or n_" << std::endl;
      exit(-1);
    }
  }
}

int DLPlanHeuristic::compute_heuristic(const State &ancestor_state) {
  // 1. convert FD state to dlplan state
  State state = convert_ancestor_state(ancestor_state);

  std::vector<int> atom_indices;
  for (const FactProxy &fact : state) {
    auto fact_pair = fact.get_pair();
    if (_fact_pair_to_atom_idx.count(fact_pair)) {
      atom_indices.push_back(_fact_pair_to_atom_idx[fact_pair]);
    }
  }

  dlplan::core::State dlplan_state(0, _instance_info, atom_indices);

  // 2. compute heuristic
  dlplan::core::DenotationsCaches caches;
  double h = 0;

  for (size_t i = 0; i < _b_features.size(); i++) {
    h += _b_weights[i] * _b_features[i]->evaluate(dlplan_state, caches);
  }

  for (size_t i = 0; i < _n_features.size(); i++) {
    h += _n_weights[i] * _n_features[i]->evaluate(dlplan_state, caches);
  }

  return static_cast<int>(std::round(h));
}

class DLPlanHeuristicFeature
    : public plugins::TypedFeature<Evaluator, DLPlanHeuristic> {
 public:
  DLPlanHeuristicFeature() : TypedFeature("dlplan") {
    document_title("DLPlan heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<std::string>("model_file", "path to trained python model",
                            "default_value");
    add_option<std::string>("domain_file", "path to domain file",
                            "default_value");
    add_option<std::string>("instance_file", "path to instance file",
                            "default_value");

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

static plugins::FeaturePlugin<DLPlanHeuristicFeature> _plugin;

}  // namespace dlplan_heuristic
