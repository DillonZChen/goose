#ifndef GOOSE_BAYES_H
#define GOOSE_BAYES_H

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include "goose_wl_heuristic.h"
#include "../goose/coloured_graph.h"


/* Bayes model which calls python sklearn for evaluation */

namespace goose_bayes {

class GooseBayes : public goose_wl::WLGooseHeuristic {
 protected:
  int compute_heuristic(const State &ancestor_state) override;
  
 public:
  explicit GooseBayes(const plugins::Options &opts);

  // void print_statistics() const override;

//  private:
//   std::set<std::tuple<double, double, double>> std_ratio_pairs;
};

}  // namespace goose_bayes

#endif  // GOOSE_BAYES_H
