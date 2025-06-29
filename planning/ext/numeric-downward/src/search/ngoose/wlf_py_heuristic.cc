#include "wlf_py_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include <utility>

using namespace std;

namespace wlf_py_heuristic {
WlfPyHeuristic::WlfPyHeuristic(const options::Options &opts)
    : PybindHeuristic(opts, "FeatureGenerationModel") {}

WlfPyHeuristic::~WlfPyHeuristic() {}

ap_float WlfPyHeuristic::compute_heuristic(const GlobalState &global_state) {
  std::vector<std::string> bool_vals =
      global_state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true);
  std::vector<ap_float> num_vals = global_state.get_num_values(fluent_indices);

  // global_state.dump_readable();

  bool is_deadend = model.attr("predict_deadend")(bool_vals, num_vals).cast<bool>();
  if (is_deadend) {
    return DEAD_END;
  }

  ap_float h = model.attr("evaluate")(bool_vals, num_vals).cast<ap_float>();

  return h;
}

std::vector<ap_float>
WlfPyHeuristic::compute_heuristic_batch(const std::vector<GlobalState> &states) {
  std::vector<ap_float> hs;

  for (const GlobalState &state : states) {
    hs.push_back(compute_heuristic(state));
  }

  return hs;
}

static Heuristic *_parse(OptionParser &parser) {
  parser.document_synopsis("WLF heuristic through pybind", "");
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
    return new WlfPyHeuristic(opts);
}

static Plugin<Heuristic> _plugin("wlf_py", _parse);
}  // namespace wlf_py_heuristic
