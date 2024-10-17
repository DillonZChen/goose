#include "batch_eager_search.h"

#include "search_common.h"

#include "../evaluation_context.h"
#include "../globals.h"
#include "../heuristic.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../pruning_method.h"
#include "../successor_generator.h"
#include "../utils/planvis.h"
#include "../utils/timer.h"

#include "../open_lists/open_list_factory.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <set>
#include <sstream>

using namespace std;

// copied from heuristic.h
static constexpr ap_float DEAD_END = std::numeric_limits<ap_float>::min();
static constexpr ap_float NO_VALUE = std::numeric_limits<ap_float>::quiet_NaN();

namespace batch_eager_search {
BatchEagerSearch::BatchEagerSearch(const Options &opts)
    : SearchEngine(opts),
      open_list(
          opts.get<shared_ptr<OpenListFactory>>("open")->create_state_open_list()) {
  heuristic = opts.get_list<ScalarEvaluator *>("evals")[0];
}

void BatchEagerSearch::initialize() {
  cout << "Conducting best first search without"
       << " reopening closed nodes, (real) bound = " << bound << endl;
  assert(open_list);

  const GlobalState &initial_state = g_initial_state();
  EvaluationContext eval_context(initial_state, 0, true, &statistics);

  statistics.inc_evaluated_states();

  if (open_list->is_dead_end(eval_context)) {
    cout << "Initial state is a dead end." << endl;
    exit(0);
  } else {
    print_initial_h_values(eval_context);
    if (search_progress.check_progress(eval_context))
      print_checkpoint_line(0);
    SearchNode node = search_space.get_node(initial_state);
    node.open_initial();

    open_list->insert(eval_context, initial_state.get_id());
  }

  best_h = get_initial_h_values(eval_context)[0];

  if (check_goal_and_set_plan(initial_state)) {
    std::cout << "Initial state is a goal state. Terminating..." << std::endl;
    exit(-1);
  }
}

void BatchEagerSearch::print_checkpoint_line(int g) const {
  cout << "[g=" << g << ", ";
  statistics.print_basic_statistics();
  cout << "]" << endl;
}

void BatchEagerSearch::print_statistics() const {
  statistics.print_detailed_statistics();
  search_space.print_statistics();
}

SearchStatus BatchEagerSearch::step() {
  pair<SearchNode, bool> n = fetch_next_node();
  if (!n.second) {
    return FAILED;
  }
  SearchNode node = n.first;

  GlobalState s = node.get_state();

  vector<const GlobalOperator *> applicable_ops;
  set<const GlobalOperator *> preferred_ops;

  g_successor_generator->generate_applicable_ops(s, applicable_ops);

  // collect new successor nodes
  std::vector<GlobalState> succ_states;

  for (const GlobalOperator *op : applicable_ops) {
    if ((node.get_real_g() + op->get_cost()) >= bound)
      continue;

    GlobalState succ_state = g_state_registry->get_successor_state(s, *op);
    statistics.inc_generated();

    SearchNode succ_node = search_space.get_node(succ_state);

    if (succ_node.is_new()) {
      succ_node.open(node, op);

      if (check_goal_and_set_plan(succ_state)) {
        return SOLVED;
      }

      succ_states.push_back(succ_state);
    }
  }

  // batch evaluate and add to queue
  size_t n_succs = succ_states.size();
  if (n_succs > 0) {
    ap_float tmp_best_h = best_h;
    std::vector<ap_float> hs = heuristic->compute_result_batch(succ_states);
    statistics.inc_evaluated_states(n_succs);
    statistics.inc_evaluations(n_succs);

    for (size_t i = 0; i < n_succs; i++) {
      GlobalState succ_state = succ_states[i];
      SearchNode succ_node = search_space.get_node(succ_state);
      ap_float h = hs[i];

      if (h == DEAD_END) {
        succ_node.mark_as_dead_end();
        statistics.inc_dead_ends();
        continue;
      }

      open_list->insert(h, succ_state.get_id());
      tmp_best_h = std::min(tmp_best_h, h);
    }

    if (tmp_best_h < best_h) {
      best_h = tmp_best_h;
      cout << "New best h=" << best_h << " ";
      print_checkpoint_line(node.get_g() + 1);
    }
  }

  return IN_PROGRESS;
}

pair<SearchNode, bool> BatchEagerSearch::fetch_next_node() {
  while (true) {
    if (open_list->empty()) {
      cout << "Completely explored state space -- no solution!" << endl;
      // HACK! HACK! we do this because SearchNode has no default/copy constructor
      SearchNode dummy_node = search_space.get_node(g_initial_state());
      return make_pair(dummy_node, false);
    }
    vector<ap_float> last_key_removed;
    StateID id = open_list->remove_min(nullptr);
    // TODO is there a way we can avoid creating the state here and then
    //      recreate it outside of this function with node.get_state()?
    //      One way would be to store GlobalState objects inside SearchNodes
    //      instead of StateIDs
    GlobalState s = g_state_registry->lookup_state(id);
    if (violates_global_constraint(s))
      continue;
    SearchNode node = search_space.get_node(s);

    if (node.is_closed())
      continue;

    node.close();
    assert(!node.is_dead_end());
    statistics.inc_expanded();
    return make_pair(node, true);
  }
}

void BatchEagerSearch::save_plan_if_necessary() const {
  if (found_solution()) {
    save_plan(get_plan());
    heuristic->print_statistics();
  }
}

void BatchEagerSearch::dump_search_space() const { search_space.dump(); }

static SearchEngine *_parse_greedy(OptionParser &parser) {
  parser.document_synopsis("Batch greedy search (eager)", "");
  parser.document_note("Closed nodes", "Closed node are not re-opened");

  parser.add_list_option<ScalarEvaluator *>("evals", "scalar evaluators");
  parser.add_list_option<Heuristic *>(
      "preferred", "not used, but required to implement this option to run", "[]");
  parser.add_option<ap_float>(
      "boost", "not used, but required to implement this option to run", "0");

  SearchEngine::add_options_to_parser(parser);

  Options opts = parser.parse();
  opts.verify_list_non_empty<ScalarEvaluator *>("evals");

  BatchEagerSearch *engine = nullptr;
  if (!parser.dry_run()) {
    opts.set("open", search_common::create_greedy_open_list_factory(opts));
    engine = new BatchEagerSearch(opts);
  }
  return engine;
}

static Plugin<SearchEngine> _plugin_greedy("batch_eager_greedy", _parse_greedy);
}  // namespace batch_eager_search
