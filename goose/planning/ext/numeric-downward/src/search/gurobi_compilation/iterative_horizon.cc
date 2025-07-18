#include "iterative_horizon.h"

#include <cmath>

#include "../evaluation_context.h"
#include "../evaluators/sum_evaluator.h"
#include "../evaluators/weighted_evaluator.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../search_engines/search_common.h"
#include "ip_compilation.h"

using namespace std;
using namespace gurobi_ip_compilation;

// TODO: put default evaluator

GurobiIterativeHorizon::GurobiIterativeHorizon(const Options &opts)
    : SearchEngine(opts),
      initial_h(-1),
      current_t(1),
      iterations(0),
      last_iteration(false),
      model(new GurobiIPCompilation(opts, get_task_from_options(opts))),
      h_evaluator(opts.get<ScalarEvaluator *>("eval")) {
  cout << "ip compilation" << endl;
}

GurobiIterativeHorizon::~GurobiIterativeHorizon() { delete h_evaluator; }

void GurobiIterativeHorizon::initialize() {
  cout << "initialize" << endl;
  GlobalState initial_state = g_initial_state();
  EvaluationContext state(initial_state);
  EvaluationResult result = h_evaluator->compute_result(state);

  if (result.is_infinite()) {
    cout << "Initial state is a dead end." << endl;
  } else {
    initial_h = result.get_h_value();
    current_t = std::ceil<int>(initial_h);
    current_t = std::max(current_t, 1);
    cout << "initial time horizon " << current_t << endl;
  }
  model->initialize(current_t);
}

SearchStatus GurobiIterativeHorizon::step() {
  if (initial_h == -1) return FAILED;

  if (last_iteration) model->add_sequence_constraint();

  std::cout << "horizon = " << current_t << std::endl;
  ++iterations;
  ap_float plan_cost = model->compute_plan();

  if (plan_cost >= 0) {
    Plan plan = model->extract_plan();
    plan_cost = 0;

    for (auto action : plan)
      plan_cost += action->get_cost();

    ap_float min_action_cost = model->get_min_action_cost();
    ap_float optimal_cost_bound = current_t * min_action_cost;
    ap_float min_plan_cost_diff = model->get_min_plan_cost_diff();
    if (plan_cost - initial_h < min_plan_cost_diff
        || plan_cost - optimal_cost_bound < min_plan_cost_diff
        || last_iteration) {
      set_plan(plan);
      model->print_statistics();
      std::cout << "Iterations: " << iterations << std::endl;
      return SOLVED;
    }
    current_t = std::ceil(plan_cost / min_action_cost);
    last_iteration = true;
  } else {
    ++current_t;
  }
  model->update(current_t);

  return IN_PROGRESS;
}

static SearchEngine *_parse(OptionParser &parser) {
  parser.document_synopsis("Gurobi ip compilation", "");
  gurobi_ip_compilation::add_model_option_to_parser(parser);
  parser.add_option<ScalarEvaluator *>("eval", "evaluator for h-value");
  parser.add_option<shared_ptr<AbstractTask>>(
      "transform",
      "Optional task transformation for the heuristic. "
      "Currently only adapt_costs is available.",
      OptionParser::NONE);
  parser.add_option<int>("threads", "Number of threads used by Gurobi", "1");
  parser.add_option<bool>("lazy_constraints", "Whether to add lazy constraints",
                          "false");
  parser.add_option<bool>("user_cuts", "Whether to add user cuts", "false");
  parser.add_option<bool>("add_one_time_step", "Whether to add a cut only at that time step",
                          "false");
  parser.add_option<int>("max_num_cuts", "Maximum number of cuts at each node",
                         "1");
  parser.add_option<bool>("linear_effects",
                          "Wheter to use domains with linear effects", "false");
  parser.add_option<bool>("lazy_mutex",
                          "Lazily generate mutex constraints", "false");
  SearchEngine::add_options_to_parser(parser);
  Options opts = parser.parse();

  if (parser.dry_run())
    return nullptr;
  else
    return new GurobiIterativeHorizon(opts);
}

static Plugin<SearchEngine> _plugin("gurobi_ip_compilation", _parse);

void gurobi_ip_compilation::add_model_option_to_parser(OptionParser &parser) {
  parser.document_note(
      "Note",
      "o utse an IP compilation, you must build the planner with LP support. "
      "See LPBuildInstructions.");
  vector<string> ip_models;
  vector<string> ip_models_doc;
  ip_models.push_back("SAS");
  ip_models_doc.push_back("SAS+, SAS+ state-change compilation");
  parser.add_list_option<shared_ptr<GurobiIPConstraintGenerator>>(
      "gurobi_ipmodel",
      "methods that generate constraints over operator counting variables");
}
