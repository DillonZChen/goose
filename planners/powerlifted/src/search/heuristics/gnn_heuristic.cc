#include "gnn_heuristic.h"

namespace py = pybind11;

GNNHeuristic::GNNHeuristic(const Task &task,
                           const std::string &model_path,
                           const std::string &domain_file,
                           const std::string &instance_file) {
    
    // std::cout << "Trying to load model from file " << model_path << " ..." << std::endl;
    // try {
    //     model = torch::jit::load(model_path);
    //     std::cout << "Model loaded successfully!" << std::endl;
    // } catch (const c10::Error &e) {
    //     std::cerr << "Error loading the model!!!" << std::endl;
    //     exit(-1);
    // }

    // Add learning heuristics submodule to the python path
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


    std::cout << "Trying to load model from file " << model_path << " ..." << std::endl;
    py::module util_module = py::module::import("util.save_load");
    model = util_module.attr("load_and_setup_gnn_model")(model_path, domain_file, instance_file);
    std::cout << "Loaded model!" << std::endl;
    model.attr("dump_model_stats")();
    lifted_state_input = model.attr("lifted_state_input")().cast<bool>();
}

pybind11::list GNNHeuristic::lifted_state_to_python(const DBState &s, const Task &task) {
  //  task.dump_state(s);
    py::list py_state;

    const auto& nullary_atoms = s.get_nullary_atoms();
    for (size_t j = 0; j < nullary_atoms.size(); ++j) {
        if (nullary_atoms[j]) {
            py_state.append(py::make_tuple(task.predicates[j].get_name(), py::list()));
        }
    }
    const auto& relations = s.get_relations();
    for (size_t i = 0; i < relations.size(); ++i) {
        for (auto const &tuple : relations[i].tuples) {
            py::list args;
            for (auto obj : tuple) {
                args.append(task.objects[obj].get_name());
            }
            py_state.append(py::make_tuple(task.predicates[i].get_name(), args));
        }
    }
    return py_state;
}

pybind11::list GNNHeuristic::grounded_state_to_python(const DBState &s, const Task &task) {
  //  task.dump_state(s);
    py::list py_state;
    
    const auto& nullary_atoms = s.get_nullary_atoms();
    for (size_t j = 0; j < nullary_atoms.size(); ++j) {
        if (nullary_atoms[j]) {
          py_state.append("("+task.predicates[j].get_name()+")");
        }
    }
    const auto& relations = s.get_relations();
    for (size_t i = 0; i < relations.size(); ++i) {
        for (auto &tuple : relations[i].tuples) {
            std::string fact = "(" + task.predicates[i].get_name();
            for (auto obj : tuple) {
                fact += " " + task.objects[obj].get_name();
            }
            fact += ")";
            py_state.append(fact);
        }
    }
    return py_state;
}

int GNNHeuristic::compute_heuristic(const DBState &s, const Task &task) {
    if (task.is_goal(s)) {
        return 0;
    }
    py::object h;
    if (lifted_state_input) {
      h = model.attr("h")(lifted_state_to_python(s, task));
    } else {
      h = model.attr("h")(grounded_state_to_python(s, task));
    }
    int ret = h.cast<int>();
    return ret;
}

std::vector<int> GNNHeuristic::compute_heuristic_batch(const std::vector<DBState> &states, const Task &task) {
    py::list py_states;
    if (lifted_state_input) {
      for (const auto& s: states) {
          py_states.append(lifted_state_to_python(s, task));
      }
    } else {
      for (const auto& s: states) {
          py_states.append(grounded_state_to_python(s, task));
      }
    }
    py::object heuristics = model.attr("h_batch")(py_states);
    std::vector<int> ret = heuristics.cast<std::vector<int>>();
    for (size_t i = 0; i < ret.size(); i++) {
        if (task.is_goal(states[i])) {
            ret[i] = 0;
        } else {
            ret[i] = std::max(1, ret[i]);
        }
    }
    return ret;
}
