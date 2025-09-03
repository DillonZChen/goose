#ifndef GUROBI_IP_ITERATIVE_SEARCH_H
#define GUROBI_IP_ITERATIVE_SEARCH_H

#include <memory>

#include "../scalar_evaluator.h"
#include "../search_engine.h"
#include "../task_proxy.h"
#include "../utils/countdown_timer.h"
#include "../utils/language.h"
#include "../utils/system.h"
#include "ip_compilation.h"

namespace options {
class Options;
}

namespace gurobi_ip_compilation {

class GurobiIPCompilation;

class GurobiIterativeHorizon : public SearchEngine {
 protected:
  virtual void initialize() override;
  virtual SearchStatus step() override;
  int initial_h;
  int current_t;
  int iterations;
  bool last_iteration;

 public:
  explicit GurobiIterativeHorizon(const options::Options &opts);
  virtual ~GurobiIterativeHorizon() override;
  virtual void print_statistics() const override{};
  std::unique_ptr<GurobiIPCompilation> model;
  ScalarEvaluator *h_evaluator;
};

void add_model_option_to_parser(options::OptionParser &parser);
}  // namespace gurobi_ip_compilation

#endif
