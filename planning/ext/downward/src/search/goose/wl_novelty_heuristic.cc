#include "wl_novelty_heuristic.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"
#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
#include "../utils/logging.h"

#include <cassert>

using namespace std;

namespace wl_novelty_heuristic {
  // construction and destruction
  WlNoveltyHeuristic::WlNoveltyHeuristic(const shared_ptr<AbstractTask> &transform,
                                         bool cache_estimates,
                                         const string &description,
                                         utils::Verbosity verbosity,
                                         const int iterations,
                                         const std::string &graph_representation)
      : Heuristic(transform, cache_estimates, description, verbosity) {

    // Construct predicates
    std::cout << "Collecting predicates..." << std::endl;
    std::map<FactPair, wl_utils::PredArgsString> fd_fact_to_pred_args =
        wl_utils::get_fd_fact_to_pred_args_map(task);
    std::set<planning::Predicate> predicates_set;
    for (const auto &[_, pred_args] : fd_fact_to_pred_args) {
      const std::string &predicate_name = pred_args.first;
      const int arity = pred_args.second.size();
      predicates_set.insert(planning::Predicate(predicate_name, arity));
    }
    std::vector<planning::Predicate> predicates(predicates_set.begin(), predicates_set.end());

    // Construct domain
    std::cout << "Initialising domain..." << std::endl;
    planning::Domain domain = planning::Domain("domain", predicates);
    std::unordered_map<std::string, planning::Predicate> name_to_predicate;
    for (const auto &pred : domain.predicates) {
      name_to_predicate[pred.name] = pred;
    }

    // Construct problem
    std::cout << "Initialising problem..." << std::endl;
    const std::map<FactPair, wl_utils::PredArgsString> &mapper =
        wl_utils::get_fd_fact_to_pred_args_map(task);
    auto [fd_fact_map, problem] = wl_utils::construct_wlplan_problem(domain, mapper, task_proxy);
    fd_fact_to_wlplan_atom = fd_fact_map;

    // Set up WLF generator
    std::cout << "Initialising WLF generator..." << std::endl;
    model = std::make_shared<feature_generator::WLFeatures>(
        domain, graph_representation, iterations, "none", true);
    model->set_problem(problem);
    model->be_quiet();

    std::cout << "WL Novelty Heuristic intialised!" << std::endl;
  }

  int WlNoveltyHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);

    int n = model->get_n_colours();
    planning::State wlplan_state = wl_utils::to_wlplan_state(state, fd_fact_to_wlplan_atom);
    model->collect(wlplan_state);

    // note that values are non-positive, lower the better for guiding exploration
    return n - model->get_n_colours();
  }

  class WLFFHeuristicFeature : public plugins::TypedFeature<Evaluator, WlNoveltyHeuristic> {
   public:
    WLFFHeuristicFeature() : TypedFeature("wlnov") {
      document_title("WL novelty heuristic");

      add_heuristic_options_to_feature(*this, "wlnov");
      add_option<int>("iterations", "WL iterations", "4");
      add_option<std::string>("graph", "Graph representation", "ilg");

      document_language_support("action costs", "ignored by design");
      document_language_support("conditional effects", "ignored by design");
      document_language_support("axioms", "ignored by design");

      document_property("admissible", "no");
      document_property("consistent", "no");
      document_property("safe", "yes");
      document_property("preferred operators", "no");
    }

    virtual shared_ptr<WlNoveltyHeuristic> create_component(const plugins::Options &opts,
                                                            const utils::Context &) const override {
      return plugins::make_shared_from_arg_tuples<WlNoveltyHeuristic>(
          get_heuristic_arguments_from_options(opts),
          opts.get<int>("iterations"),
          opts.get<std::string>("graph"));
    }
  };

  static plugins::FeaturePlugin<WLFFHeuristicFeature> _plugin;
}  // namespace wl_novelty_heuristic
