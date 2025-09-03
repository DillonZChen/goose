#include "wlf_mq_eager_search.h"

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

namespace wlf_mq_eager_search {
WlfMqEagerSearch::WlfMqEagerSearch(const Options &opts) : SearchEngine(opts) {
  heuristic = dynamic_cast<wlf_heuristic::WlfHeuristic *>(
      opts.get_list<ScalarEvaluator *>("evals")[0]);
  n_heuristics_ = heuristic->n_heuristics();
  open_lists = std::vector<GooseOpenList<StateID> *>();
  for (int i = 0; i < n_heuristics_; i++) {
    GooseOpenList<StateID> *open_list = new GooseOpenList<StateID>();
    open_lists.push_back(open_list);
  }
  popped = std::unordered_set<StateID>();
  cur_queue = 0;
}

void WlfMqEagerSearch::initialize() {
  cout << "Conducting multi-queue best first search for NGOOSE without"
       << " reopening closed nodes, (real) bound = " << bound << endl;
  const GlobalState &initial_state = g_initial_state();
  statistics.inc_evaluated_states();
  best_hs = heuristic->compute_multi_heuristics(initial_state);

  open_lists[0]->insert(best_hs[0], initial_state.get_id());

  for (int i = 0; i < n_heuristics_; i++) {
    std::cout << "Initial h_" << i << "=" << best_hs[i] << std::endl;
  }

  if (check_goal_and_set_plan(initial_state)) {
    std::cout << "Initial state is a goal state. Terminating..." << std::endl;
    exit(-1);
  }
}

void WlfMqEagerSearch::print_checkpoint_line(int g) const {
  cout << "[g=" << g << ", ";
  statistics.print_basic_statistics();
  cout << "]" << endl;
}

void WlfMqEagerSearch::print_statistics() const {
  statistics.print_detailed_statistics();
  search_space.print_statistics();
}

SearchStatus WlfMqEagerSearch::step() {
  pair<SearchNode, bool> n = fetch_next_node();
  if (!n.second) {
    return FAILED;
  }
  SearchNode node = n.first;

  GlobalState s = node.get_state();

  vector<const GlobalOperator *> applicable_ops;

  g_successor_generator->generate_applicable_ops(s, applicable_ops);

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
      const std::vector<ap_float> hs = heuristic->compute_multi_heuristics(succ_state);

      // check deadend
      bool is_dead_end = false;
      for (int i = 0; i < n_heuristics_; i++) {
        if (hs[i] == DEAD_END) {
          is_dead_end = true;
          break;
        }
      }
      if (is_dead_end) {
        succ_node.mark_as_dead_end();
        statistics.inc_dead_ends();
        continue;
      }

      // add to queues
      for (int i = 0; i < n_heuristics_; i++) {
        open_lists[i]->insert(hs[i], succ_state.get_id());
      }

      // update best h values
      for (int i = 0; i < n_heuristics_; i++) {
        if (hs[i] < best_hs[i]) {
          best_hs[i] = hs[i];
          cout << "New best h_" << i << "=" << hs[i] << " ";
          print_checkpoint_line(node.get_g() + 1);
        }
      }
    }
  }

  return IN_PROGRESS;
}

pair<SearchNode, bool> WlfMqEagerSearch::fetch_next_node() {
  int incremented = 0;
  while (true) {
    if (incremented == n_heuristics_) {
      cout << "Completely explored state space -- no solution!" << endl;
      SearchNode dummy_node = search_space.get_node(g_initial_state());
      return make_pair(dummy_node, false);
    }

    if (open_lists[cur_queue]->empty()) {
      cur_queue = (cur_queue + 1) % n_heuristics_;
      incremented++;
      continue;
    }

    StateID id = open_lists[cur_queue]->remove_min();

    if (popped.count(id) > 0) {
      continue;
    }

    cur_queue = (cur_queue + 1) % n_heuristics_;
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

void WlfMqEagerSearch::save_plan_if_necessary() const {
  if (found_solution()) {
    save_plan(get_plan());
    // heuristic->print_statistics();
  }
}

void WlfMqEagerSearch::dump_search_space() const { search_space.dump(); }

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

  WlfMqEagerSearch *engine = nullptr;
  if (!parser.dry_run()) {
    opts.set("open", search_common::create_greedy_open_list_factory(opts));
    engine = new WlfMqEagerSearch(opts);
  }
  return engine;
}

static Plugin<SearchEngine> _plugin_greedy("wlf_mq_eager_greedy", _parse_greedy);
}  // namespace wlf_mq_eager_search
