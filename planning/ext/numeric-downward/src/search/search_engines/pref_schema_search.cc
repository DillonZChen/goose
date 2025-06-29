#include "pref_schema_search.h"

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
static constexpr ap_float MAX_VALUE = std::numeric_limits<ap_float>::max();
static constexpr ap_float NO_VALUE = std::numeric_limits<ap_float>::quiet_NaN();

namespace pref_schema_search {
PrefSchemaSearch::PrefSchemaSearch(const Options &opts) : SearchEngine(opts) {
  heuristic = dynamic_cast<wlf_heuristic::WlfHeuristic *>(
      opts.get_list<ScalarEvaluator *>("evals")[0]);
  int n_models = heuristic->n_heuristics();  // h_model + n_schemata * p_model
  open_lists = std::vector<GooseOpenList<StateID> *>();
  for (int i = 0; i < n_models; i++) {
    GooseOpenList<StateID> *open_list = new GooseOpenList<StateID>();
    open_lists.push_back(open_list);
  }
  popped = std::unordered_set<StateID>();
  cur_queue = 0;
}

void PrefSchemaSearch::initialize() {
  cout << "Conducting pref schema best first search for NGOOSE without"
       << " reopening closed nodes, (real) bound = " << bound << endl;
  const GlobalState &initial_state = g_initial_state();
  statistics.inc_evaluated_states();
  std::vector<ap_float> x = heuristic->compute_features(initial_state);
  best_h = heuristic->compute_heuristic(x);

  open_lists[0]->insert(best_h, initial_state.get_id());

  n_schemata_ = heuristic->get_schema_to_index().size();

  std::cout << "Initial h=" << best_h << std::endl;

  for (const GlobalOperator &op : g_operators) {
    std::istringstream iss(op.get_name());
    std::string schema;
    std::getline(iss, schema, ' ');
    name_to_schema_index[op.get_name()] = heuristic->get_schema_to_index()[schema];
  }

  if (check_goal_and_set_plan(initial_state)) {
    std::cout << "Initial state is a goal state. Terminating..." << std::endl;
    exit(-1);
  }
}

void PrefSchemaSearch::print_checkpoint_line(int g) const {
  cout << "[g=" << g << ", ";
  statistics.print_basic_statistics();
  cout << "]" << endl;
}

void PrefSchemaSearch::print_statistics() const {
  statistics.print_detailed_statistics();
  search_space.print_statistics();
}

SearchStatus PrefSchemaSearch::step() {
  pair<SearchNode, bool> n = fetch_next_node();
  if (!n.second) {
    return FAILED;
  }
  SearchNode node = n.first;

  GlobalState s = node.get_state();

  vector<const GlobalOperator *> applicable_ops;

  g_successor_generator->generate_applicable_ops(s, applicable_ops);

  std::vector<ap_float> schemata_to_best_h(n_schemata_, MAX_VALUE);
  std::vector<std::vector<StateID>> schemata_to_best_h_succs(n_schemata_,
                                                             std::vector<StateID>());

  const std::vector<ap_float> x = heuristic->compute_features(s);
  const std::vector<bool> pref_schema = heuristic->compute_pref_schemata(x);

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

      // evaluate all heuristics
      statistics.inc_evaluated_states();
      const std::vector<ap_float> x_succ = heuristic->compute_features(succ_state);
      const ap_float h = heuristic->compute_heuristic(x_succ);

      // check deadend
      bool is_dead_end = false;
      if (h == DEAD_END) {
        is_dead_end = true;
        break;
      }
      if (is_dead_end) {
        succ_node.mark_as_dead_end();
        statistics.inc_dead_ends();
        continue;
      }

      // add to queues TODO
      open_lists[0]->insert(h, succ_state.get_id());

      int schema_index = name_to_schema_index[op->get_name()];
      if (pref_schema[schema_index] && h < schemata_to_best_h[schema_index]) {
        schemata_to_best_h[schema_index] = h;
        schemata_to_best_h_succs[schema_index].clear();
        schemata_to_best_h_succs[schema_index].push_back(succ_state.get_id());
      } else if (pref_schema[schema_index] && h == schemata_to_best_h[schema_index]) {
        schemata_to_best_h_succs[schema_index].push_back(succ_state.get_id());
      }

      // update best h values
      if (h < best_h) {
        best_h = h;
        cout << "New best h=" << h << " ";
        print_checkpoint_line(node.get_g() + 1);
      }
    }
  }

  for (int i = 0; i < n_schemata_; i++) {
    for (const StateID &succ_id : schemata_to_best_h_succs[i]) {
      open_lists[i + 1]->insert(schemata_to_best_h[i], succ_id);
    }
  }

  return IN_PROGRESS;
}

pair<SearchNode, bool> PrefSchemaSearch::fetch_next_node() {
  int incremented = 0;
  int n = open_lists.size();
  while (true) {
    if (incremented == n) {
      cout << "Completely explored state space -- no solution!" << endl;
      SearchNode dummy_node = search_space.get_node(g_initial_state());
      return make_pair(dummy_node, false);
    }

    if (open_lists[cur_queue]->empty()) {
      cur_queue = (cur_queue + 1) % n;
      incremented++;
      continue;
    }

    StateID id = open_lists[cur_queue]->remove_min();

    if (popped.count(id) > 0) {
      continue;
    }

    cur_queue = (cur_queue + 1) % n;
    popped.insert(id);
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

void PrefSchemaSearch::save_plan_if_necessary() const {
  if (found_solution()) {
    save_plan(get_plan());
    // heuristic->print_statistics();
  }
}

void PrefSchemaSearch::dump_search_space() const { search_space.dump(); }

static SearchEngine *_parse_greedy(OptionParser &parser) {
  parser.document_synopsis("Multi-queue greedy search (eager) for NGOOSE", "");
  parser.document_note("Closed nodes", "Closed node are not re-opened");

  parser.add_list_option<ScalarEvaluator *>("evals", "scalar evaluators");
  parser.add_list_option<Heuristic *>(
      "preferred", "not used, but required to implement this option to run", "[]");
  parser.add_option<ap_float>(
      "boost", "not used, but required to implement this option to run", "0");

  SearchEngine::add_options_to_parser(parser);

  Options opts = parser.parse();
  opts.verify_list_non_empty<ScalarEvaluator *>("evals");

  PrefSchemaSearch *engine = nullptr;
  if (!parser.dry_run()) {
    opts.set("open", search_common::create_greedy_open_list_factory(opts));
    engine = new PrefSchemaSearch(opts);
  }
  return engine;
}

static Plugin<SearchEngine> _plugin_greedy("pref_schema_eager_greedy", _parse_greedy);
}  // namespace pref_schema_search
