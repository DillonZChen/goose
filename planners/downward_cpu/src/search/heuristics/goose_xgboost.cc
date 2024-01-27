#include "goose_xgboost.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

using std::string;

namespace goose_xgboost {

GooseXGBoost::GooseXGBoost(const plugins::Options &opts)
    : goose_wl::WLGooseHeuristic(opts) {
  std::string json_path = model.attr("get_xgboost_json_path")().cast<std::string>();

  std::cout << "Loading XGBoost model direclty into c++ from " << json_path
            << " ..." << std::endl;

  XGBoosterCreate(NULL, 0, &booster);
  XGBoosterLoadModel(booster, json_path.c_str());

  std::cout << "XGBoost model correctly loaded into c++!" << std::endl;
}

int GooseXGBoost::predict(const std::vector<int> &feature) {

  // Creating a DMatrix from the input feature vector
  DMatrixHandle dmatrix;
  std::vector<float> feature_as_float(feature.begin(), feature.end());
  const float *d_values = feature_as_float.data();
  auto data_code =
      XGDMatrixCreateFromMat(&d_values[0], 1, feature.size(), -1, &dmatrix);

  // Getting prediction
  char const config[] =
      "{\"training\": false, \"type\": 0, "
      "\"iteration_begin\": 0, \"iteration_end\": 0, \"strict_shape\": false}";
  uint64_t const *out_shape;
  uint64_t out_dim;
  float const *out_result = NULL;
  auto pred_code = XGBoosterPredictFromDMatrix(
      booster, dmatrix, config, &out_shape, &out_dim, &out_result);

  // data_code and pred_code should be 0, otherwise an error occured somewhere
  // std::cout << data_code << " " << pred_code << std::endl;

  int h = static_cast<int>(round(out_result[0]));
  // std::cout << h << std::endl;

  XGDMatrixFree(dmatrix);

  return h;
}

int GooseXGBoost::compute_heuristic(const State &ancestor_state) {
  // step 1.
  std::shared_ptr<CGraph> graph = state_to_graph(ancestor_state);
  // step 2.
  std::vector<int> feature = wl_feature(graph);
  // step 3.
  int h = predict(feature);
  return h;
}

class GooseXGBoostFeature
    : public plugins::TypedFeature<Evaluator, GooseXGBoost> {
 public:
  GooseXGBoostFeature() : TypedFeature("xgboost_model") {
    document_title("GOOSE optimised WL xgboost heuristic");
    document_synopsis("TODO");

    // https://github.com/aibasel/downward/pull/170 for string options
    add_option<std::string>("model_file", "path to trained python model",
                            "default_value");
    add_option<std::string>("domain_file", "Path to the domain file.",
                            "default_file");
    add_option<std::string>("instance_file", "Path to the instance file.",
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

static plugins::FeaturePlugin<GooseXGBoostFeature> _plugin;

}  // namespace goose_xgboost
