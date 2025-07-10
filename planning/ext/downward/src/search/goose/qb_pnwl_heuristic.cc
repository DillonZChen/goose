#include "qb_pnwl_heuristic.h"

#include "../ext/wlplan/include/feature_generator/feature_generators/wl.hpp"
#include "../heuristics/additive_heuristic.h"
#include "../heuristics/ff_heuristic.h"
#include "../heuristics/goal_count_heuristic.h"
#include "../plugins/plugin.h"
#include "../utils/logging.h"

#include <iostream>
using namespace std;

namespace qb_heuristic {
  QbPnWlHeuristic::QbPnWlHeuristic(const std::shared_ptr<AbstractTask> &transform,
                                   bool cache_estimates,
                                   const std::string &description,
                                   utils::Verbosity verbosity,
                                   const std::shared_ptr<Heuristic> &heuristic)
      : QbHeuristic(transform, cache_estimates, description, verbosity, heuristic) {
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
    model = std::make_shared<feature_generator::WLFeatures>(domain, "ilg", 2, "none", true);
    model->set_problem(problem);
    model->be_quiet();

    std::cout << "PNWL Novelty Heuristic intialised!" << std::endl;
  }

  int QbPnWlHeuristic::compute_heuristic(const State &ancestor_state) {
    int h = original_heuristic->compute_heuristic(ancestor_state);

    int nov_h = 0;
    int non_h = 0;

    State state = convert_ancestor_state(ancestor_state);

    // WL part
    planning::State wl_state = wl_utils::to_wlplan_state(state, fd_fact_to_wlplan_atom);
    model->collect(wl_state);
    std::vector<double> embed = model->embed_state(wl_state);  // TODO optimise this
    for (int i = 0; i < (int)embed.size(); i++) {
      if (embed[i] == 0) {  // feature not present, their values do not matter
        continue;
      }
      std::pair<int, int> feat = std::make_pair(i, (int)embed[i]);
      bool in_map = feat_to_lowest_h.count(feat) > 0;
      if (!in_map || h < feat_to_lowest_h[feat]) {
        feat_to_lowest_h[feat] = h;
        nov_h -= 1;
      } else if (in_map && h > feat_to_lowest_h[feat]) {
        non_h += 1;
      }
    }

    // PN part
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

  class QbPnWlHeuristicFeature : public plugins::TypedFeature<Evaluator, QbPnWlHeuristic> {
   public:
    QbPnWlHeuristicFeature() : TypedFeature("qbpnwl") {
      document_title("Goal count heuristic");

      add_option<std::string>("h", "base heuristic to use, choose from {gc, add, ff}", "ff");
      add_heuristic_options_to_feature(*this, "qbpnwl");

      document_language_support("action costs", "ignored by design");
      document_language_support("conditional effects", "supported");
      document_language_support("axioms", "supported");

      document_property("admissible", "no");
      document_property("consistent", "no");
      document_property("safe", "yes");
      document_property("preferred operators", "no");
    }

    virtual shared_ptr<QbPnWlHeuristic> create_component(const plugins::Options &opts,
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

      return std::make_shared<QbPnWlHeuristic>(opts.get<shared_ptr<AbstractTask>>("transform"),
                                               opts.get<bool>("cache_estimates"),
                                               opts.get<std::string>("description"),
                                               opts.get<utils::Verbosity>("verbosity"),

                                               heuristic);
    }
  };

  static plugins::FeaturePlugin<QbPnWlHeuristicFeature> _plugin;
}  // namespace qb_heuristic
