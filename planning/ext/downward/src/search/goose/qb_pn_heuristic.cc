#include "qb_pn_heuristic.h"

#include "../heuristics/additive_heuristic.h"
#include "../heuristics/ff_heuristic.h"
#include "../heuristics/goal_count_heuristic.h"
#include "../plugins/plugin.h"
#include "../utils/logging.h"

#include <iostream>
using namespace std;

namespace qb_heuristic {
  QbPnHeuristic::QbPnHeuristic(const std::shared_ptr<AbstractTask> &transform,
                               bool cache_estimates,
                               const std::string &description,
                               utils::Verbosity verbosity,
                               const std::shared_ptr<Heuristic> &heuristic)
      : QbHeuristic(transform, cache_estimates, description, verbosity, heuristic) {}

  int QbPnHeuristic::compute_heuristic(const State &ancestor_state) {
    int h = original_heuristic->compute_heuristic(ancestor_state);

    int nov_h = 0;
    int non_h = 0;

    State state = convert_ancestor_state(ancestor_state);
    for (const FactProxy &fact : state) {
      FactPair pair = fact.get_pair();
      bool in_map = fact_pair_to_lowest_h.count(pair) > 0;
      if (!in_map || h < fact_pair_to_lowest_h[pair]) {
        fact_pair_to_lowest_h[pair] = h;
        nov_h -= 1;
      } else if (in_map && h > fact_pair_to_lowest_h[pair]) {
        non_h += 1;
      }
    }

    return nov_h < 0 ? nov_h : non_h;
  }

  class QbPnHeuristicFeature : public plugins::TypedFeature<Evaluator, QbPnHeuristic> {
   public:
    QbPnHeuristicFeature() : TypedFeature("qbpn") {
      document_title("Goal count heuristic");

      add_option<std::string>("h", "base heuristic to use, choose from {gc, add, ff}", "ff");
      add_heuristic_options_to_feature(*this, "qbpn");

      document_language_support("action costs", "ignored by design");
      document_language_support("conditional effects", "supported");
      document_language_support("axioms", "supported");

      document_property("admissible", "no");
      document_property("consistent", "no");
      document_property("safe", "yes");
      document_property("preferred operators", "no");
    }

    virtual shared_ptr<QbPnHeuristic> create_component(const plugins::Options &opts,
                                                       const utils::Context &) const override {

      std::string base_h_name = opts.get<std::string>("h");
      std::cout << "Initialising base heuristic h=" << base_h_name << std::endl;
      std::shared_ptr<Heuristic> heuristic;
      if (base_h_name == "gc") {
        heuristic = plugins::make_shared_from_arg_tuples<goal_count_heuristic::GoalCountHeuristic>(
            get_heuristic_arguments_from_options(opts));
      } else if (base_h_name == "add") {
        heuristic = std::make_shared<additive_heuristic::AdditiveHeuristic>(
            tasks::AxiomHandlingType::APPROXIMATE_NEGATIVE,
            opts.get<shared_ptr<AbstractTask>>("transform"),
            true,
            "add",
            utils::Verbosity::SILENT);
      } else if (base_h_name == "ff") {
        heuristic = std::make_shared<ff_heuristic::FFHeuristic>(
            tasks::AxiomHandlingType::APPROXIMATE_NEGATIVE,
            opts.get<shared_ptr<AbstractTask>>("transform"),
            true,
            "ff",
            utils::Verbosity::SILENT);
      } else {
        cerr << "Unknown base heuristic: " << base_h_name << endl;
        exit(1);
      }
      std::cout << "Base heuristic initialised" << std::endl;

      return std::make_shared<QbPnHeuristic>(opts.get<shared_ptr<AbstractTask>>("transform"),
                                             opts.get<bool>("cache_estimates"),
                                             opts.get<std::string>("description"),
                                             opts.get<utils::Verbosity>("verbosity"),

                                             heuristic);
    }
  };

  static plugins::FeaturePlugin<QbPnHeuristicFeature> _plugin;
}  // namespace qb_heuristic
