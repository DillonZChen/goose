#include "gnn_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include <utility>

using namespace std;

namespace gnn_heuristic {
GnnHeuristic::GnnHeuristic(const options::Options &opts)
    : PybindHeuristic(opts, "DeepLearningModel") {

  std::cout << "Getting x..." << std::endl;
  std::vector<std::vector<double>> x =
      model.attr("get_graph_x")().cast<std::vector<std::vector<double>>>();

  d_ = x[0].size();
  std::cout << "X.shape: " << x.size() << ", " << d_ << std::endl;

  std::cout << "Getting edge_indices..." << std::endl;
  int n_edge_labels = model.attr("get_n_edge_labels")().cast<int>();
  std::vector<std::vector<std::vector<int>>> edge_indices;
  for (int i = 0; i < n_edge_labels; i++) {
    std::vector<std::vector<int>> edge_index;
    edge_index.push_back(
        model.attr("get_graph_edge_indices_0")(i).cast<std::vector<int>>());
    edge_index.push_back(
        model.attr("get_graph_edge_indices_1")(i).cast<std::vector<int>>());
    edge_indices.push_back(edge_index);
  }

  graph = {x, edge_indices};

  if (x.size() == 0) {
    std::cerr << "Graph X is empty" << std::endl;
    exit(-1);
  }

  bool_label_offset_ = model.attr("get_bool_label_offset")().cast<int>();
  std::cout << "Bool label offset: " << bool_label_offset_ << std::endl;

  num_goal_offset_ = model.attr("get_num_goal_offset")().cast<int>();
  std::cout << "Numerical goal offset: " << num_goal_offset_ << std::endl;

  /// copied from ngoose/graph.cc but note that DeepLearningModel has different
  /// implementations of the functions that are called here
  std::cout << "Getting name to index..." << std::endl;
  name_to_idx =
      model.attr("get_name_to_idx")().cast<std::unordered_map<std::string, int>>();

  std::cout << "Getting goal facts..." << std::endl;
  bool_goals = model.attr("get_bool_goals")().cast<std::unordered_set<std::string>>();

  std::cout << "Getting fact predicate to index dict..." << std::endl;
  std::unordered_map<std::string, int> fact_pred_to_idx =
      model.attr("get_fact_pred_to_idx")().cast<std::unordered_map<std::string, int>>();

  std::cout << "Preprocessing fluent information..." << std::endl;
  fluent_node_indices = std::vector<int>();
  fluent_node_columns = std::vector<int>();
  for (const std::string &name : fluent_names) {
    if (name_to_idx.count(name) == 0) {
      std::cerr << "Fluent name not found in name_to_idx: " << name << std::endl;
      exit(-1);
    }
    fluent_node_indices.push_back(name_to_idx.at(name));
    fluent_node_columns.push_back(model.attr("get_fluent_feat_idx")(name).cast<int>());
  }

  // preprocessing to access indices for fact nodes faster
  std::cout << "Preprocessing fact information..." << std::endl;
  for (auto it = fact_to_pred_objects.begin(); it != fact_to_pred_objects.end(); it++) {
    std::string fact = it->first;
    std::string pred = it->second.first;
    std::vector<std::string> objs = it->second.second;

    // see representation/feature_generator.py fact_node_feat_idx
    // offset = 1
    int cat_offset = 1 + 3 * fact_pred_to_idx.at(pred);

    std::vector<int> obj_idxs = std::vector<int>();
    for (const std::string &obj : objs) {
      obj_idxs.push_back(name_to_idx.at(obj));
    }
    fact_to_cat_offset_and_obj_indices[fact] = std::make_pair(cat_offset, obj_idxs);
  }

  std::cout << "GnnHeuristic initialised succesfully!" << std::endl;
}

GnnHeuristic::~GnnHeuristic() {}

GnnGraph GnnHeuristic::state_to_graph(const std::vector<std::string> &bool_vals,
                                      const std::vector<ap_float> &num_vals) const {
  /// mainly copied from ngoose/graph.cc but for gnn
  FeatureMatrix x = graph.x;
  EdgeIndices edge_indices = graph.edge_indices;

  /// facts
  // T_GOAL = 0
  // F_GOAL = 1
  // T_FACT = 2
  // std::cout << "Setting facts..." << std::endl;
  for (const std::string &var : bool_vals) {
    const std::pair<int, std::vector<int>> &cat_offset_and_obj_indices =
        fact_to_cat_offset_and_obj_indices.at(var);
    const int cat_offset = cat_offset_and_obj_indices.first;
    if (bool_goals.count(var)) {
      int idx = name_to_idx.at(var);
      x[idx][cat_offset + 0] = 1.0;
      x[idx][cat_offset + 1] = 0.0;
    } else {
      // nodes and edges are only added here
      int idx = x.size();  // this must be before the push_back later
      x.push_back(std::vector<double>(d_, 0.0));
      x[idx][cat_offset + 2] = 1.0;
      int n_objs = cat_offset_and_obj_indices.second.size();
      for (int i = 0; i < n_objs; i++) {
        int obj_idx = cat_offset_and_obj_indices.second[i];
        int edge_label = bool_label_offset_ + i;
        edge_indices[edge_label][0].push_back(obj_idx);
        edge_indices[edge_label][0].push_back(idx);
        edge_indices[edge_label][1].push_back(idx);
        edge_indices[edge_label][1].push_back(obj_idx);
      }
    }
  }

  /// fluents
  // std::cout << "Setting fluents..." << std::endl;
  for (size_t i = 0; i < num_vals.size(); i++) {
    x[fluent_node_indices[i]][fluent_node_columns[i]] = num_vals[i];
  }

  /// numeric goals
  // <int, int, ap_float> ---> <index, achieved, error>
  // NUMERICAL_GOAL_ACHIEVED = 2
  // NUMERICAL_GOAL_ERROR = 1
  // std::cout << "Setting numeric goals..." << std::endl;
  std::vector<std::tuple<int, int, ap_float>> numeric_goals =
      model.attr("get_num_goal_updates")(bool_vals, num_vals)
          .cast<std::vector<std::tuple<int, int, ap_float>>>();
  for (const std::tuple<int, int, ap_float> &goal : numeric_goals) {
    int idx = std::get<0>(goal);
    x[idx][num_goal_offset_ + 2] = std::get<1>(goal);
    x[idx][num_goal_offset_ + 1] = std::get<2>(goal);
  }

  // std::cout << "done!" << std::endl;

  GnnGraph ret = {x, edge_indices};
  
  // ret.dump();
  // exit(-1);

  return ret;
}

ap_float GnnHeuristic::compute_heuristic(const GlobalState &global_state) {
  std::vector<std::string> bool_vals =
      global_state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true);
  std::vector<ap_float> num_vals = global_state.get_num_values(fluent_indices);

  // global_state.dump_readable();

  // this is only called once from batch_eager_search so no need to optimise
  ap_float h = model.attr("evaluate")(bool_vals, num_vals).cast<ap_float>();

  return h;
}

std::vector<ap_float>
GnnHeuristic::compute_heuristic_batch(const std::vector<GlobalState> &states) {
  start_time = get_time();

  std::vector<FeatureMatrix> x_list;
  std::vector<EdgeIndices> edge_indices_list;
  for (const GlobalState &state : states) {
    std::vector<std::string> bool_vals =
        state.get_facts(fdr_pair_to_name, fdr_pair_to_is_true);
    std::vector<ap_float> num_vals = state.get_num_values(fluent_indices);
    GnnGraph state_graph = state_to_graph(bool_vals, num_vals);
    x_list.push_back(state_graph.x);
    edge_indices_list.push_back(state_graph.edge_indices);
  }

  end_time = get_time();
  graph_time += end_time.count() - start_time.count();

  std::vector<ap_float> h = model.attr("evaluate_batch")(x_list, edge_indices_list)
                                .cast<std::vector<ap_float>>();

  return h;
}

void GnnHeuristic::print_statistics() {
  std::cout << "GnnHeuristic graph time: " << graph_time << "s\n";
  std::cout << "GnnHeuristic data_loader time: "
            << model.attr("get_dataloader_time")().cast<double>() << "s\n";
  std::cout << "GnnHeuristic gnn time: " << model.attr("get_gnn_time")().cast<double>()
            << "s\n";
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
    return new GnnHeuristic(opts);
}

static Plugin<Heuristic> _plugin("gnn", _parse);
}  // namespace gnn_heuristic
