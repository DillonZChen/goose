#include "wlf_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include <chrono>
#include <utility>

using namespace std;

typedef std::vector<int> CatFeature;
typedef std::vector<ap_float> ConFeature;

namespace wlf_heuristic {
WlfHeuristic::WlfHeuristic(const options::Options &opts)
    : PybindHeuristic(opts, "FeatureGenerationModel") {
  cat_iterations_ = model.attr("get_cat_iterations")().cast<int>();
  con_iterations_ = model.attr("get_con_iterations")().cast<int>();
  round_ = model.attr("get_round_opts_list")().cast<std::vector<bool>>();
  weights_list_ =
      model.attr("get_weights_list")().cast<std::vector<std::vector<ap_float>>>();
  n_models_ = weights_list_.size();
  hash_ = model.attr("get_hash")().cast<std::unordered_map<std::string, int>>();

  n_init_features_ = model.attr("get_n_init_features")().cast<int>();
  n_con_features_ = model.attr("get_n_con_features")().cast<int>();
  n_cat_features_ = model.attr("get_n_cat_features")().cast<int>();
  n_features_ = n_con_features_ + n_cat_features_;
  numeric = n_con_features_ > 0;

  std::vector<std::string> targets_list =
      model.attr("get_targets_list")().cast<std::vector<std::string>>();
  heuristic_models_ = std::vector<int>();
  deadend_models_ = std::vector<int>();
  policy_models_ = std::vector<int>();
  for (int i = 0; i < n_models_; i++) {
    std::cout << "model_" << i << " predicts " << targets_list[i] << std::endl;
    if (i == 0 && !(targets_list[i] == "h" || targets_list[i] == "r")) {
      std::cerr << "First model must predict heuristic" << std::endl;
      exit(-1);
    }
    if (targets_list[i] == "h" || targets_list[i] == "r") {
      heuristic_models_.push_back(i);
    } else if (targets_list[i] == "d") {
      deadend_models_.push_back(i);
    } else if (targets_list[i] == "p") {
      policy_models_.push_back(i);
    }
  }

  schema_to_index_ = model.attr("get_schema_to_index")()
                         .cast<std::unordered_map<std::string, int>>();

  std::cout << "cat_iterations=" << cat_iterations_ << std::endl;
  std::cout << "con_iterations=" << con_iterations_ << std::endl;
  std::cout << "round.size=" << round_.size() << std::endl;
  std::cout << "weights_list.size=" << weights_list_.size() << std::endl;
  for (int i = 0; i < n_models_; i++) {
    std::cout << "weights_" << i << ".size=" << weights_list_[i].size() << std::endl;
  }
  std::cout << "n_models=" << n_models_ << std::endl;
  std::cout << "n_init_features=" << n_init_features_ << std::endl;
  std::cout << "n_con_features=" << n_con_features_ << std::endl;
  std::cout << "n_cat_features=" << n_cat_features_ << std::endl;
  std::cout << "heuristic_models.size=" << heuristic_models_.size() << std::endl;
  std::cout << "deadend_models.size=" << deadend_models_.size() << std::endl;
  std::cout << "policy_models.size=" << policy_models_.size() << std::endl;

  graph_time = 0;
  wl_time = 0;
  linear_time = 0;

  // initialise graph object
  std::cout << "Initialising graph object from NFD..." << std::endl;

  graph = std::make_shared<ngoose_wlf_graph::WlfGraph>(ngoose_wlf_graph::WlfGraph(
      std::make_shared<pybind11::object>(model), fact_to_pred_objects, fluent_names));

  std::cout << "WlfGraph object initialised." << std::endl;
}

WlfHeuristic::~WlfHeuristic() {}

void WlfHeuristic::print_statistics() {
  std::cout << "WlfHeuristic graph time: " << graph_time << "s\n";
  std::cout << "WlfHeuristic feature time: " << wl_time << "s\n";
  std::cout << "WlfHeuristic linear time: " << linear_time << "s\n";
}

const ngoose_wlf_graph::WlfGraph
WlfHeuristic::state_to_graph(const GlobalState &global_state) {
  std::vector<std::string> bool_vals =
      global_state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true);
  std::vector<ap_float> num_vals = global_state.get_num_values(fluent_indices);
  // global_state.dump_readable();

  start_time = get_time();
  const ngoose_wlf_graph::WlfGraph state_graph = graph->state_to_graph(bool_vals, num_vals);

  // state_graph.dump();
  // exit(-1);

  end_time = get_time();
  graph_time += end_time.count() - start_time.count();

  return state_graph;
}

std::vector<ap_float>
WlfHeuristic::compute_features(const ngoose_wlf_graph::WlfGraph &state_graph) {
  const std::vector<int> x_cat = state_graph.get_x_cat();
  const std::vector<ap_float> x_con = state_graph.get_x_con();
  const std::vector<std::vector<std::pair<int, int>>> neighbours =
      state_graph.get_neighbours();
  const size_t n_nodes = x_cat.size();

  start_time = get_time();
  CatFeature prv_x(n_nodes, 0);
  CatFeature cur_x(n_nodes, 0);
  std::vector<ap_float> x(n_features_, 0);

  int cat;
  std::string new_colour;
  for (size_t u = 0; u < n_nodes; u++) {
    new_colour = std::to_string(x_cat[u]);
    if (hash_.count(new_colour)) {
      cat = hash_.at(new_colour);
      x[cat]++;
      if (numeric) {
        x[cat + n_cat_features_] += x_con[u];
      }
    } else {
      cat = -1;
    }
    prv_x[u] = cat;
  }

  for (int itr = 0; itr < cat_iterations_; itr++) {
    for (size_t u = 0; u < n_nodes; u++) {
      const std::vector<std::pair<int, int>> &neigh = neighbours[u];
      std::set<std::pair<int, int>> neighbour_cats;  // sorted set

      if (prv_x[u] == -1) {
        cat = -1;
        goto end_of_loop;
      }

      // collect colours from neighbours
      for (size_t i = 0; i < neigh.size(); i++) {
        int v = neigh[i].first;
        cat = prv_x[v];
        if (cat == -1) {
          goto end_of_loop;
        }
        neighbour_cats.insert(std::make_pair(cat, neigh[i].second));
      }

      // add current colour and sorted neighbours into sorted colour key
      new_colour = std::to_string(prv_x[u]);
      for (const auto &ne_pair : neighbour_cats) {
        new_colour += "," + std::to_string(ne_pair.first) + "," +
                      std::to_string(ne_pair.second);
      }

      // hash seen colours
      if (hash_.count(new_colour)) {
        cat = hash_.at(new_colour);
        x[cat]++;
        if (numeric) {
          x[cat + n_cat_features_] += x_con[u];
        }
      } else {
        cat = -1;
      }

    end_of_loop:
      cur_x[u] = cat;
    }
    cur_x.swap(prv_x);
  }

  end_time = get_time();
  wl_time += end_time.count() - start_time.count();
  return x;
}

ap_float WlfHeuristic::estimate(const std::vector<ap_float> x, int model_index) {
  ap_float h = 0;
  start_time = get_time();
  for (int j = 0; j < n_features_; j++) {
    h += x[j] * weights_list_[model_index][j];
  }
  end_time = get_time();
  linear_time += end_time.count() - start_time.count();
  return h;
}

bool WlfHeuristic::predict_binary(const std::vector<ap_float> x, int model_index) {
  ap_float y = estimate(x, model_index);
  bool binary = y > 0;
  return binary;
}

ap_float WlfHeuristic::predict_heuristic(const std::vector<ap_float> x,
                                         int model_index) {
  ap_float h = estimate(x, model_index);
  if (round_[model_index]) {
    h = round(h);
  }
  return h;
}

std::vector<ap_float> WlfHeuristic::compute_features(const GlobalState &global_state) {
  return compute_features(state_to_graph(global_state));
}

ap_float WlfHeuristic::compute_heuristic(const GlobalState &global_state) {
  // Only single heuristic models should call this, assuming model_0 predicts heuristic
  const std::vector<ap_float> x = compute_features(global_state);
  for (const int model_index : deadend_models_) {
    bool is_deadend = predict_binary(x, model_index);
    if (is_deadend) {
      return DEAD_END;
    }
  }
  ap_float h = predict_heuristic(x, 0);
  return h;
}

std::vector<ap_float>
WlfHeuristic::compute_heuristic_batch(const std::vector<GlobalState> &states) {
  std::vector<ap_float> hs;
  for (const GlobalState &state : states) {
    hs.push_back(compute_heuristic(state));
  }
  return hs;
}

std::vector<ap_float>
WlfHeuristic::compute_multi_heuristics(const GlobalState &global_state) {
  const std::vector<ap_float> x = compute_features(global_state);
  std::vector<ap_float> hs;
  for (const int model_index : heuristic_models_) {
    hs.push_back(predict_heuristic(x, model_index));
  }
  return hs;
}

ap_float WlfHeuristic::compute_heuristic(const std::vector<ap_float> x) {
  return predict_heuristic(x, 0);
}

std::vector<bool> WlfHeuristic::compute_pref_schemata(const std::vector<ap_float> x) {
  std::vector<bool> pref_schemata;
  for (const int model_index : policy_models_) {
    pref_schemata.push_back(predict_binary(x, model_index));
  }
  return pref_schemata;
}

static Heuristic *_parse(OptionParser &parser) {
  parser.document_synopsis("WLF heuristic optimised", "");
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
    return new WlfHeuristic(opts);
}

static Plugin<Heuristic> _plugin("wlf", _parse);
}  // namespace wlf_heuristic
