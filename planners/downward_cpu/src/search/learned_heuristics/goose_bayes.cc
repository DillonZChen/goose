#include "goose_bayes.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

using std::string;

namespace goose_bayes {

GooseBayes::GooseBayes(const plugins::Options &opts) : goose_wl::WLGooseHeuristic(opts) {}

// void GooseBayes::print_statistics() const {
//   log << "Number of seen " << wl_algorithm_ << " colours: " << cnt_seen_colours << std::endl;
//   log << "Number of unseen " << wl_algorithm_ << " colours: " << cnt_unseen_colours << std::endl;
//   // for (auto const &[r, s, h] : std_ratio_pairs) {
//   //   std::cout << r << " " << s << " " << h << std::endl;
//   // }
// }

int GooseBayes::compute_heuristic(const State &ancestor_state) {
  // int cur_seen_colours = cnt_seen_colours;
  // int cur_unseen_colours = cnt_unseen_colours;

  // step 1.
  std::shared_ptr<CGraph> graph = state_to_graph(ancestor_state);
  // step 2.
  std::vector<int> feature = wl_feature(graph);
  // step 3.
  // std::pair<double, double> h_std_pair =
  //     model.attr("predict_h_with_std")(feature).cast<std::pair<double, double>>();
  // int h = static_cast<int>(round(h_std_pair.first));
  // double std = h_std_pair.second;

  int h = model.attr("predict_h")(feature).cast<int>();
  return h;

  // cur_seen_colours = cnt_seen_colours - cur_seen_colours;
  // cur_unseen_colours = cnt_unseen_colours - cur_unseen_colours;
  // double ratio_seen_colours =
  //     static_cast<double>(cur_seen_colours) / (cur_unseen_colours + cur_seen_colours);

  // std_ratio_pairs.insert(std::make_tuple(ratio_seen_colours, std, h));

  return h;
}

class GooseBayesFeature : public plugins::TypedFeature<Evaluator, GooseBayes> {
 public:
  GooseBayesFeature() : TypedFeature("bayes_model") {
    document_title("GOOSE optimised WL bayes heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<std::string>(
        "model_file", "path to trained model data in the form of a .pkl file", "default_value");
    add_option<std::string>("domain_file", "Path to the domain file.", "default_file");
    add_option<std::string>("instance_file", "Path to the instance file.", "default_file");

    Heuristic::add_options_to_feature(*this);

    document_language_support("action costs", "not supported");
    document_language_support("conditional effects", "not supported");
    document_language_support("axioms", "not supported");

    document_property("admissible", "no");
    document_property("consistent", "no");
    document_property("safe", "yes");
    document_property("preferred operators", "no");
  }
};

static plugins::FeaturePlugin<GooseBayesFeature> _plugin;

}  // namespace goose_bayes
