#include "goose_wl_heuristic.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
#include "../goose/ilg.h"
#include "../goose/opg.h"

using std::string;
namespace py = pybind11;

namespace goose_wl {

WLGooseHeuristic::WLGooseHeuristic(const plugins::Options &opts)
    : goose_heuristic::GooseHeuristic(opts) {
  lifted_goose = true;
  initialise_lifted_facts();

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

  try {
    // Load python object model
    model_path = opts.get<std::string>("model_file");
    domain_file = opts.get<std::string>("domain_file");
    instance_file = opts.get<std::string>("instance_file");
    std::cout << "Trying to load model from file " << model_path << " ...\n";
    py::module util_module = py::module::import("models.save_load");
    model = util_module.attr("load_kernel_model_and_setup")(
        model_path, domain_file, instance_file);
    std::cout << "Loaded model!" << std::endl;

    // use I/O to get graph, WL and ML data (hash and weights)
    model.attr("write_model_data")();
    std::string model_data_path =
        model.attr("get_model_data_path")().cast<std::string>();
    update_model_from_data_path(model_data_path);
  } catch (py::error_already_set &e) {
    std::cout << "encountered some python error" << std::endl;
    exit(-1);
  }
}

void WLGooseHeuristic::update_model_from_data_path(
    const std::string model_data_path) {
  // load model data
  std::string line;
  std::ifstream infile(model_data_path);
  int hash_cnt = 0, hash_size = 0, weight_cnt = 0, weight_size = 0;

  hash_ = std::unordered_map<std::string, int>();

  // there's probably a better way to parse things
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
      continue;
    } else if (line.find("linear model(s)") != std::string::npos) {
      n_linear_models_ = stoi(toks[0]);
      weights_ = std::vector<std::vector<double>>(n_linear_models_,
                                                  std::vector<double>());
    } else if (line.find("weights size") != std::string::npos) {
      weight_size = stoi(toks[0]);
      weight_cnt = 0;
      continue;
    } else if (line.find("bias") != std::string::npos) {
      for (int i = 0; i < n_linear_models_; i++) {
        bias_.push_back(stod(toks[i]));
      }
      continue;
    } else if (line.find("iterations") != std::string::npos) {
      iterations_ = stoi(toks[0]);
      continue;
    } else if (line.find("representation") != std::string::npos) {
      graph_representation_ = toks[0];
      continue;
    } else if (line.find("wl_algorithm") != std::string::npos) {
      wl_algorithm_ = toks[0];
      continue;
    } else if (line.find("NO_EDGE") != std::string::npos) {
      NO_EDGE_ = stoi(toks[0]);
      continue;
    }

    if (hash_cnt < hash_size) {
      hash_[toks[0]] = stoi(toks[1]);
      hash_cnt++;
      continue;
    }

    if (weight_cnt < weight_size) {
      for (int i = 0; i < n_linear_models_; i++) {
        weights_[i].push_back(stod(toks[i]));
      }
      weight_cnt++;
      continue;
    }
  }

  cnt_seen_colours = std::vector<long>(iterations_, 0);
  cnt_unseen_colours = std::vector<long>(iterations_, 0);

  // remove file
  char *char_array = new char[model_data_path.length() + 1];
  strcpy(char_array, model_data_path.c_str());
  remove(char_array);

  // store feature size
  feature_size_ = static_cast<int>(hash_.size());
  std::cout << "Feature size: " << feature_size_ << std::endl;
  if (weights_[0].size() != 0 &&
      feature_size_ != static_cast<int>(weights_[0].size())) {
    std::cout << "error: hash size " << feature_size_ << " and weights size "
              << weights_[0].size() << " not the same" << std::endl;
    exit(-1);
  }

  // collect and check supported graph representation
  model.attr("write_representation_to_file")();
  std::string graph_data_path =
      model.attr("get_graph_file_path")().cast<std::string>();
  if (graph_representation_ == "ilg") {
    graph_ = std::make_shared<InstanceLearningGraph>(InstanceLearningGraph(graph_data_path));
  } else if (graph_representation_ == "opg") {
    graph_ = std::make_shared<ObjectPairGraph>(ObjectPairGraph(graph_data_path));
  } else {
    std::cout << "error: " << graph_representation_ << " not supported"
              << std::endl;
    exit(-1);
  }
  std::cout << "Model updating complete!" << std::endl;
}

void WLGooseHeuristic::print_statistics() const {
  for (size_t i = 0; i < iterations_; i++) {
    log << wl_algorithm_ << " seen/unseen colours in itr " << i << ": "
        << cnt_seen_colours[i] << " " << cnt_unseen_colours[i] << std::endl;
  }
}

std::shared_ptr<CGraph> WLGooseHeuristic::state_to_graph(const State &state) {
  PredicateArguments pred_args;
  for (const FactProxy &fact : convert_ancestor_state(state)) {
    pred_args.push_back(fact_to_l_input[fact.get_pair()]);
  }
  return graph_->predicate_arguments_to_graph(pred_args);
}

std::vector<int> WLGooseHeuristic::wl_feature(const std::shared_ptr<CGraph> &graph) {
  if (wl_algorithm_ == "1wl") {
    return wl1_feature(graph);
  } else if (wl_algorithm_ == "2gwl") {
    return gwl2_feature(graph);
  } else if (wl_algorithm_ == "2lwl") {
    return lwl2_feature(graph);
  } else {
    std::cout << "error: encountered invalid WL algorithm " << wl_algorithm_
              << std::endl;
    exit(-1);
  }
  return std::vector<int>();
}

std::vector<int> WLGooseHeuristic::wl1_feature(const std::shared_ptr<CGraph> &graph) {
  // feature to return is a histogram of colours seen during training
  std::vector<int> feature(feature_size_, 0);

  const size_t n_nodes = graph->n_nodes();

  // role of colours_0 and colours_1 is switched every iteration for storing old
  // and new colours
  std::vector<int> colours_0(n_nodes);
  std::vector<int> colours_1(n_nodes);
  std::vector<std::vector<std::pair<int, int>>> edges = graph->get_edges();

  // determine size of neighbour colours from the start
  std::vector<std::vector<std::pair<int, int>>> neighbours = edges;

  int col = -1;
  std::string new_colour;

  // collect initial colours
  for (size_t u = 0; u < n_nodes; u++) {
    // initial colours always in hash and hash value always within size
    col = hash_[std::to_string(graph->colour(u))];
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
          col = -1;
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
          new_colour += "," + std::to_string(ne_pair.first) + "," +
                        std::to_string(ne_pair.second);
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
        if (col == -1) {
          cnt_unseen_colours[itr]++;
        } else {
          cnt_seen_colours[itr]++;
        }
      }
    } else {
      for (size_t u = 0; u < n_nodes; u++) {
        // we ignore colours we have not seen during training
        if (colours_1[u] == -1) {
          col = -1;
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
          new_colour += "," + std::to_string(ne_pair.first) + "," +
                        std::to_string(ne_pair.second);
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
        if (col == -1) {
          cnt_unseen_colours[itr]++;
        } else {
          cnt_seen_colours[itr]++;
        }
      }
    }
  }

  return feature;
}

inline int pair_to_index_map(int n, int i, int j) {
  // map pair where 0 <= i < j < n to vec index
  return j - i - 1 + (i * n) - (i * (i + 1)) / 2;
}

inline int triple_to_index_map(int n, int i, int j, int k) {
  // map pair where 0 <= i < j < k < n to vec index
  // (i choose 1) + (j choose 2) + (k choose 3)
  return i + (j * (j - 1)) / 2 + (k * (k - 1) * (k - 2)) / 6;
}

std::vector<int> WLGooseHeuristic::gwl2_feature(const std::shared_ptr<CGraph> &graph) {
  // feature to return is a histogram of colours seen during training
  std::vector<int> feature(feature_size_, 0);

  const int n_nodes = static_cast<int>(graph->n_nodes());
  const int n_subsets = static_cast<int>((n_nodes * (n_nodes - 1)) / 2);

  // role of colours_0 and colours_1 is switched every iteration for storing old
  // and new colours
  std::vector<int> colours_0(n_subsets);
  std::vector<int> colours_1(n_subsets);
  std::vector<std::vector<std::pair<int, int>>> edges = graph->get_edges();

  // get edge labels between all pairs of nodes
  std::vector<int> pair_to_edge_label(n_subsets, NO_EDGE_);
  for (int u = 0; u < n_nodes; u++) {
    for (const auto &[v, edge_label] : edges[u]) {
      if (u < v) {
        pair_to_edge_label[pair_to_index_map(n_nodes, u, v)] = edge_label;
      }
    }
  }

  int col = -1;
  std::string new_colour;

  // collect initial colours
  for (int u = 0; u < n_nodes; u++) {
    for (int v = u + 1; v < n_nodes; v++) {
      int index = pair_to_index_map(n_nodes, u, v);
      // initial colours always in hash and hash value always within size
      std::string u_col = std::to_string(graph->colour(u));
      std::string v_col = std::to_string(graph->colour(v));
      std::string e_col = std::to_string(pair_to_edge_label[index]);
      new_colour = u_col + "," + v_col + "," + e_col;
      // not sure but maybe some pairs are not seen in training?
      if (hash_.count(new_colour)) {
        col = hash_[new_colour];
        feature[col]++;
        colours_0[index] = col;
      }
    }
  }

  // main 2-GWL algorithm loop
  std::vector<std::pair<int, int>> neighbour_colours(n_nodes - 2);
  int subset1, subset2, cs1, cs2;
  for (size_t itr = 0; itr < iterations_; itr++) {
    // instead of assigning colours_0 = colours_1 at the end of every loop
    // we just switch the roles of colours_0 and colours_1 every loop
    if (itr % 2 == 0) {
      for (int u = 0; u < n_nodes; u++) {
        for (int v = u + 1; v < n_nodes; v++) {
          int index = pair_to_index_map(n_nodes, u, v);
          col = colours_0[index];

          // we ignore colours we have not seen during training
          if (col == -1) {
            goto end_of_loop0;
          }

          // collect colours from neighbours and sort
          for (int w = 0; w < u; w++) {  // (w, u) (w, v)
            subset1 = pair_to_index_map(n_nodes, w, u);
            subset2 = pair_to_index_map(n_nodes, w, v);
            cs1 = colours_0[subset1];
            cs2 = colours_0[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop0;
            }
            neighbour_colours[w] =
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2));
          }
          for (int w = u + 1; w < v; w++) {  // (u, w) (w, v)
            subset1 = pair_to_index_map(n_nodes, u, w);
            subset2 = pair_to_index_map(n_nodes, w, v);
            cs1 = colours_0[subset1];
            cs2 = colours_0[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop0;
            }
            neighbour_colours[w - 1] =
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2));
          }
          for (int w = v + 1; w < n_nodes; w++) {  // (u, w) (v, w)
            subset1 = pair_to_index_map(n_nodes, u, w);
            subset2 = pair_to_index_map(n_nodes, v, w);
            cs1 = colours_0[subset1];
            cs2 = colours_0[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop0;
            }
            neighbour_colours[w - 2] =
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2));
          }

          sort(neighbour_colours.begin(), neighbour_colours.end());

          // add current colour and sorted neighbours into sorted colour key
          new_colour = std::to_string(col);
          for (const auto &ne_pair : neighbour_colours) {
            new_colour += "," + std::to_string(ne_pair.first) + "," +
                          std::to_string(ne_pair.second);
          }

          // hash seen colours
          if (hash_.count(new_colour)) {
            col = hash_[new_colour];
            feature[col]++;
          } else {
            col = -1;
          }
        end_of_loop0:
          colours_1[index] = col;
          if (col == -1) {
            cnt_unseen_colours[itr]++;
          } else {
            cnt_seen_colours[itr]++;
          }
        }
      }
    } else {
      for (int u = 0; u < n_nodes; u++) {
        for (int v = u + 1; v < n_nodes; v++) {
          int index = pair_to_index_map(n_nodes, u, v);
          col = colours_1[index];

          // we ignore colours we have not seen during training
          if (col == -1) {
            goto end_of_loop1;
          }

          // collect colours from neighbours and sort
          for (int w = 0; w < u; w++) {  // (w, u) (w, v)
            subset1 = pair_to_index_map(n_nodes, w, u);
            subset2 = pair_to_index_map(n_nodes, w, v);
            cs1 = colours_1[subset1];
            cs2 = colours_1[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop1;
            }
            neighbour_colours[w] =
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2));
          }
          for (int w = u + 1; w < v; w++) {  // (u, w) (w, v)
            subset1 = pair_to_index_map(n_nodes, u, w);
            subset2 = pair_to_index_map(n_nodes, w, v);
            cs1 = colours_1[subset1];
            cs2 = colours_1[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop1;
            }
            neighbour_colours[w - 1] =
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2));
          }
          for (int w = v + 1; w < n_nodes; w++) {  // (u, w) (v, w)
            subset1 = pair_to_index_map(n_nodes, u, w);
            subset2 = pair_to_index_map(n_nodes, v, w);
            cs1 = colours_1[subset1];
            cs2 = colours_1[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop1;
            }
            neighbour_colours[w - 2] =
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2));
          }

          sort(neighbour_colours.begin(), neighbour_colours.end());

          // add current colour and sorted neighbours into sorted colour key
          new_colour = std::to_string(col);
          for (const auto &ne_pair : neighbour_colours) {
            new_colour += "," + std::to_string(ne_pair.first) + "," +
                          std::to_string(ne_pair.second);
          }

          // hash seen colours
          if (hash_.count(new_colour)) {
            col = hash_[new_colour];
            feature[col]++;
          } else {
            col = -1;
          }
        end_of_loop1:
          colours_0[index] = col;
          if (col == -1) {
            cnt_unseen_colours[itr]++;
          } else {
            cnt_seen_colours[itr]++;
          }
        }
      }
    }
  }

  return feature;
}

std::vector<int> WLGooseHeuristic::lwl2_feature(const std::shared_ptr<CGraph> &graph) {
  // feature to return is a histogram of colours seen during training
  std::vector<int> feature(feature_size_, 0);

  const int n_nodes = static_cast<int>(graph->n_nodes());
  const int n_subsets = static_cast<int>((n_nodes * (n_nodes - 1)) / 2);

  // role of colours_0 and colours_1 is switched every iteration for storing old
  // and new colours
  std::vector<int> colours_0(n_subsets);
  std::vector<int> colours_1(n_subsets);
  std::vector<std::vector<std::pair<int, int>>> edges = graph->get_edges();

  // get edge labels between all pairs of nodes
  std::vector<int> pair_to_edge_label(n_subsets, NO_EDGE_);
  for (int u = 0; u < n_nodes; u++) {
    for (const auto &[v, edge_label] : edges[u]) {
      if (u < v) {
        pair_to_edge_label[pair_to_index_map(n_nodes, u, v)] = edge_label;
      }
    }
  }

  // get neighbours of all pairs of nodes
  std::vector<std::set<int>> node_to_neighbours(n_nodes, std::set<int>());
  for (int u = 0; u < n_nodes; u++) {
    for (const auto &[v, _] : edges[u]) {
      node_to_neighbours[u].insert(v);
    }
  }
  std::vector<std::set<int>> pair_to_neighbours(n_subsets, std::set<int>());
  for (int u = 0; u < n_nodes; u++) {
    for (int v = u + 1; v < n_nodes; v++) {
      int index = pair_to_index_map(n_nodes, u, v);
      pair_to_neighbours[index] = node_to_neighbours[u];
      pair_to_neighbours[index].insert(node_to_neighbours[v].cbegin(),
                                       node_to_neighbours[v].cend());
      pair_to_neighbours[index].erase(u);
      pair_to_neighbours[index].erase(v);
    }
  }

  int col = -1;
  std::string new_colour;

  // collect initial colours
  for (int u = 0; u < n_nodes; u++) {
    for (int v = u + 1; v < n_nodes; v++) {
      int index = pair_to_index_map(n_nodes, u, v);
      // initial colours always in hash and hash value always within size
      std::string u_col = std::to_string(graph->colour(u));
      std::string v_col = std::to_string(graph->colour(v));
      std::string e_col = std::to_string(pair_to_edge_label[index]);
      new_colour = u_col + "," + v_col + "," + e_col;
      // not sure but maybe some pairs are not seen in training?
      if (hash_.count(new_colour)) {
        col = hash_[new_colour];
        feature[col]++;
        colours_0[index] = col;
      }
    }
  }

  // main 2-GWL algorithm loop
  std::vector<std::pair<int, int>> neighbour_colours;
  int subset1, subset2, cs1, cs2;
  for (size_t itr = 0; itr < iterations_; itr++) {
    // instead of assigning colours_0 = colours_1 at the end of every loop
    // we just switch the roles of colours_0 and colours_1 every loop
    if (itr % 2 == 0) {
      for (int u = 0; u < n_nodes; u++) {
        for (int v = u + 1; v < n_nodes; v++) {
          int index = pair_to_index_map(n_nodes, u, v);
          col = colours_0[index];

          // we ignore colours we have not seen during training
          if (col == -1) {
            goto end_of_loop0;
          }

          // collect colours from neighbours and sort
          neighbour_colours = std::vector<std::pair<int, int>>();
          for (const int w : pair_to_neighbours[index]) {
            subset1 = pair_to_index_map(n_nodes, w, u);
            subset2 = pair_to_index_map(n_nodes, w, v);
            cs1 = colours_0[subset1];
            cs2 = colours_0[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop0;
            }
            neighbour_colours.push_back(
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2)));
          }

          sort(neighbour_colours.begin(), neighbour_colours.end());

          // add current colour and sorted neighbours into sorted colour key
          new_colour = std::to_string(col);
          for (const auto &ne_pair : neighbour_colours) {
            new_colour += "," + std::to_string(ne_pair.first) + "," +
                          std::to_string(ne_pair.second);
          }

          // hash seen colours
          if (hash_.count(new_colour)) {
            col = hash_[new_colour];
            feature[col]++;
          } else {
            col = -1;
          }
        end_of_loop0:
          colours_1[index] = col;
          if (col == -1) {
            cnt_unseen_colours[itr]++;
          } else {
            cnt_seen_colours[itr]++;
          }
        }
      }
    } else {
      for (int u = 0; u < n_nodes; u++) {
        for (int v = u + 1; v < n_nodes; v++) {
          int index = pair_to_index_map(n_nodes, u, v);
          col = colours_1[index];

          // we ignore colours we have not seen during training
          if (col == -1) {
            goto end_of_loop1;
          }

          // collect colours from neighbours and sort
          neighbour_colours = std::vector<std::pair<int, int>>();
          for (const int w : pair_to_neighbours[index]) {
            subset1 = pair_to_index_map(n_nodes, w, u);
            subset2 = pair_to_index_map(n_nodes, w, v);
            cs1 = colours_1[subset1];
            cs2 = colours_1[subset2];
            if (cs1 == -1 || cs2 == -1) {
              col = -1;
              goto end_of_loop1;
            }
            neighbour_colours.push_back(
                std::make_pair(std::min(cs1, cs2), std::max(cs1, cs2)));
          }

          sort(neighbour_colours.begin(), neighbour_colours.end());

          // add current colour and sorted neighbours into sorted colour key
          new_colour = std::to_string(col);
          for (const auto &ne_pair : neighbour_colours) {
            new_colour += "," + std::to_string(ne_pair.first) + "," +
                          std::to_string(ne_pair.second);
          }

          // hash seen colours
          if (hash_.count(new_colour)) {
            col = hash_[new_colour];
            feature[col]++;
          } else {
            col = -1;
          }
        end_of_loop1:
          colours_0[index] = col;
          if (col == -1) {
            cnt_unseen_colours[itr]++;
          } else {
            cnt_seen_colours[itr]++;
          }
        }
      }
    }
  }

  return feature;
}

std::vector<int> WLGooseHeuristic::lwl3_feature(const std::shared_ptr<CGraph> &graph) {
  // feature to return is a histogram of colours seen during training
  std::vector<int> feature(feature_size_, 0);

  std::cout << "not implemented" << std::endl;

  exit(-1);

  // const int n_nodes = static_cast<int>(graph->n_nodes());
  // const int n_pairs = static_cast<int>((n_nodes * (n_nodes - 1)) / 2);
  // const int n_triples = static_cast<int>((n_nodes * (n_nodes - 1) * (n_nodes
  // - 2)) / 6);

  // // role of colours_0 and colours_1 is switched every iteration for storing
  // old and new colours std::vector<int> colours_0(n_triples); std::vector<int>
  // colours_1(n_triples); std::vector<std::vector<std::pair<int, int>>> edges =
  // graph->get_edges();

  // // get neighbours of all triples of nodes
  // std::vector<std::set<int>> node_to_neighbours(n_nodes, std::set<int>());
  // for (int u = 0; u < n_nodes; u++) {
  //   for (const auto &[v, _] : edges[u]) {
  //     node_to_neighbours[u].insert(v);
  //   }
  // }
  // std::vector<std::set<int>> triple_to_neighbours(n_triples,
  // std::set<int>()); for (int u = 0; u < n_nodes; u++) {
  //   for (int v = u + 1; v < n_nodes; v++) {
  //     for (int w = v + 1; w < n_nodes; v++) {
  //       int index = triple_to_index_map(n_nodes, u, v, w);
  //       triple_to_neighbours[index] = node_to_neighbours[u];
  //       triple_to_neighbours[index].insert(node_to_neighbours[v].cbegin(),
  //                                          node_to_neighbours[v].cend());
  //       triple_to_neighbours[index].insert(node_to_neighbours[w].cbegin(),
  //                                          node_to_neighbours[w].cend());
  //       triple_to_neighbours[index].erase(u);
  //       triple_to_neighbours[index].erase(v);
  //       triple_to_neighbours[index].erase(w);
  //     }
  //   }
  // }

  // // get edge labels between all pairs of nodes
  // std::vector<int> pair_to_edge_label(n_pairs, NO_EDGE_);
  // for (int u = 0; u < n_nodes; u++) {
  //   for (const auto &[v, edge_label] : edges[u]) {
  //     if (u < v) {
  //       pair_to_edge_label[pair_to_index_map(n_nodes, u, v)] = edge_label;
  //     }
  //   }
  // }

  // // get 3 graph isomorphism types of all triples of nodes
  // std::vector<std::string> triple_to_isomorphism_type(n_triples);
  // // TODO(DZC) continue from here
  // exit(-1);

  // int col = -1;
  // std::string new_colour;

  // // collect initial colours
  // for (int u = 0; u < n_nodes; u++) {
  //   for (int v = u + 1; v < n_nodes; v++) {
  //     int index = pair_to_index_map(n_nodes, u, v);
  //     // initial colours always in hash and hash value always within size
  //     std::string u_col = std::to_string(graph->colour(u));
  //     std::string v_col = std::to_string(graph->colour(v));
  //     std::string e_col = std::to_string(pair_to_edge_label[index]);
  //     new_colour = u_col + "," + v_col + "," + e_col;
  //     // not sure but maybe some pairs are not seen in training?
  //     if (hash_.count(new_colour)) {
  //       col = hash_[new_colour];
  //       feature[col]++;
  //       colours_0[index] = col;
  //       cnt_seen_colours[itr]++;
  //     } else {
  //       cnt_unseen_colours[itr]++;
  //     }
  //   }
  // }

  // // main 2-GWL algorithm loop
  // std::vector<std::pair<int, int>> neighbour_colours;
  // int subset1, subset2, cs1, cs2;
  // for (size_t itr = 0; itr < iterations_; itr++) {
  //   // instead of assigning colours_0 = colours_1 at the end of every loop
  //   // we just switch the roles of colours_0 and colours_1 every loop
  //   if (itr % 2 == 0) {
  //     for (int u = 0; u < n_nodes; u++) {
  //       for (int v = u + 1; v < n_nodes; v++) {
  //         int index = pair_to_index_map(n_nodes, u, v);
  //         col = colours_0[index];

  //         // we ignore colours we have not seen during training
  //         if (col == -1) {
  //           goto end_of_loop0;
  //         }

  //         // collect colours from neighbours and sort
  //         neighbour_colours = std::vector<std::pair<int, int>>();
  //         for (const int w : pair_to_neighbours[index]) {
  //           subset1 = pair_to_index_map(n_nodes, w, u);
  //           subset2 = pair_to_index_map(n_nodes, w, v);
  //           cs1 = colours_0[subset1];
  //           cs2 = colours_0[subset2];
  //           if (cs1 == -1 || cs2 == -1) {
  //             col = -1;
  //             goto end_of_loop0;
  //           }
  //           neighbour_colours.push_back(std::make_pair(std::min(cs1, cs2),
  //           std::max(cs1, cs2)));
  //         }

  //         sort(neighbour_colours.begin(), neighbour_colours.end());

  //         // add current colour and sorted neighbours into sorted colour key
  //         new_colour = std::to_string(col);
  //         for (const auto &ne_pair : neighbour_colours) {
  //           new_colour +=
  //               "," + std::to_string(ne_pair.first) + "," +
  //               std::to_string(ne_pair.second);
  //         }

  //         // hash seen colours
  //         if (hash_.count(new_colour)) {
  //           col = hash_[new_colour];
  //           feature[col]++;
  //         } else {
  //           col = -1;
  //         }
  //       end_of_loop0:
  //         colours_1[index] = col;
  //         if (col == -1) {
  //           cnt_unseen_colours[itr]++;
  //         } else {
  //           cnt_seen_colours[itr]++;
  //         }
  //       }
  //     }
  //   } else {
  //     for (int u = 0; u < n_nodes; u++) {
  //       for (int v = u + 1; v < n_nodes; v++) {
  //         int index = pair_to_index_map(n_nodes, u, v);
  //         col = colours_1[index];

  //         // we ignore colours we have not seen during training
  //         if (col == -1) {
  //           goto end_of_loop1;
  //         }

  //         // collect colours from neighbours and sort
  //         neighbour_colours = std::vector<std::pair<int, int>>();
  //         for (const int w : pair_to_neighbours[index]) {
  //           subset1 = pair_to_index_map(n_nodes, w, u);
  //           subset2 = pair_to_index_map(n_nodes, w, v);
  //           cs1 = colours_1[subset1];
  //           cs2 = colours_1[subset2];
  //           if (cs1 == -1 || cs2 == -1) {
  //             col = -1;
  //             goto end_of_loop1;
  //           }
  //           neighbour_colours.push_back(std::make_pair(std::min(cs1, cs2),
  //           std::max(cs1, cs2)));
  //         }

  //         sort(neighbour_colours.begin(), neighbour_colours.end());

  //         // add current colour and sorted neighbours into sorted colour key
  //         new_colour = std::to_string(col);
  //         for (const auto &ne_pair : neighbour_colours) {
  //           new_colour +=
  //               "," + std::to_string(ne_pair.first) + "," +
  //               std::to_string(ne_pair.second);
  //         }

  //         // hash seen colours
  //         if (hash_.count(new_colour)) {
  //           col = hash_[new_colour];
  //           feature[col]++;
  //         } else {
  //           col = -1;
  //         }
  //       end_of_loop1:
  //         colours_0[index] = col;
  //         if (col == -1) {
  //           cnt_unseen_colours[itr]++;
  //         } else {
  //           cnt_seen_colours[itr]++;
  //         }
  //       }
  //     }
  //   }
  // }

  return feature;
}

}  // namespace goose_wl
