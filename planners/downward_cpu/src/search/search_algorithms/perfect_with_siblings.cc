#include "perfect_with_siblings.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <map>
#include <vector>

#include "../evaluation_context.h"
#include "../evaluator.h"
#include "../open_list_factory.h"
#include "../pruning_method.h"

#include "../algorithms/ordered_set.h"
#include "../plugins/options.h"
#include "../task_utils/successor_generator.h"
#include "../utils/logging.h"

using namespace std;

namespace perfectSiblings {
    PerfectSiblings::PerfectSiblings(const plugins::Options &opts) : SearchAlgorithm(opts) {}

void PerfectSiblings::initialize() {
  initialise_grounded_facts();
  State initial_state = state_registry.get_initial_state();
}

void PerfectSiblings::print_statistics() const {
  statistics.print_detailed_statistics();
  search_space.print_statistics();
}

void PerfectSiblings::initialise_grounded_facts() {
  FactsProxy facts(*task);
  for (FactProxy fact : facts) {
    // TODO(DZC) this is an artifact of FD-Hypernet/STRIPS-HGN code
    // we can remove this conversion in both the code here and in python
    string name = fact.get_name();

    // Convert from FDR var-val pairs back to propositions
    if (name == "<none of those>") {
      continue;
    } else {
      if (name.substr(0, 5) == "Atom ") {
        name = name.substr(5);
      } else if (name.substr(0, 12) == "NegatedAtom ") {
        continue;
      } else {
        std::cout << "Substring of downward fact does not start with 'Atom ': "
                  << "or 'NegatedAtom '" << name << std::endl;
        exit(-1);
      }
    }

    fact_to_string.insert({fact.get_pair(), name});
  }
}

void PerfectSiblings::write_state_to_file(const State &s, std::ofstream &plan_file) {
  State state = task_proxy.convert_ancestor_state(s);
  for (FactProxy fact : state) {
    std::string str = fact_to_string[fact.get_pair()];
    std::string str2 = "()";
    if (str.find(str2) != std::string::npos) {
      str.replace(str.find(str2), std::string(str2).length(), "");
    }
    str2 = ")";
    if (str.find(str2) != std::string::npos) {
      str.replace(str.find(str2), std::string(str2).length(), ",)");
    }
    for (size_t i = 0; i < str.length(); i++) {
      str2 = " ";
      if (str.find(str2) != std::string::npos) {
        str.replace(str.find(str2), std::string(str2).length(), "");
      }
    }
    plan_file << str << " ";
  }
  plan_file << endl;
}

inline bool is_goal_state(TaskProxy task, const State &state) {
  for (FactProxy goal : task.get_goals()) {
    if (state[goal.get_variable()] != goal)
      return false;
  }
  return true;
}

SearchStatus PerfectSiblings::step() {
  State s = state_registry.get_initial_state();
  std::ofstream output_file(std::getenv("STATES_OUTPUT_PATH"));
  write_state_to_file(s, output_file);
  output_file << endl;

  std::map<std::string, size_t> name_to_op_id;
  for (size_t op_id = 0; op_id < task_proxy.get_operators().size(); op_id++) {
    OperatorProxy op = task_proxy.get_operators()[op_id];
    name_to_op_id[op.get_name()] = op_id;
//    std::cout << op.get_name() << std::endl;
  }

  std::string line;
  State s_next = s;
  ifstream file(std::getenv("PLAN_INPUT_PATH"));
  if (file.is_open()) {
    while (getline(file, line)) {
      if (line[0] == ';') {  // finished parsing
        break;
      }

      line = line.substr(1, line.size() - 2);

      if (name_to_op_id.count(line)) {
        size_t op_id = name_to_op_id[line];
        OperatorProxy op = task_proxy.get_operators()[op_id];
        s_next = state_registry.get_successor_state(s, op);
        write_state_to_file(s_next, output_file);
        vector<OperatorID> applicable_ops;
        successor_generator.generate_applicable_ops(s, applicable_ops);
        for (OperatorID appl_op_id : applicable_ops) {
          OperatorProxy appl_op = task_proxy.get_operators()[appl_op_id];
          if (appl_op == op){
            continue;
          }
          State succ_state = state_registry.get_successor_state(s, appl_op);
          write_state_to_file(succ_state, output_file);
        }
        s = s_next;
        output_file << endl;
      } else {
        std::cout << "invalid plan because cannot find action " << line << std::endl;
        return FAILED;
      }
    }

    file.close();
    if (!is_goal_state(task_proxy, s)) {
      std::cout << "invalid plan because final state is not a goal state" << std::endl;
      return FAILED;
    }
  } else {
    cout << "Unable to open file";
    return FAILED;
  }
  output_file << "; GOOD";
  return SOLVED;
}

void PerfectSiblings::dump_search_space() const { search_space.dump(task_proxy); }

void add_options_to_feature(plugins::Feature &feature) {
  SearchAlgorithm::add_pruning_option(feature);
  SearchAlgorithm::add_options_to_feature(feature);
}
}  // namespace perfect
