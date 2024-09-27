#ifndef SEARCH_ENGINES_PLAN_SPACE_SUCCESSORS_H
#define SEARCH_ENGINES_PLAN_SPACE_SUCCESSORS_H

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

namespace plan_trace_successors {
class PlanTraceSuccessors : public SearchEngine {
 protected:
  virtual void initialize() override{};
  virtual SearchStatus step() override { return SOLVED; };

  bool validate_only;

  std::vector<std::string> op_names;
  std::unordered_map<std::string, int> op_name_to_i;

  void validate_plan();
  void print_plan_trace_successors();
  void print_state(const GlobalState &s, const GlobalState &parent_s, const std::string description, const ap_float &h, const std::string &op_name);


 public:
  explicit PlanTraceSuccessors(const options::Options &opts);
  virtual ~PlanTraceSuccessors() = default;

  virtual void print_statistics() const override{};
};
}  // namespace perfect

#endif
