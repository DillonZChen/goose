#include "goose_heuristic.h"

#include "../plugins/plugin.h"
#include <iostream>
#include <fstream> 

namespace py = pybind11;

using std::string;

namespace goose_heuristic {
GooseHeuristic::GooseHeuristic(const plugins::Options &opts)
    : Heuristic(opts) {
    
    // Add GOOSE submodule to the python path
    auto gnn_path = std::getenv("GOOSE");
    if (!gnn_path) {
        std::cout << "GOOSE environment variable not found. Aborting." << std::endl;
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

    std::string config_path;
    switch (opts.get<int>("graph"))
    {
    case 0:
      config_path = "slg";
      break;
    case 1:
      config_path = "flg";
      break;
    case 2:
      config_path = "llg";
      break;
    case 3:
      config_path = "glg";
      break;
    default:
        std::cout << "Unknown enum of graph representation" << std::endl;
        exit(-1);
    }

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

    std::cout << "Trying to load model from file " << model_path << " ..." << std::endl;
    py::module util_module = py::module::import("util.save_load");
    model = util_module.attr("load_model_and_setup")(model_path, domain_file, instance_file);
    std::cout << "Loaded model!" << std::endl;
    model.attr("dump_model_stats")();
    lifted_state_input = model.attr("lifted_state_input")().cast<bool>();
}

int GooseHeuristic::compute_heuristic(const State &ancestor_state) {
    // TODO replace with GOOSE heuristic
    State state = convert_ancestor_state(ancestor_state);
    int unsatisfied_goal_count = 0;

    for (FactProxy goal : task_proxy.get_goals()) {
        const VariableProxy var = goal.get_variable();
        if (state[var] != goal) {
            ++unsatisfied_goal_count;
        }
    }
    return unsatisfied_goal_count;
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

        // requires modifying the parser to accept paths with / . - or numerical characters
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