#include "gnn_py_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include <utility>

using namespace std;

namespace gnn_py_heuristic {
GnnPyHeuristic::GnnPyHeuristic(const options::Options &opts)
    : PybindHeuristic(opts, "DeepLearningModel") {}

GnnPyHeuristic::~GnnPyHeuristic() {}

ap_float GnnPyHeuristic::compute_heuristic(const GlobalState &global_state) {
  std::vector<std::string> bool_vals =
      global_state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true);
  std::vector<ap_float> num_vals = global_state.get_num_values(fluent_indices);

  // global_state.dump_readable();
  
  ap_float h = model.attr("evaluate")(bool_vals, num_vals).cast<ap_float>();

  return h;
}

std::vector<ap_float>
GnnPyHeuristic::compute_heuristic_batch(const std::vector<GlobalState> &states) {
  std::vector<std::vector<std::string>> bool_vals;
  std::vector<std::vector<ap_float>> num_vals;
  for (const GlobalState &state : states) {
    bool_vals.push_back(state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true));
    num_vals.push_back(state.get_num_values(fluent_indices));
  }

  std::vector<ap_float> h =
      model.attr("evaluate_batch_py")(bool_vals, num_vals).cast<std::vector<ap_float>>();

  return h;
}

void GnnPyHeuristic::print_statistics() {
  std::cout << "GnnPyHeuristic graph time: " << model.attr("get_graph_time")().cast<double>() << "s\n";
  std::cout << "GnnPyHeuristic data_loader time: " << model.attr("get_dataloader_time")().cast<double>() << "s\n";
  std::cout << "GnnPyHeuristic gnn time: " << model.attr("get_gnn_time")().cast<double>() << "s\n";
}

static Heuristic *_parse(OptionParser &parser) {
  parser.document_synopsis("GNN heuristic", "");
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
    return new GnnPyHeuristic(opts);
}

static Plugin<Heuristic> _plugin("gnn_py", _parse);
}  // namespace gnn_py_heuristic
