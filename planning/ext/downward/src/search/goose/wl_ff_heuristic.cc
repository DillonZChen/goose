#include "wl_ff_heuristic.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"
#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
#include "../utils/logging.h"

#include <cassert>

using namespace std;

namespace wl_ff_heuristic {
  // construction and destruction
  WLFFHeuristic::WLFFHeuristic(tasks::AxiomHandlingType axioms,
                               const shared_ptr<AbstractTask> &transform,
                               bool cache_estimates,
                               const string &description,
                               utils::Verbosity verbosity,
                               const int iterations,
                               const std::string &graph_representation)
      : ff_heuristic::FFHeuristic(axioms, transform, cache_estimates, description, verbosity) {

    // Construct predicates
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
    planning::Domain domain = planning::Domain("domain", predicates);
    std::unordered_map<std::string, planning::Predicate> name_to_predicate;
    for (const auto &pred : domain.predicates) {
      name_to_predicate[pred.name] = pred;
    }

    // Construct problem
    const std::map<FactPair, wl_utils::PredArgsString> &mapper =
        wl_utils::get_fd_fact_to_pred_args_map(task);
    auto [fd_fact_map, problem] = wl_utils::construct_wlplan_problem(domain, mapper, task_proxy);
    fd_fact_to_wlplan_atom = fd_fact_map;

    // Set up WLF generator
    model = std::make_shared<feature_generator::WLFeatures>(
        domain, graph_representation, iterations, "none", true);
  }

  int WLFFHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);
    int h_ff = ff_heuristic::FFHeuristic::compute_heuristic(state);

    int n = model->get_n_colours();
    planning::State wlplan_state = wl_utils::to_wlplan_state(state, fd_fact_to_wlplan_atom);
    model->collect(wlplan_state);
    n = model->get_n_colours();

    std::cout << n << " " << h_ff << std::endl;

    return h_ff;
  }

  class WLFFHeuristicFeature : public plugins::TypedFeature<Evaluator, WLFFHeuristic> {
   public:
    WLFFHeuristicFeature() : TypedFeature("wlff") {
      document_title("WL FF heuristic");

      relaxation_heuristic::add_relaxation_heuristic_options_to_feature(*this, "ff");
      add_option<int>("iterations", "WL iterations", "4");
      add_option<std::string>("graph", "Graph representation", "ilg");

      document_language_support("action costs", "supported");
      document_language_support("conditional effects", "supported");
      document_language_support("axioms",
                                "supported (in the sense that the planner won't complain -- "
                                "handling of axioms might be very stupid "
                                "and even render the heuristic unsafe)");

      document_property("admissible", "no");
      document_property("consistent", "no");
      document_property("safe", "yes for tasks without axioms");
      document_property("preferred operators", "yes");
    }

    virtual shared_ptr<WLFFHeuristic> create_component(const plugins::Options &opts,
                                                       const utils::Context &) const override {
      return plugins::make_shared_from_arg_tuples<WLFFHeuristic>(
          relaxation_heuristic::get_relaxation_heuristic_arguments_from_options(opts),
          opts.get<int>("iterations"),
          opts.get<std::string>("graph"));
    }
  };

  static plugins::FeaturePlugin<WLFFHeuristicFeature> _plugin;
}  // namespace wl_ff_heuristic
