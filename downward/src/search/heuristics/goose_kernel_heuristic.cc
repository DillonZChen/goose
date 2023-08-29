#include "goose_kernel_heuristic.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <cstdio>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

namespace py = pybind11;

using std::string;

std::vector<std::string> tokenise(const string str) {
  std::istringstream iss(str);
  std::string s;
  std::vector<std::string> ret;
  while (std::getline(iss, s, ' ')) {
    ret.push_back(s);
  }
  return ret;
}

namespace goose_kernel_heuristic {
CGraph::CGraph() 
{ }

CGraph::CGraph(const std::string &path) {
  std::string line;
  std::vector<std::string> toks;
  std::ifstream infile(path);
  int node = 0;
  int n_nodes = -1;
  int n_pos_goal_nodes = -1;
  int n_neg_goal_nodes = -1;
  int pos_goal_cnt = 0;
  int neg_goal_cnt = 0;

  // collect graph information
  while (std::getline(infile, line)) {
    toks = tokenise(line);

    // collect graph structure and colours
    // line number = node
    // <node_name> <node_colour> [<neighbour_node> <edge_label>]
    if (n_nodes == -1) {
      n_nodes = stoi(toks[0]);
      continue;
    }
    if (node < n_nodes) {
      string node_name = toks[0];
      node_index_[node_name] = node;

      int colour = stoi(toks[1]);
      colour_.push_back(colour);

      std::vector<std::pair<int, int>> neighbours;

      for (size_t i = 2; i < toks.size(); i+=2) {
        int neighbour_node = stoi(toks[i]);
        int edge_label = stoi(toks[i+1]);
        neighbours.push_back({neighbour_node, edge_label});
      }

      edges_.push_back(neighbours);

      node++;
      continue;
    }

    // collect positive goal nodes
    if (n_pos_goal_nodes == -1) {
      n_pos_goal_nodes = stoi(toks[0]);
      continue;
    }
    if (pos_goal_cnt < n_pos_goal_nodes) {
      string node_name = toks[0];
      pos_goal_nodes_.insert(node_name);
      pos_goal_cnt++;
      continue;
    }

    // collect negative goal nodes
    if (n_neg_goal_nodes == -1) {
      n_neg_goal_nodes = stoi(toks[0]);
      continue;
    }
    if (neg_goal_cnt < n_neg_goal_nodes) {
      string node_name = toks[0];
      neg_goal_nodes_.insert(node_name);
      neg_goal_cnt++;
      continue;
    }
  }

  // remove file
  char* char_array = new char[path.length() + 1];
  strcpy(char_array, path.c_str());
  remove(char_array);
}

CGraph::CGraph(const std::vector<std::vector<std::pair<int, int>>> &edges, const std::vector<int> &colour) : edges_(edges), colour_(colour) 
{ }

GooseKernelHeuristic::GooseKernelHeuristic(const plugins::Options &opts)
    : Heuristic(opts) {
  initialise_model(opts);
  initialise_facts();
}

void GooseKernelHeuristic::initialise_model(const plugins::Options &opts) {
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
  py::module sys = py::module::import("sys");
  sys.attr("path").attr("append")(path);

  // Force all output being printed to stdout. Otherwise INFO logging from
  // python will be printed to stderr, even if it is not an error.
  sys.attr("stderr") = sys.attr("stdout");

  // Read paths
  std::string model_path = opts.get<string>("model_path");
  std::string domain_file = opts.get<string>("domain_file");
  std::string instance_file = opts.get<string>("instance_file");

  // Throw everything into Python code
  std::cout << "Trying to load model from file " << model_path << " ...\n";
  py::module util_module = py::module::import("util.save_load");
  pybind11::object model = util_module.attr("load_kernel_model_and_setup")(model_path, domain_file, instance_file);
  
  // only supports LLG WL kernel
  if (!model.attr("lifted_state_input")().cast<bool>()) {
    std::cout << "Grounded optimised kernel not implemented. "
              << "This optimisation is only for LLG." << std::endl;
    exit(-1);
  }

  // collect data from saved python model
  std::string graph_file_path = model.attr("get_graph_file_path")().cast<std::string>();
  graph_ = CGraph(graph_file_path);
  hash_ = model.attr("get_hash")().cast<std::map<std::string, int>>();
  weights_ = model.attr("get_weights")().cast<std::vector<double>>();
  bias_ = model.attr("get_bias")().cast<double>();
  feature_size_ = static_cast<int>(weights_.size());
  iterations_ = model.attr("get_iterations")().cast<size_t>();
}

void GooseKernelHeuristic::initialise_facts() {
  FactsProxy facts(*task);
  for (FactProxy fact : facts) {
    string name = fact.get_name();

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

    #ifndef NDEBUG
      std::cout << name << " ";
    #endif
  }

  #ifndef NDEBUG
    std::cout << std::endl;
  #endif
}

CGraph GooseKernelHeuristic::state_to_graph(const State &state) {
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
      // std::cout<<node_name<<" "<<graph_.get_node_index(node_name)<<std::endl;
      continue;
    }
    if (graph_.is_neg_goal_node(node_name)) {
      colours[graph_.get_node_index(node_name)] = graph_.TRUE_NEG_GOAL_;
      continue;
    }

    // add new node
    cur_node_fact = new_idx;
    colours.push_back(1);
    // colours.push_back(graph_.TRUE_FACT_);  // this causes issues when linking?
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

std::vector<int> GooseKernelHeuristic::wl_feature(CGraph &graph) {
  // feature to return is a histogram of colours seen during training
  std::vector<int> feature(feature_size_, 0);

  size_t n_nodes = graph.n_nodes();
  std::vector<int> cur_colours(n_nodes);
  std::vector<std::vector<std::pair<int, int>>> edges = graph.get_edges();
  int col;  // 0 <= col < feature_size_

  // collect initial colours
  for (size_t u = 0; u < n_nodes; u++) {
    // initial colours always in hash and hash value always within size
    col = hash_[std::to_string(graph.colour(u))];
    feature[col]++;
    cur_colours[u] = col;
  }

  // main WL algorithm loop
  for (size_t itr = 0; itr < iterations_; itr++) {
    std::vector<int> new_colours(n_nodes);
    for (size_t u = 0; u < n_nodes; u++) {
      col = cur_colours[u];

      // we ignore colours we have not seen during training
      if (col==-1) {
        new_colours[u] = -1;
        continue;
      }

      // this will be added to with sorted neighbour colours
      std::string new_colour = std::to_string(col);

      // collect colours from neighbours and sort
      std::vector<std::pair<int, int>> neighbours;
      for (const auto &ne_pair : edges[u]) {
        col = cur_colours[ne_pair.first];
        neighbours.push_back(std::make_pair(col, ne_pair.second));
      }
      sort(neighbours.begin(), neighbours.end());

      // add sorted neighbours into sorted colour key
      for (const auto &ne_pair : neighbours) {
        new_colour += "," + std::to_string(ne_pair.first) + "," + std::to_string(ne_pair.second);
      }

      // hash seen colours
      if (hash_.count(new_colour)) {
        col = hash_[new_colour];
        feature[col]++;
      } else {
        col = -1;
      }
      new_colours[u] = col;
    }

    // can be optimised by not assigning but switching between the two instead...
    cur_colours = new_colours;
  }

  return feature;
}

int GooseKernelHeuristic::predict(const std::vector<int> &feature) {
  double ret = bias_;
  for (int i = 0; i < feature_size_; i++) {
    ret += feature[i] * weights_[i];
  }
  return static_cast<int>(round(ret));
}

int GooseKernelHeuristic::compute_heuristic(const State &ancestor_state) {
  // step 1.
  CGraph graph = state_to_graph(ancestor_state);
  // step 2.
  std::vector<int> feature = wl_feature(graph);
  // step 3.
  double h = predict(feature);
  return h;
}

std::vector<int> GooseKernelHeuristic::compute_heuristic_batch(const std::vector<State> &ancestor_states) {
  std::vector<int> ret;
  for (auto state : ancestor_states) {
    ret.push_back(compute_heuristic(state));
  }
  return ret;
}

class GooseKernelHeuristicFeature : public plugins::TypedFeature<Evaluator, GooseKernelHeuristic> {
 public:
  GooseKernelHeuristicFeature() : TypedFeature("kernel") {
    document_title("GOOSE optimised kernel heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<string>(
        "model_path",
        "path to trained model or model weights",
        "default_value");
    add_option<string>(
        "domain_file",
        "Path to the domain file.",
        "default_file");
    add_option<string>(
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

static plugins::FeaturePlugin<GooseKernelHeuristicFeature> _plugin;

}  // namespace goose_kernel_heuristic
