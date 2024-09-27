#include "lrnn_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include <utility>

using namespace std;

namespace lrnn_heuristic {
LrnnHeuristic::LrnnHeuristic(const options::Options &opts)
    : PybindHeuristic(opts, "LrnnHeuristic") {}

LrnnHeuristic::~LrnnHeuristic() {}

ap_float LrnnHeuristic::compute_heuristic(const GlobalState &global_state) {
  std::vector<std::string> bool_vals =
      global_state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true);
  std::vector<ap_float> num_vals = global_state.get_num_values(fluent_indices);

  // global_state.dump_readable();

  // this is only called once from batch_eager_search so no need to optimise
  ap_float h = model.attr("evaluate")(bool_vals, num_vals).cast<ap_float>();

  return h;
}

std::vector<ap_float>
LrnnHeuristic::compute_heuristic_batch(const std::vector<GlobalState> &states) {
  std::vector<ap_float> hs(states.size());

  for (size_t i = 0; i < states.size(); ++i) {
    hs[i] = compute_heuristic(states[i]);
  }

  return hs;
}

void LrnnHeuristic::print_statistics() {
  std::cout << "LrnnHeuristic datalog time: "
            << model.attr("get_datalog_time")().cast<double>() << "s\n";
  std::cout << "LrnnHeuristic nn time: " 
            << model.attr("get_nn_time")().cast<double>() << "s\n";
}

static Heuristic *_parse(OptionParser &parser) {
  parser.document_synopsis("LRNN heuristic", "");
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
    return new LrnnHeuristic(opts);
}

static Plugin<Heuristic> _plugin("lrnn", _parse);
}  // namespace lrnn_heuristic
