#include "pybind_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/strings.h"

#include <algorithm>
#include <chrono>
#include <set>
#include <utility>

namespace py = pybind11;

using namespace std;

namespace pybind_heuristic {
PybindHeuristic::PybindHeuristic(const options::Options &opts,
                                 const std::string py_model_class)
    : Heuristic(opts) {
  // NGOOSE should point to root of this repository.
  // This is handled in a run.py script.
  auto gnn_path = std::getenv("NGOOSE");
  if (!gnn_path) {
    std::cout << "NGOOSE env variable not found. Aborting." << std::endl;
    exit(-1);
  }
  std::string path(gnn_path);
  std::cout << "NGOOSE path is " << path << std::endl;
  if (access(path.c_str(), F_OK) == -1) {
    std::cout << "NGOOSE points to non-existent path. Aborting." << std::endl;
    exit(-1);
  }

  // Append python module directory to the path
  py::module sys = py::module::import("sys");
  sys.attr("path").attr("append")(path);

  // Force all output being printed to stdout. Otherwise INFO logging from
  // python will be printed to stderr, even if it is not an error.
  sys.attr("stderr") = sys.attr("stdout");

  // Read paths
  model_path = opts.get<string>("model_path");
  domain_path = opts.get<string>("domain_path");
  problem_path = opts.get<string>("problem_path");

  std::chrono::duration<double> start_walltime =
      std::chrono::high_resolution_clock::now().time_since_epoch();
  std::cout << "Initialising NGOOSE heuristic." << std::endl;

  load_and_init_model(py_model_class);

  std::chrono::duration<double> end_walltime =
      std::chrono::high_resolution_clock::now().time_since_epoch();
  std::cout << "NGOOSE initialisation time: "
            << end_walltime.count() - start_walltime.count() << "s\n";
}

void PybindHeuristic::load_and_init_model(std::string py_model_class) {
  std::cout << "Trying to load model from file " << model_path << " ..." << std::endl;
  std::cout << "Importing module..." << std::endl;
  pybind11::module model_module = pybind11::module::import("learner");
  std::cout << "Module imported." << std::endl;
  std::cout << "Initialising model object..." << std::endl;
  model = model_module.attr(py_model_class.c_str())();
  std::cout << "Model object initialised." << std::endl;
  std::cout << "Loading..." << std::endl;
  model.attr("load")(model_path);
  std::cout << "Loaded." << std::endl;
  model.attr("dump")();
  std::string nfd_vars_string = g_initial_state().get_str_facts_and_fluents();
  // std::cout << nfd_vars_string << std::endl;
  std::cout << "Setting domain and problem..." << std::endl;
  model.attr("set_domain_problem")(domain_path, problem_path, nfd_vars_string);
  std::cout << "Domain and problem set." << std::endl;

  // facts
  std::unordered_set<std::string> fact_names_set = get_fact_names();

  // fluents
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    std::string name = g_numeric_var_names[i];
    if (name.substr(0, 4) == "PNE " && !contains_substr(name, "new-axiom@") &&
        !contains_substr(name, "!") && !contains_substr(name, "total-cost()")) {
      fluent_indices.push_back(i);
      fluent_names.push_back(name.substr(4));
    }
  }

  // important to do this to preserve ordering of fluents that is used in NFD
  model.attr("set_fluents")(fluent_names);

  // map facts to their predicates and objects
  fact_to_pred_objects =
      model.attr("get_fact_to_pred_objects")(fact_names_set)
          .cast<std::unordered_map<std::string,
                                   std::pair<std::string, std::vector<std::string>>>>();
  std::cout << "Base model initialisation complete!" << std::endl;
}

std::unordered_set<std::string> PybindHeuristic::get_fact_names() {
  std::unordered_set<std::string> fact_names_set;
  for (size_t i = 0; i < g_fact_names.size(); ++i) {
    std::vector<std::string> fdr_var_i_val_names;
    std::vector<bool> fdr_var_i_is_true;
    for (size_t j = 0; j < g_fact_names[i].size(); ++j) {
      std::string fact_name = g_fact_names[i][j];
      bool is_true;
      if (fact_name != "<none of those>" && !contains_substr(fact_name, "derived!") &&
          !contains_substr(fact_name, "new-axiom@")) {
        if (contains_substr(fact_name, "NegatedAtom ")) {
          fact_name = fact_name.substr(12);
          is_true = false;
        } else if (contains_substr(fact_name, "Atom ")) {
          fact_name = fact_name.substr(5);
          is_true = true;
        } else {
          fact_name = "";
          is_true = false;
        }
      } else {
        fact_name = "";
        is_true = false;
      }
      fdr_var_i_val_names.push_back(fact_name);
      fdr_var_i_is_true.push_back(is_true);
    }
    fdr_pair_to_name.push_back(fdr_var_i_val_names);
    fdr_pair_to_is_true.push_back(fdr_var_i_is_true);
  }
  for (auto const &val_names : fdr_pair_to_name) {
    for (auto const &fact_name : val_names) {
      if (fact_name.size() > 0) {
        fact_names_set.insert(fact_name);
      }
    }
  }
  return fact_names_set;
}

PybindHeuristic::~PybindHeuristic() {}
}  // namespace pybind_heuristic
