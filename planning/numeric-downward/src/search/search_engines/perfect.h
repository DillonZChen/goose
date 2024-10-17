#ifndef SEARCH_ENGINES_PERFECT_H
#define SEARCH_ENGINES_PERFECT_H

#include <fstream>
#include <map>
#include <memory>
#include <vector>

#include "../open_lists/open_list.h"
#include "../search_engine.h"

class GlobalOperator;
class Heuristic;
class PruningMethod;
class ScalarEvaluator;

namespace options {
class Options;
}

namespace perfect {
class Perfect : public SearchEngine {
 protected:
  virtual void initialize() override{};
  virtual SearchStatus step() override { return SOLVED; };

 public:
  explicit Perfect(const options::Options &opts);
  virtual ~Perfect() = default;

  virtual void print_statistics() const override{};
};
}  // namespace perfect

#endif
