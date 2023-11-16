#ifndef GOOSE_LINEAR_REGRESSION_H
#define GOOSE_LINEAR_REGRESSION_H

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include "../heuristic.h"
#include "../goose/coloured_graph.h"


/* Optimised linear regression evaluation. 
  TODO: use OOP to reduce copied code with goose_kernel 
*/

namespace goose_linear_regression {

class GooseLinearRegression : public Heuristic {
  void initialise_model(const plugins::Options &opts);
  void initialise_facts();

  // map facts to a better data structure for heuristic computation
  std::map<FactPair, std::pair<std::string, std::vector<std::string>>> fact_to_lifted_input;

  /* Heuristic computation consists of three steps */

  // 1. convert state to CGraph
  CGraph state_to_graph(const State &state);

  // 2. perform WL on CGraph
  std::vector<int> wl_feature(const CGraph &graph);

  // 3. make a prediction with explicit feature
  int predict(const std::vector<int> &feature);

 protected:
  int compute_heuristic(const State &ancestor_state) override;
  std::vector<int> compute_heuristic_batch(const std::vector<State> &ancestor_states) override;
  
 public:
  explicit GooseLinearRegression(const plugins::Options &opts);

 private:
  CGraph graph_;
  std::map<std::string, int> hash_;
  std::vector<double> weights_;
  double bias_;
  int feature_size_;
  size_t iterations_;
};

}  // namespace goose_linear_regression

#endif  // GOOSE_LINEAR_REGRESSION_H
