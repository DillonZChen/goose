#include "goose_kernel.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <cstdio>
#include <algorithm>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

namespace goose_kernel {

GooseKernel::GooseKernel(const plugins::Options &opts) : Heuristic(opts)
{
  initialise_model(opts);
  initialise_facts();
}

void GooseKernel::initialise_model(const plugins::Options &opts) {
  // Add GOOSE submodule to the python path
  auto gnn_path = std::getenv("GOOSE");
  if (!gnn_path) {
      std::cout << "GOOSE env variable not found. Aborting." << std::endl;
      exit(-1);
  }
  std::string path(gnn_path);
  std::cout << "GOOSE path is " << path << std::endl;
  if (access(path.c_str(), F_OK) == -1) {
      std::cout << "GOOSE points to non-existent path. Aborting." << std::endl;
      exit(-1);
  }

  // Append python module directory to the path
  pybind11::module sys = pybind11::module::import("sys");
  sys.attr("path").attr("append")(path);

  // Force all output being printed to stdout. Otherwise INFO logging from
  // python will be printed to stderr, even if it is not an error.
  sys.attr("stderr") = sys.attr("stdout");

  std::string model_path = opts.get<std::string>("model_data");
  std::string domain_file = opts.get<std::string>("domain_file");
  std::string instance_file = opts.get<std::string>("instance_file");
  std::cout << "Trying to load model from file " << model_path << " ...\n";
  pybind11::module util_module = pybind11::module::import("util.save_load");
  model = util_module.attr("load_kernel_model_and_setup")(
    model_path, domain_file, instance_file
  );
  std::cout << "Loaded model!" << std::endl;

  // use I/O similar to goose_linear_regression to get graph representation and WL data
  model.attr("write_model_data")(0);
  model.attr("write_representation_to_file")();
  std::string model_data_path = model.attr("get_model_data_path")().cast<std::string>();
  std::string graph_data_path = model.attr("get_graph_file_path")().cast<std::string>();
  graph_ = CGraph(graph_data_path);

  // load WL hash data
  std::string line;
  std::ifstream infile(model_data_path);
  int hash_cnt = 0, hash_size = 0;
  while (std::getline(infile, line)) {
    std::vector<std::string> toks;
    std::istringstream iss(line);
    std::string s;
    while (std::getline(iss, s, ' ')) {
      toks.push_back(s);
    }
    if (line.find("hash size") != std::string::npos) {
      hash_size = stoi(toks[0]);
      hash_cnt = 0;
      feature_size_ = hash_size;
      continue;
    } else if (line.find("iterations") != std::string::npos) {
      iterations_ = stoi(toks[0]);
      continue;
    } 

    if (hash_cnt < hash_size) {
      hash_[toks[0]] = stoi(toks[1]);
      hash_cnt++;
      continue;
    }
  }

  // remove file
  char* char_array = new char[model_data_path.length() + 1];
  strcpy(char_array, model_data_path.c_str());
  remove(char_array);

  std::cout << "Model initialised!" << std::endl;
}

void GooseKernel::initialise_facts() {
  FactsProxy facts(*task);
  for (FactProxy fact : facts) {
    std::string name = fact.get_name();

    // Convert from FDR var-val pairs back to propositions
    if (name == "<none of those>") {
      continue;
    } else {
      if (name.substr(0, 5) == "Atom ") {
        name = name.substr(5);
      } else if (name.substr(0, 12) == "NegatedAtom ") {
        continue;
      } else {
        std::cout << "Substring of downward fact does not start with 'Atom ': "
                  << "or 'NegatedAtom '"
                  << name << std::endl;
        exit(-1);
      }
    }

    // replace all occurrences of '(' and ')' by ' '
    std::replace(name.begin(), name.end(), '(', ' ');
    std::replace(name.begin(), name.end(), ')', ' ');

    // Remove occurrences of ','
    name.erase(std::remove(name.begin(), name.end(), ','), name.end());

    // Trim string
    if (std::isspace(name[0])) {
      name.erase(0, 1);
    }
    if (std::isspace(name.back())) {
      name.erase(name.end() - 1, name.end());
    }

    std::istringstream iss(name);
    std::string s;
    std::string pred = "";
    std::vector<std::string> args;

    while (std::getline(iss, s, ' ')) {
      if (pred == "") {
        pred = s;
      } else {
        args.push_back(s);
      }
    }
    std::pair<std::string, std::vector<std::string>> lifted_fact(pred, args); 

    fact_to_lifted_input.insert({fact.get_pair(), lifted_fact});
  }
}

CGraph GooseKernel::state_to_graph(const State &state) {
  std::vector<std::vector<std::pair<int, int>>> edges = graph_.get_edges();
  std::vector<int> colours = graph_.get_colours();
  int cur_node_fact, cur_node_arg;
  int new_idx = graph_.n_nodes();

  std::pair<std::string, std::vector<std::string>> pred_args;
  std::string pred, node_name;
  std::vector<std::string> args;
  for (const FactProxy &fact : convert_ancestor_state(state)) {
    pred_args = fact_to_lifted_input[fact.get_pair()];
    pred = pred_args.first;
    args = pred_args.second;
    if (pred.size() == 0) {
      continue;
    }

    node_name = pred;
    for (const std::string &arg : args) {
      node_name += ',' + arg;
    }

    if (graph_.is_pos_goal_node(node_name)) {
      colours[graph_.get_node_index(node_name)] = graph_.TRUE_POS_GOAL_;
      continue;
    }
    if (graph_.is_neg_goal_node(node_name)) {
      colours[graph_.get_node_index(node_name)] = graph_.TRUE_NEG_GOAL_;
      continue;
    }

    // add new node
    cur_node_fact = new_idx;
    colours.push_back(1);
    std::vector<std::pair<int, int>> new_edges_fact;
    edges.push_back(new_edges_fact);

    // connect fact to predicate
    int pred_node = graph_.get_node_index(pred);
    edges[cur_node_fact].push_back(std::make_pair(pred_node, graph_.GROUND_EDGE_LABEL_));
    edges[pred_node].push_back(std::make_pair(cur_node_fact, graph_.GROUND_EDGE_LABEL_));

    for (size_t k = 0; k < args.size(); k++) {
      new_idx++;
      cur_node_arg = new_idx;
      colours.push_back(-static_cast<int>(k));

      std::vector<std::pair<int, int>> new_edges_arg;
      edges.push_back(new_edges_arg);

      // connect variable to predicate
      edges[cur_node_fact].push_back(std::make_pair(cur_node_arg, graph_.GROUND_EDGE_LABEL_));
      edges[cur_node_arg].push_back(std::make_pair(cur_node_fact, graph_.GROUND_EDGE_LABEL_));

      // connect variable to object
      int object_node = graph_.get_node_index(args[k]);
      edges[object_node].push_back(std::make_pair(cur_node_arg, graph_.GROUND_EDGE_LABEL_));
      edges[cur_node_arg].push_back(std::make_pair(object_node, graph_.GROUND_EDGE_LABEL_));
    }

    new_idx++;
  }

  return {edges, colours};
}

std::vector<int> GooseKernel::wl_feature(const CGraph &graph) {
  // feature to return is a histogram of colours seen during training
  std::vector<int> feature(feature_size_, 0);

  const size_t n_nodes = graph.n_nodes();

  // role of colours_0 and colours_1 is switched every iteration for storing old and new colours
  std::vector<int> colours_0(n_nodes);
  std::vector<int> colours_1(n_nodes);
  std::vector<std::vector<std::pair<int, int>>> edges = graph.get_edges();

  // determine size of neighbour colours from the start
  std::vector<std::vector<std::pair<int, int>>> neighbours = edges;

  int col = -1;
  std::string new_colour;

  // collect initial colours
  for (size_t u = 0; u < n_nodes; u++) {
    // initial colours always in hash and hash value always within size
    col = hash_[std::to_string(graph.colour(u))];
    feature[col]++;
    colours_0[u] = col;
  }

  // main WL algorithm loop
  for (size_t itr = 0; itr < iterations_; itr++) {
    // instead of assigning colours_0 = colours_1 at the end of every loop
    // we just switch the roles of colours_0 and colours_1 every loop
    if (itr % 2 == 0) {
      for (size_t u = 0; u < n_nodes; u++) {
        // we ignore colours we have not seen during training
        if (colours_0[u] == -1) {
          goto end_of_loop0;
        }

        // collect colours from neighbours and sort
        for (size_t i = 0; i < edges[u].size(); i++) {
          col = colours_0[edges[u][i].first];
          if (col == -1) {
            goto end_of_loop0;
          }
          neighbours[u][i] = std::make_pair(col, edges[u][i].second);
        }
        sort(neighbours[u].begin(), neighbours[u].end());

        // add current colour and sorted neighbours into sorted colour key
        new_colour = std::to_string(colours_0[u]);
        for (const auto &ne_pair : neighbours[u]) {
          new_colour += "," + std::to_string(ne_pair.first) + "," + std::to_string(ne_pair.second);
        }

        // hash seen colours
        if (hash_.count(new_colour)) {
          col = hash_[new_colour];
          feature[col]++;
        } else {
          col = -1;
        }
end_of_loop0:
        colours_1[u] = col;
      }
    } else {
      for (size_t u = 0; u < n_nodes; u++) {
        // we ignore colours we have not seen during training
        if (colours_1[u] == -1) {
          goto end_of_loop1;
        }

        // collect colours from neighbours and sort
        for (size_t i = 0; i < edges[u].size(); i++) {
          col = colours_1[edges[u][i].first];
          if (col == -1) {
            goto end_of_loop1;
          }
          neighbours[u][i] = std::make_pair(col, edges[u][i].second);
        }
        sort(neighbours[u].begin(), neighbours[u].end());

        // add current colour and sorted neighbours into sorted colour key
        new_colour = std::to_string(colours_1[u]);
        for (const auto &ne_pair : neighbours[u]) {
          new_colour += "," + std::to_string(ne_pair.first) + "," + std::to_string(ne_pair.second);
        }

        // hash seen colours
        if (hash_.count(new_colour)) {
          col = hash_[new_colour];
          feature[col]++;
        } else {
          col = -1;
        }
end_of_loop1:
        colours_0[u] = col;
      }
    }
  }

  return feature;
}

int GooseKernel::predict(const std::vector<int> &feature)
{
  // py::list py_feature;
  int h = model.attr("svr_predict")(feature).cast<int>();
  return h;
}

int GooseKernel::compute_heuristic(const State &ancestor_state) {
  // step 1.
  CGraph graph = state_to_graph(ancestor_state);
  // step 2.
  std::vector<int> feature = wl_feature(graph);
  // step 3.
  int h = predict(feature);
  return h;
}

std::vector<int> GooseKernel::compute_heuristic_batch(const std::vector<State> &ancestor_states) {
  std::vector<int> ret;
  for (auto state : ancestor_states) {
    ret.push_back(compute_heuristic(state));
  }
  return ret;
}

class GooseKernelFeature : public plugins::TypedFeature<Evaluator, GooseKernel> {
 public:
  GooseKernelFeature() : TypedFeature("kernel") {
    document_title("GOOSE optimised WL kernel heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<std::string>(
      "model_data",
      "path to trained model data in the form of a .joblib file",
      "default_value");
    add_option<std::string>(
      "domain_file",
      "Path to the domain file.",
      "default_file");
    add_option<std::string>(
      "instance_file",
      "Path to the instance file.",
      "default_file");

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

static plugins::FeaturePlugin<GooseKernelFeature> _plugin;

}  // namespace goose_kernel