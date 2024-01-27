#ifndef SEARCH_ENGINES_EAGER_SEARCH_H
#define SEARCH_ENGINES_EAGER_SEARCH_H

#include <fstream>
#include <map>
#include <memory>
#include <vector>

#include "../open_list.h"
#include "../search_algorithm.h"

class Evaluator;
class PruningMethod;

namespace plugins {
class Feature;
}

namespace perfect {
class Perfect : public SearchAlgorithm {
  std::map<FactPair, std::string> fact_to_string;

  void initialise_grounded_facts();
  void write_state_to_file(const State &s, std::ofstream &plan_file);

 protected:
  virtual void initialize() override;
  virtual SearchStatus step() override;

 public:
  explicit Perfect(const plugins::Options &opts);
  virtual ~Perfect() = default;

  virtual void print_statistics() const override;

  void dump_search_space() const;
};

extern void add_options_to_feature(plugins::Feature &feature);
}  // namespace perfect

#endif
