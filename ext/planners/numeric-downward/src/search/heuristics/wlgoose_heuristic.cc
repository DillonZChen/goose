#include "wlgoose_heuristic.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../utils/logging.h"
#include "../utils/strings.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace wlgoose_heuristic {
  std::pair<std::string, std::vector<std::string>> fd_fact_to_pred_args(std::string &name) {
    // Replace all occurrences of '(' and ')' by ' '
    std::replace(name.begin(), name.end(), '(', ' ');
    std::replace(name.begin(), name.end(), ')', ' ');
    // Remove occurrences of ','
    name.erase(std::remove(name.begin(), name.end(), ','), name.end());
    // Trim string
    if (std::isspace(name[0]))
      name.erase(0, 1);
    if (std::isspace(name.back()))
      name.erase(name.end() - 1, name.end());

    std::istringstream iss(name);
    std::string token;
    std::string predicate_name = "";
    std::vector<planning::Object> args;

    while (std::getline(iss, token, ' ')) {
      if (predicate_name == "") {
        predicate_name = token;
      } else {
        args.push_back(token);
      }
    }

    return {predicate_name, args};
  }

  WlGooseHeuristic::WlGooseHeuristic(const options::Options &opts) : Heuristic(opts) {
    std::string model_path = opts.get<std::string>("model_path");
    std::string domain_path = opts.get<std::string>("domain_path");
    std::string problem_path = opts.get<std::string>("problem_path");
    model = load_feature_generator(model_path);

    // WLPlan domain
    const planning::Domain domain = *(model->get_domain());
    std::unordered_map<std::string, planning::Predicate> name_to_predicate;
    std::unordered_map<std::string, planning::Function> name_to_function;
    for (const auto &pred : domain.predicates) {
      name_to_predicate[pred.name] = pred;
    }
    for (const auto &func : domain.functions) {
      name_to_function[func.name] = func;
    }

    // WLPlan problem
    py::object parse_problem = py::module::import("wlplan.planning").attr("parse_problem");
    // use python to help parse the numeric goals, but below for ignoring statics
    planning::Problem problem =
        py::cast<planning::Problem>(parse_problem(domain_path, problem_path));
    model->set_problem(problem);
    values = problem.get_fluent_values();

    // atoms
    fdr_pair_to_wlplan_atom.clear();
    std::cout << "Preprocessing atoms...";
    std::string nfd_fact_name, fact_name;
    for (size_t i = 0; i < g_fact_names.size(); ++i) {
      std::vector<std::shared_ptr<planning::Atom>> atoms;
      for (size_t j = 0; j < g_fact_names[i].size(); ++j) {
        nfd_fact_name = g_fact_names[i][j];
        fact_name = "";
        if (nfd_fact_name != "<none of those>" && !contains_substr(nfd_fact_name, "derived!") &&
            !contains_substr(nfd_fact_name, "new-axiom@")) {
          if (contains_substr(nfd_fact_name, "NegatedAtom ")) {
            // pass
          } else if (contains_substr(nfd_fact_name, "Atom ")) {  // keep only positive facts
            fact_name = nfd_fact_name.substr(5);
          }
        }

        if (fact_name == "") {
          atoms.push_back(nullptr);
        } else {
          std::pair<std::string, std::vector<std::string>> pred_args =
              fd_fact_to_pred_args(fact_name);
          std::string predicate_name = pred_args.first;
          std::vector<planning::Object> args = pred_args.second;

          if (!name_to_predicate.count(predicate_name)) {
            std::cout << "Fact " << fact_name << " does not have a proper predicate." << std::endl;
            exit(-1);
          }

          planning::Atom wlplan_atom = planning::Atom(name_to_predicate.at(predicate_name), args);
          atoms.push_back(std::make_shared<planning::Atom>(wlplan_atom));
        }
      }
      fdr_pair_to_wlplan_atom.push_back(atoms);
    }
    std::cout << " done!" << std::endl;

    // fluents
    std::cout << "Preprocessing fluents...";
    for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
      std::string name = g_numeric_var_names[i];
      if (name.substr(0, 4) == "PNE " && !contains_substr(name, "new-axiom@") &&
          !contains_substr(name, "!") && !contains_substr(name, "total-cost()")) {
        std::string fluent_name = name.substr(4);
        int index = problem.get_fluent_id(fluent_name);
        nfd_to_wlplan_fluent_index.push_back(index);
        nfd_fluent_indices.push_back(i);
      }
    }
    std::cout << " done!" << std::endl;
  }

  planning::State WlGooseHeuristic::nfd_to_wlplan_state(const GlobalState &global_state) {
    std::vector<std::shared_ptr<planning::Atom>> atoms;
    std::vector<int> fdr_pairs = global_state.get_fdr_pairs();
    std::shared_ptr<planning::Atom> atom;

    int j;
    for (int i = 0; i < (int)fdr_pairs.size(); i++) {
      j = fdr_pairs[i];
      atom = fdr_pair_to_wlplan_atom[i][j];
      if (atom != nullptr) {
        atoms.push_back(atom);
      }
    }

    // update non static fluents
    std::vector<ap_float> nfd_values = global_state.get_num_values(nfd_fluent_indices);
    for (int i = 0; i < (int)nfd_values.size(); i++) {
      values[nfd_to_wlplan_fluent_index[i]] = nfd_values[i];
    }

    planning::State wlplan_state = planning::State(atoms, values);
    return wlplan_state;
  }

  ap_float WlGooseHeuristic::compute_heuristic(const GlobalState &global_state) {
    planning::State state = nfd_to_wlplan_state(global_state);
    double h = model->predict(state);
    int h_round = static_cast<int>(std::round(h));
    return h_round;
  }

  static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("WLF heuristic optimised", "");
    parser.document_language_support("action costs", "no");
    parser.document_language_support("conditional effects", "no");
    parser.document_language_support("axioms", "no");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    parser.add_option<string>("model_path", "path to model file", "_");
    parser.add_option<string>("domain_path", "path to domain file", "_");
    parser.add_option<string>("problem_path", "path to problem file", "_");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
      return 0;
    else
      return new WlGooseHeuristic(opts);
  }

  static Plugin<Heuristic> _plugin("wlgoose", _parse);
}  // namespace wlgoose_heuristic
