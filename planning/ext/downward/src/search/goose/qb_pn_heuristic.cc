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
                               const std::shared_ptr<Evaluator> base_heuristic)
      : QbHeuristic(transform, cache_estimates, description, verbosity, base_heuristic) {}

  int QbPnHeuristic::compute_heuristic(const State &ancestor_state) {
    EvaluationContext eval_context(ancestor_state, 0, false, &statistics);
    int h = eval_context.get_evaluator_value_or_infinity(base_heuristic.get());
    if (h == EvaluationResult::INFTY)
      return DEAD_END;

    int nov_h = 0;
    int non_h = 0;

    State state = convert_ancestor_state(ancestor_state);
    for (const FactProxy &fact : state) {
      const std::pair<int, int> pair = fact.get_int_pair();
      bool in_map = feat_to_lowest_h.count(pair) > 0;
      if (!in_map || h < feat_to_lowest_h[pair]) {
        feat_to_lowest_h[pair] = h;
        nov_h -= 1;
      } else if (in_map && h > feat_to_lowest_h[pair]) {
        non_h += 1;
      }
    }

    return nov_h < 0 ? nov_h : non_h;
  }

  class QbPnHeuristicFeature : public plugins::TypedFeature<Evaluator, QbPnHeuristic> {
   public:
    QbPnHeuristicFeature() : TypedFeature("qbpn") {
      document_title("Goal count heuristic");

      add_option<shared_ptr<Evaluator>>("eval", "Heuristic for novelty calculation");
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
      return std::make_shared<QbPnHeuristic>(opts.get<shared_ptr<AbstractTask>>("transform"),
                                             opts.get<bool>("cache_estimates"),
                                             opts.get<std::string>("description"),
                                             opts.get<utils::Verbosity>("verbosity"),
                                             opts.get<shared_ptr<Evaluator>>("eval"));
    }
  };

  static plugins::FeaturePlugin<QbPnHeuristicFeature> _plugin;
}  // namespace qb_heuristic
