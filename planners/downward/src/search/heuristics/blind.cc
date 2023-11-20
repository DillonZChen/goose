#include "blind.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

using std::string;

namespace blind_heuristic {
Blind::Blind(const plugins::Options &opts)
    : Heuristic(opts) {
}

int Blind::compute_heuristic(const State &ancestor_state) {
  // Convert state into Python object and feed into Goose.
  if (task_properties::is_goal_state(task_proxy, ancestor_state))
    return 0;
  else
    return 1;
}

std::vector<int> Blind::compute_heuristic_batch(const std::vector<State> &ancestor_states) {
  std::vector<int> ret;
  for (size_t i = 0; i < ret.size(); i++) {
    ret.push_back(compute_heuristic(ancestor_states[i]));
  }
  return ret;
}

class BlindHeuristicFeature : public plugins::TypedFeature<Evaluator, Blind> {
public:
    BlindHeuristicFeature() : TypedFeature("blind") {
        document_title("blind heuristic");
        document_synopsis("TODO");

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

static plugins::FeaturePlugin<BlindHeuristicFeature> _plugin;

} // namespace blind_heuristic
