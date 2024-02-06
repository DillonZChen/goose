#ifndef GOOSE_LINEAR_ONLINE_H
#define GOOSE_LINEAR_ONLINE_H

#include <vector>

#include "../goose/coloured_graph.h"
#include "goose_linear.h"

/* Performs online learning by regressing from goal condition and using seen colours ratios to
decide what states to train on */

namespace goose_linear_online {

typedef std::vector<int> PartialState;
typedef std::vector<FactPair> FullState;

struct SearchNodeStats {
  int h;
  std::vector<double> ratio;

  SearchNodeStats(int h, const std::vector<double> ratio) : h(h), ratio(ratio){};
};

struct BackwardsSearchNode {
  PartialState state;
  int y;
  int h;
  // std::vector<double> ratio;

  BackwardsSearchNode(const PartialState &state, int y) : state(state), y(y) { h = 0; };

  BackwardsSearchNode(const PartialState &state, int y, int h) : state(state), y(y), h(h){};

  // BackwardsSearchNode(const PartialState &state, int y, const SearchNodeStats &stats)
  //     : state(state), h(stats.h), y(y), ratio(stats.ratio){};
};

class GooseLinearOnline : public goose_linear::GooseLinear {
  // big method containing the online training procedure
  // TODO(DZC) documentation
  void train();

  // does not account for mutexes
  FullState assign_random_state(const PartialState &state);
  SearchNodeStats compute_heuristic_vector_state(const FullState &state);
  FullState partial_state_to_fullstate_type(const PartialState &state);

  template <typename T>
  std::vector<T> get_random_elements(const std::vector<T> &originalVector, std::size_t n);

 public:
  explicit GooseLinearOnline(const plugins::Options &opts);

 private:
  // const size_t MAX_REGRESSION_STATES_ = 10000;
  const size_t MAX_REGRESSION_STATES_ = 100000;
  // const size_t MAX_REGRESSION_STATES_ = 1000000;

  int n_variables;
  std::mt19937 rng;
  VariablesProxy vars;
};

}  // namespace goose_linear_online

#endif  // GOOSE_LINEAR_ONLINE_H
