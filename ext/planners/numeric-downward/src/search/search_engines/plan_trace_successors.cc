#include "plan_trace_successors.h"

#include "search_common.h"

#include "../evaluation_context.h"
#include "../globals.h"
#include "../heuristic.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../pruning_method.h"
#include "../successor_generator.h"
#include "../utils/planvis.h"
#include "../utils/timer.h"

#include "../open_lists/open_list_factory.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <set>
#include <sstream>

using namespace std;

namespace plan_trace_successors {
PlanTraceSuccessors::PlanTraceSuccessors(const Options &opts) : SearchEngine(opts) {
  validate_only = opts.get<bool>("validate_only");
  std::string plan_path = opts.get<std::string>("plan_path");
  std::ifstream plan_file(plan_path);
  std::string line;

  while (std::getline(plan_file, line)) {
    if (line[0] == '(') {
      op_names.push_back(line);
      op_name_to_i[line] = op_name_to_i.size();
      if (!validate_only) {
        std::cout << "_action^" << line << std::endl;
      }
    }
  }
  op_names.push_back("__dummy_to_print_goal__");

  if (validate_only) {
    validate_plan();
  } else {
    print_plan_trace_successors();
  }
}

void PlanTraceSuccessors::validate_plan() {
  std::cout << "\nValidating plan..." << std::endl;

  // get the NFD operators
  std::vector<GlobalOperator> plan_ops(op_names.size() - 1, g_operators[0]);
  std::vector<bool> found_op(op_names.size() - 1, false);
  if (validate_only) {
    for (const GlobalOperator &op : g_operators) {
      std::string op_name = "(" + op.get_name() + ")";
      if (op_name_to_i.find(op_name) != op_name_to_i.end()) {
        int idx = op_name_to_i[op_name];
        plan_ops[idx] = op;
        found_op[idx] = true;
      }
    }
    for (size_t i = 0; i < found_op.size(); i++) {
      if (!found_op[i]) {
        std::cout << "Invalid plan: non-existent operator " << op_names[i] << std::endl;
        exit(0);
      }
    }
  }

  // execute plan
  GlobalState s = g_initial_state();
  for (size_t i = 0; i < op_names.size(); i++) {
    std::string op_name = op_names[i];

    if (i == op_names.size() - 1) {
      break;  // goal should have no successors
    }

    s = g_state_registry->get_successor_state(s, plan_ops[i]);
  }


  if (test_goal(s)) {
    std::cout << "Plan is valid!" << std::endl;
  } else {
    std::cout << "Plan is invalid: does not end in goal." << std::endl;
  }
  exit(0);
}

void PlanTraceSuccessors::print_plan_trace_successors() {
  std::cout << "__START_HERE__";

  GlobalState state = g_initial_state();
  GlobalState opt_succ = g_initial_state();
  std::vector<GlobalState> successors;

  // get all fluents and facts
  std::cout << "_atoms";
  for (const auto &fact : get_fact_strings()) {
    std::cout << "|" << fact;
  }
  std::cout << std::endl;
  std::cout << "_fluents";
  for (const auto &fluent : get_fluent_strings()) {
    std::cout << "|" << fluent;
  }
  std::cout << std::endl;

  // execute plan and get successors
  for (size_t i = 0; i < op_names.size(); i++) {
    std::string op_name = op_names[i];
    successors.clear();

    vector<const GlobalOperator *> applicable_ops;
    g_successor_generator->generate_applicable_ops(state, applicable_ops);
    for (const GlobalOperator *op : applicable_ops) {
      if ("(" + op->get_name() + ")" == op_name) {
        // print in next iter
        opt_succ = g_state_registry->get_successor_state(state, *op);
      } else if (!validate_only) {
        GlobalState s2 = g_state_registry->get_successor_state(state, *op);
        successors.push_back(s2);
      }
    }

    // print current_state | opt_state | siblings
    std::cout<< "_state";
    std::cout << "|";
    state.dump_readableB();
    std::cout << "|";
    opt_succ.dump_readableB();
    for (const GlobalState &s : successors) {
      std::cout << "|";
      s.dump_readableB();
    }
    std::cout << std::endl;

    state = opt_succ;
  }

  std::cout << "__END_HERE__" << std::endl;
  exit(0);
}

static SearchEngine *_parse(OptionParser &parser) {
  parser.document_synopsis("Print states from plan", "");
  parser.add_option<string>("plan_path", "path to plan file", "_");
  parser.add_option<bool>("validate_only", "only validate plan", "false");
  SearchEngine::add_options_to_parser(parser);
  Options opts = parser.parse();
  PlanTraceSuccessors *engine = nullptr;
  if (!parser.dry_run()) {
    engine = new PlanTraceSuccessors(opts);
  }
  return engine;
}

static Plugin<SearchEngine> _plugin("plan_trace_successors", _parse);
}  // namespace plan_trace_successors
