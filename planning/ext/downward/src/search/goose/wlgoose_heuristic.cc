#include "wlgoose_heuristic.h"

#include "../plugins/plugin.h"
#include "../utils/logging.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace wlgoose_heuristic {
  WlGooseHeuristic::WlGooseHeuristic(const std::string &model_file,
                                     const std::shared_ptr<AbstractTask> &transform,
                                     bool cache_estimates,
                                     const std::string &description,
                                     utils::Verbosity verbosity)
      : Heuristic(transform, cache_estimates, description, verbosity) {
    model = load_feature_generator(model_file);

    // Some boilerplate to set up WLPlan domain and problem
    const planning::Domain domain = *(model->get_domain());
    const std::map<FactPair, wl_utils::PredArgsString> &mapper =
        wl_utils::get_fd_fact_to_pred_args_map(task);
    auto [fd_fact_map, problem] = wl_utils::construct_wlplan_problem(domain, mapper, task_proxy);
    fd_fact_to_wlplan_atom = fd_fact_map;

    model->set_problem(problem);
  }

  int WlGooseHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);

    planning::State wlplan_state = wl_utils::to_wlplan_state(state, fd_fact_to_wlplan_atom);

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
      add_heuristic_options_to_feature(*this, "wlgoose");

      document_language_support("action costs", "ignored by design");
      document_language_support("conditional effects", "supported");
      document_language_support("axioms", "supported");

      document_property("admissible", "no");
      document_property("consistent", "no");
      document_property("safe", "yes");
      document_property("preferred operators", "no");
    }

    virtual shared_ptr<WlGooseHeuristic> create_component(const plugins::Options &opts,
                                                          const utils::Context &) const override {
      return plugins::make_shared_from_arg_tuples<WlGooseHeuristic>(
          opts.get<std::string>("model_file"), get_heuristic_arguments_from_options(opts));
    }
  };

  static plugins::FeaturePlugin<WlGooseHeuristicFeature> _plugin;
}  // namespace wlgoose_heuristic
