#include "goose_heuristic.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

#include <iostream>
#include <fstream> 

namespace py = pybind11;

using std::string;

namespace goose_heuristic {
GooseHeuristic::GooseHeuristic(const plugins::Options &opts)
    : Heuristic(opts) {
    
  initialise_model(opts);
  initialise_fact_strings();
    
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

  // A really disgusting hack because FeaturePlugin cannot parse string options
  std::string config_path;
  switch (opts.get<int>("graph"))
  {
  case 0: config_path = "slg"; break;
  case 1: config_path = "flg"; break;
  case 2: config_path = "llg"; break;
  case 3: config_path = "glg"; break;
  default:
      std::cout << "Unknown enum of graph representation" << std::endl;
      exit(-1);
  }

  // Parse paths from file at config_path
  std::string model_path;
  std::string domain_file;
  std::string instance_file;

  std::string line;
  std::ifstream config_file(config_path);
  int file_line = 0;
  while (getline(config_file, line)) {
    switch (file_line) {
      case 0:
        model_path = line;
        break;
      case 1:
        domain_file = line;
        break;
      case 2:
        instance_file = line;
        break;
      default:
        std::cout << "config file " << config_path 
                  << " must only have 3 lines" << std::endl;
        exit(-1);
    }
    file_line++;
  }
  config_file.close(); 

  // Throw everything into Python code
  std::cout << "Trying to load model from file " << model_path << " ...\n";
  py::module util_module = py::module::import("util.save_load");
  model = util_module.attr("load_model_and_setup")(model_path, 
                                                   domain_file, instance_file);
  std::cout << "Loaded model!" << std::endl;
  model.attr("dump_model_stats")();

  // Not implemented for lifted graph representations
  if (model.attr("lifted_state_input")().cast<bool>()) {
    std::cout << "Lifted representation not supported for goose-downward\n";
    exit(-1);
  }
}

void GooseHeuristic::initialise_fact_strings() {
  FactsProxy facts(*task);
  for (FactProxy fact : facts) {
    string name = fact.get_name();

    // Convert from FDR var-val pairs back to propositions
    if (name == "<none of those>") {
      continue;
    } else {
      if (name.substr(0, 5) == "Atom ") {
        name = name.substr(5);
      } else if (name.substr(0, 12) == "NegatedAtom ") {
        continue;
      } else {
        std::cout << "Substring of downward fact does not start with 'Atom ': "
                  << "or 'NegatedAtom '"
                  << name << std::endl;
        exit(-1);
      }
    }

    // replace all occurrences of '(' and ')' by ' '
    std::replace(name.begin(), name.end(), '(', ' ');
    std::replace(name.begin(), name.end(), ')', ' ');

    // Remove occurrences of ','
    name.erase(std::remove(name.begin(), name.end(), ','), name.end());

    // Trim string
    if (std::isspace(name[0])) {
      name.erase(0, 1);
    }
    if (std::isspace(name.back())) {
      name.erase(name.end() - 1, name.end());
    }

    // Add parentheses around string
    name = "(" + name + ")";
    fact_to_goose_string.insert({fact.get_pair(), name});

    #ifndef NDEBUG
      std::cout << name << " ";
    #endif
  }

  #ifndef NDEBUG
    std::cout << std::endl;
  #endif
}

py::list GooseHeuristic::list_to_goose_state(const State &ancestor_state) {
  State state = convert_ancestor_state(ancestor_state);

  py::list goose_state;
  for (FactProxy fact : state) {
    goose_state.append(fact_to_goose_string[fact.get_pair()]);
  }
  return goose_state;
}

int GooseHeuristic::compute_heuristic(const State &ancestor_state) {
  // Convert state into Python object and feed into Goose.
  py::list goose_state = list_to_goose_state(ancestor_state);
  int h = model.attr("h")(goose_state).cast<int>();
  return h;
}

std::vector<int> GooseHeuristic::compute_heuristic_batch(const std::vector<State> &ancestor_states) {
  py::list py_states;
  for (const auto& state: ancestor_states) {
      py_states.append(list_to_goose_state(state));
  }
  py::object heuristics = model.attr("h_batch")(py_states);
  std::vector<int> ret = heuristics.cast<std::vector<int>>();
  for (size_t i = 0; i < ret.size(); i++) {
    if (task_properties::is_goal_state(task_proxy, ancestor_states[i]))
      ret[i] = 0;
    else
      ret[i] = std::max(1, ret[i]);
  }
  return ret;
}

class GooseHeuristicFeature : public plugins::TypedFeature<Evaluator, GooseHeuristic> {
public:
    GooseHeuristicFeature() : TypedFeature("goose") {
        document_title("GOOSE heuristic");
        document_synopsis("TODO");

        add_option<int>(
            "graph",
            "0: slg, 1: flg, 2: llg, 3: glg",
            "-1");

        // add_option does not work with <string>

        // add_option<string>(
        //     "model_path",
        //     "path to trained model weights of file type .dt",
        //     "default_value.dt");

        // add_option<string>(
        //     "domain_file",
        //     "Path to the domain file.",
        //     "default_file.pddl");

        // add_option<string>(
        //     "instance_file",
        //     "Path to the instance file.",
        //     "default_file.pddl");

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

} // namespace goose_heuristic