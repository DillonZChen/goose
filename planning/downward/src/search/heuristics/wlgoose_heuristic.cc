#include "wlgoose_heuristic.h"

#include "../plugins/plugin.h"
#include "../utils/logging.h"

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

  WlGooseHeuristic::WlGooseHeuristic(const plugins::Options &opts) : Heuristic(opts) {
    model = std::make_shared<feature_generation::WLFeatures>(opts.get<std::string>("model_file"));

    const planning::Domain domain = *(model->get_domain());
    std::unordered_map<std::string, planning::Predicate> name_to_predicate;
    for (const auto &pred : domain.predicates) {
      name_to_predicate[pred.name] = pred;
    }

    std::unordered_set<planning::Object> objects;

    // Preprocess Downward's FDR var-val pairs and map to WLPlan atoms. See :fd_fact_to_wlplan_atom:
    FactsProxy facts(*task);
    for (FactProxy fact : facts) {
      string name = fact.get_name();

      // Convert from FDR var-val pairs back to propositions
      if (name == "<none of those>" || name.substr(0, 12) == "NegatedAtom ") {
        continue;
      } else if (name.substr(0, 5) == "Atom ") {
        name = name.substr(5);
      } else {
        std::cout << "Error: substring of downward fact does not start with 'Atom ': "
                  << "or 'NegatedAtom '" << name << std::endl;
        exit(-1);
      }

      std::pair<std::string, std::vector<std::string>> pred_args = fd_fact_to_pred_args(name);
      std::string predicate_name = pred_args.first;
      std::vector<planning::Object> args = pred_args.second;

      for (planning::Object arg : args) {
        objects.insert(arg);
      }

      if (name_to_predicate.count(predicate_name)) {
        planning::Atom wlplan_atom = planning::Atom(name_to_predicate.at(predicate_name), args);
        fd_fact_to_wlplan_atom.insert({fact.get_pair(), wlplan_atom});
      }
    }

    /* Construct a WLPlan Problem from Downward */

    // Sort objects into vector
    std::vector<planning::Object> objects_vec_sorted(objects.begin(), objects.end());
    std::sort(objects_vec_sorted.begin(), objects_vec_sorted.end());

    // Deal with goals
    std::vector<planning::Atom> positive_goals;
    std::vector<planning::Atom> negative_goals;

    for (FactProxy goal : task_proxy.get_goals()) {
      string name = goal.get_name();
      bool positive;

      // Convert from FDR var-val pairs back to propositions
      if (name == "<none of those>") {
        continue;
      } else if (name.substr(0, 12) == "NegatedAtom ") {
        name = name.substr(12);
        positive = false;
      } else if (name.substr(0, 5) == "Atom ") {
        name = name.substr(5);
        positive = true;
      } else {
        std::cout << "Error: substring of downward fact does not start with 'Atom ': "
                  << "or 'NegatedAtom '" << name << std::endl;
        exit(-1);
      }

      std::pair<std::string, std::vector<std::string>> pred_args = fd_fact_to_pred_args(name);
      std::string predicate_name = pred_args.first;
      std::vector<planning::Object> args = pred_args.second;
      planning::Atom atom = planning::Atom(name_to_predicate.at(predicate_name), args);

      if (positive) {
        positive_goals.push_back(atom);
      } else {
        negative_goals.push_back(atom);
      }
    }

    planning::Problem problem =
        planning::Problem(domain, objects_vec_sorted, positive_goals, negative_goals);
    model->set_problem(problem);
  }

  int WlGooseHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);

    planning::State wlplan_state;

    for (const FactProxy &fact : state) {
      if (fd_fact_to_wlplan_atom.count(fact.get_pair())) {
        wlplan_state.push_back(fd_fact_to_wlplan_atom.at(fact.get_pair()));
      }
    }

    double h = model->predict(wlplan_state);
    int h_round = static_cast<int>(std::round(h));

    return h_round;
  }

  class WlGooseHeuristicFeature : public plugins::TypedFeature<Evaluator, WlGooseHeuristic> {
   public:
    WlGooseHeuristicFeature() : TypedFeature("wlgoose") {
      document_title("WL-GOOSE Heuristic");

      // https://github.com/aibasel/downward/pull/170 for string options
      add_option<std::string>("model_file", "path to trained model", "default_value");

      Heuristic::add_options_to_feature(*this);

      document_language_support("action costs", "ignored by design");
      document_language_support("conditional effects", "supported");
      document_language_support("axioms", "supported");

      document_property("admissible", "no");
      document_property("consistent", "no");
      document_property("safe", "yes");
      document_property("preferred operators", "no");
    }
  };

  static plugins::FeaturePlugin<WlGooseHeuristicFeature> _plugin;
}  // namespace wlgoose_heuristic
