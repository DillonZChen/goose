#include "iterative_horizon.h"

#include "../evaluation_context.h"
#include "../evaluators/sum_evaluator.h"
#include "../evaluators/weighted_evaluator.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../search_engines/search_common.h"
#include "ip_compilation.h"

using namespace std;
using namespace ip_compilation;

// TODO: put default evaluator

IterativeHorizon::IterativeHorizon(const Options &opts)
    : SearchEngine(opts), timer(1800) {
  cout << "ip compilation" << endl;

  const std::shared_ptr<AbstractTask> task(get_task_from_options(opts));
  model = new IPCompilation(opts, task);

  //    OptionParser
  //    h_parser("operatorcounting([delete_relaxation_constraints([basic])],CPLEX,lp)",true);
  //    Options h_opts = h_parser.parse();

  ScalarEvaluator *h = opts.get<ScalarEvaluator *>("eval");

  GlobalState initial_state = g_initial_state();
  EvaluationContext state(initial_state);
  initial_t = h->compute_result(state).get_h_value();
  cout << "initial time horizon " << initial_t + 1 << endl;
}

IterativeHorizon::~IterativeHorizon() {}

void IterativeHorizon::initialize() {
  cout << "/ninside initialize" << endl;
  const std::shared_ptr<AbstractTask> task(g_root_task());
  // calculate heursistic
  // initial model
  ap_float min_action_cost = model->get_min_action_cost();
  bool forget = model->contains_forget();
  int t = !forget ? initial_t : initial_t * 2;
  model->initialize(t);
  // iterative method;
  bool solved = false;
  int n_iterations = 0;
  bool last_iteration = false;
  int increment = 1;
  while (!solved) {
    n_iterations++;
    if (timer.is_expired()) break;
    if (last_iteration) cout << "optimality check " << increment << endl;
    double plan_cost =
        model->compute_plan(t + increment, timer.get_remaining_time());
    if (plan_cost >= 0) {
      if (plan_cost == initial_t) {
        solved = true;
        continue;
      }
      if (last_iteration) {
        solved = true;
        continue;
      }
      double to_check = plan_cost / min_action_cost;
      if (forget) to_check = to_check * 2;
      if (to_check == t) {
        solved = true;
        continue;
      }
      last_iteration = true;
      increment = to_check - t;
    }
    t++;
  }
  if (solved) model->print_plan();
  cout << "Iterations: " << n_iterations << endl;
  cout << "Time: " << timer.get_elapsed_time() << " s" << endl;
}

static SearchEngine *_parse(OptionParser &parser) {
  parser.document_synopsis("ip compilation", "");
  ip_compilation::add_model_option_to_parser(parser);
  lp::add_lp_solver_option_to_parser(parser);
  lp::add_lp_constraint_option_to_parser(parser);
  parser.add_option<ScalarEvaluator *>("eval", "evaluator for h-value");
  parser.add_option<shared_ptr<AbstractTask>>(
      "transform",
      "Optional task transformation for the heuristic. "
      "Currently only adapt_costs is available.",
      OptionParser::NONE);
  SearchEngine::add_options_to_parser(parser);
  Options opts = parser.parse();

  if (parser.dry_run())
    return nullptr;
  else
    return new IterativeHorizon(opts);
}

static Plugin<SearchEngine> _plugin("ip_compilation", _parse);

void ip_compilation::add_model_option_to_parser(OptionParser &parser) {
  parser.document_note(
      "Note",
      "o utse an IP compilation, you must build the planner with LP support. "
      "See LPBuildInstructions.");
  vector<string> ip_models;
  vector<string> ip_models_doc;
  ip_models.push_back("SB");
  ip_models_doc.push_back("state-based compilation");
  ip_models.push_back("SC");
  ip_models_doc.push_back("state-change compilation");
  ip_models.push_back("SAS");
  ip_models_doc.push_back("SAS+, state-change compilation");
  //    parser.add_enum_option(
  //                           "ipmodel",
  //                           ip_models,
  //                           "compilation model",
  //                           "SB",
  //                           ip_models_doc);
  parser.add_list_option<shared_ptr<IPConstraintGenerator>>(
      "ipmodel",
      "methods that generate constraints over operator counting variables");
}
