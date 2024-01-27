#include "goose_linear_online.h"

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

namespace goose_linear_online {

GooseLinearOnline::GooseLinearOnline(const plugins::Options &opts)
    : goose_linear::GooseLinear(opts) {
  std::seed_seq seed{0};
  rng = std::mt19937(seed);

  // suboptimal (load a model again after it was deleted in GooseLinear)
  pybind11::module util_module = pybind11::module::import("models.save_load");
  model = util_module.attr("load_kernel_model_and_setup")(model_path, domain_file, instance_file);

  train();
}

FullState GooseLinearOnline::assign_random_state(const PartialState &state) {
  FullState ret;

  for (int var = 0; var < n_variables; var++) {
    int val = state[var];
    if (val == -1) {
      std::uniform_int_distribution<std::mt19937::result_type> dist(0, vars[var].get_domain_size() -
                                                                           1);
      val = dist(rng);
    }
    ret.push_back(FactPair(var = var, val = val));
  }

  return ret;
}

FullState GooseLinearOnline::partial_state_to_fullstate_type(const PartialState &state) {
  FullState ret;

  for (int var = 0; var < n_variables; var++) {  // TODO(DZC) can optimise if -1 vars are static?
    int val = state[var];
    ret.push_back(FactPair(var = var, val = val));
  }

  return ret;
}

inline bool regressable(const PartialState &state, const OperatorProxy &op) {
  // https://fai.cs.uni-saarland.de/teaching/winter18-19/planning-material/planning06-progression-and-regression-post-handout.pdf
  // slide 16/32
  bool non_empty = false;

  std::unordered_set<int> effect_vars;

  FactPair fact_pair;
  for (const EffectProxy &eff : op.get_effects()) {
    fact_pair = eff.get_fact().get_pair();  // assume no conditional effects
    int var = fact_pair.var;
    int g_v = state[var];
    int eff_v = fact_pair.value;
    effect_vars.insert(var);

    // (i) effect and partial state non empty
    if (g_v == eff_v) {
      non_empty = true;
    }

    // (ii) effect leads into the partial state
    if (g_v != -1 && eff_v != g_v) {
      return false;
    }
  }

  if (!non_empty) {
    return false;
  }

  for (const FactProxy &fact : op.get_preconditions()) {
    fact_pair = fact.get_pair();  // assume no conditional effects
    int var = fact_pair.var;
    int g_v = state[var];
    int pre_v = fact_pair.value;

    // (iii) unchanged precondition still the same in the partial state
    if (!effect_vars.count(var) && g_v != -1 && pre_v != g_v) {
      return false;
    }
  }

  return true;
}

inline PartialState regress(const PartialState &state, const OperatorProxy &op) {
  // (g \ eff_a) \cup pre_a
  PartialState ret = state;

  FactPair fact_pair;
  int var, val;
  for (const EffectProxy &eff : op.get_effects()) {
    fact_pair = eff.get_fact().get_pair();  // assume no conditional effects
    var = fact_pair.var;
    val = fact_pair.value;

    // delete the effect
    if (state[var] == val) {  // equivalently state[var] != -1 because of regressable
      ret[var] = -1;
    }
  }

  for (const FactProxy &fact : op.get_preconditions()) {
    fact_pair = fact.get_pair();  // assume no conditional effects
    var = fact_pair.var;
    val = fact_pair.value;

    // add the precondition
    ret[var] = val;
  }

  return ret;
}

template <typename T>
std::vector<T> GooseLinearOnline::get_random_elements(const std::vector<T> &original_vector,
                                                      std::size_t n) {
  std::vector<T> shuffled_vector = original_vector;
  std::shuffle(shuffled_vector.begin(), shuffled_vector.end(), rng);
  shuffled_vector.resize(n);
  return shuffled_vector;
}

void GooseLinearOnline::train() {
  n_variables = task->get_num_variables();
  vars = task_proxy.get_variables();

  // initial state in backwards search is the goal condition
  std::map<VariableProxy, int> var_to_ind;
  for (int i = 0; i < n_variables; i++) {
    var_to_ind[vars[i]] = i;
  }

  PartialState goal_condition(n_variables, -1);
  for (FactProxy goal : task_proxy.get_goals()) {
    goal_condition[var_to_ind[goal.get_variable()]] = goal.get_value();
  }

  FullState full_state = assign_random_state(goal_condition);
  int y = 0;
  // SearchNodeStats stats = compute_heuristic_vector_state(full_state);

  BackwardsSearchNode node(goal_condition, y);

  std::set<PartialState> seen;
  std::set<std::vector<int>> seen_features;  // partial pruning
  std::queue<BackwardsSearchNode> q;
  std::unordered_map<int, std::vector<PartialState>> y_to_states;
  int max_y = 0;

  /* main BFS loop for performing regression */
  double start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  int previous_state_cnt = 0;
  std::cout << "performing BFS regression up to " << MAX_REGRESSION_STATES_ << " states"
            << std::endl;
  seen.insert(goal_condition);
  q.push(node);
  while (!q.empty() && seen.size() < MAX_REGRESSION_STATES_) {
    BackwardsSearchNode node = q.front();
    q.pop();
    PartialState partial_state = node.state;
    int y = node.y;

    // check regressable operators; could probably optimise with a lot of effort like done in FD
    for (const auto &op : task_proxy.get_operators()) {
      if (regressable(partial_state, op)) {
        PartialState succ_state = regress(partial_state, op);
        if (seen.count(succ_state)) {
          continue;
        }
        seen.insert(succ_state);

        std::shared_ptr<CGraph> graph = fact_pairs_to_graph(partial_state_to_fullstate_type(succ_state));
        // std::vector<int> feature = wl1_feature(graph);
        // if (seen_features.count(feature)) {
        //   continue;
        // }
        // seen_features.insert(feature);
        // int h = predict(feature);
        int succ_y = y + 1;
        q.push(BackwardsSearchNode(succ_state, y = succ_y));

        // store non goal regression training states
        if (!y_to_states.count(succ_y)) {
          y_to_states[succ_y] = std::vector<PartialState>();
        }
        y_to_states[succ_y].push_back(succ_state);
        max_y = std::max(max_y, succ_y);
      }
    }

    if (seen.size() - previous_state_cnt >= 10000) {
      previous_state_cnt = seen.size();
      double elapsed_time =
          std::chrono::high_resolution_clock::now().time_since_epoch().count() - start_time;
      elapsed_time /= 1000000000;
      double nodes_per_second = static_cast<double>(previous_state_cnt) / elapsed_time;
      std::cout << "regressed " << seen.size() << " partial states, "
                << "max_y: " << max_y << ", t: " << elapsed_time << "s"
                << " (" << nodes_per_second << " nodes/s)" << std::endl;
    }

    if (q.empty()) {
      std::cout << "BFS terminated because regression space completely explored" << std::endl;
    }
  }

  // std::cout << "Logging y to number of seen partial states with 1wl pruning:" << std::endl;
  // for (int y = max_y; y > 0; y--) {
  //   std::cout << y << " " << y_to_states[y].size() << std::endl;
  // }

  pybind11::list goose_states;  // pybind11::list of GooseState
  pybind11::list ys;            // pybind11::list of int
  // int to_keep = max_y;
  // int to_keep = static_cast<int>(floor(log2(max_y)));
  // int to_keep = 1;
  for (int y = max_y; y > 0; y--) {
    int to_keep = static_cast<int>(floor(log2(y))) + 1;
    // random here can be improved
    std::vector<PartialState> states = get_random_elements(
        y_to_states[y], std::min(static_cast<int>(y_to_states[y].size()), to_keep));

    for (const PartialState &partial_state : states) {
      // random here can be improved
      FullState full_state = assign_random_state(partial_state);

      GooseState goose_state = fact_pairs_list_to_goose_state(full_state);
      goose_states.append(goose_state);
      ys.append(pybind11::int_(y));
    }
  }

  std::string model_data_path =
      model.attr("online_training")(goose_states, ys, domain_file, instance_file)
          .cast<std::string>();
  pybind11::print();

  update_model_from_data_path(model_data_path);
}

SearchNodeStats GooseLinearOnline::compute_heuristic_vector_state(const FullState &state) {
  std::vector<long> cur_seen_colours = cnt_seen_colours;
  std::vector<long> cur_unseen_colours = cnt_unseen_colours;
  std::vector<double> ratio(iterations_);
  std::shared_ptr<CGraph> graph = fact_pairs_to_graph(state);
  std::vector<int> feature = wl_feature(graph);
  int h = predict(feature);
  for (size_t i = 0; i < iterations_; i++) {
    cur_seen_colours[i] -= cnt_seen_colours[i];
    cur_unseen_colours[i] -= cnt_unseen_colours[i];
    ratio[i] = cur_seen_colours[i] / (cur_seen_colours[i] + cur_unseen_colours[i]);
  }
  return SearchNodeStats(h = h, ratio = ratio);
}

class GooseLinearOnlineFeature : public plugins::TypedFeature<Evaluator, GooseLinearOnline> {
 public:
  GooseLinearOnlineFeature() : TypedFeature("linear_model_online") {
    document_title("GOOSE optimised WL feature linear model heuristic with online training");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<std::string>("model_file", "path to trained python model", "default_value");
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

static plugins::FeaturePlugin<GooseLinearOnlineFeature> _plugin;

}  // namespace goose_linear_online
