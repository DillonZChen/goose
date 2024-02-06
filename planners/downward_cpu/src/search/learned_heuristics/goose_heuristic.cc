#include "goose_heuristic.h"

#include <fstream>
#include <iostream>

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

using std::string;

namespace goose_heuristic {

GooseHeuristic::GooseHeuristic(const plugins::Options &opts) : Heuristic(opts) {}

void GooseHeuristic::initialise_grounded_facts() {
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

    // replace all occurrences of '(' and ')' by ' '
    std::replace(name.begin(), name.end(), '(', ' ');
    std::replace(name.begin(), name.end(), ')', ' ');

    // Remove occurrences of ','
    name.erase(std::remove(name.begin(), name.end(), ','), name.end());

    // Trim string
    if (std::isspace(name[0])) {
      name.erase(0, 1);
    }
    if (std::isspace(name.back())) {
      name.erase(name.end() - 1, name.end());
    }

    // Add parentheses around string
    name = "(" + name + ")";
    fact_to_g_input.insert({fact.get_pair(), name});
  }
}

void GooseHeuristic::initialise_lifted_facts() {
  FactsProxy facts(*task);
  for (FactProxy fact : facts) {
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

    // replace all occurrences of '(' and ')' by ' '
    std::replace(name.begin(), name.end(), '(', ' ');
    std::replace(name.begin(), name.end(), ')', ' ');

    // Remove occurrences of ','
    name.erase(std::remove(name.begin(), name.end(), ','), name.end());

    // Trim string
    if (std::isspace(name[0])) {
      name.erase(0, 1);
    }
    if (std::isspace(name.back())) {
      name.erase(name.end() - 1, name.end());
    }

    std::istringstream iss(name);
    std::string s;
    std::string pred = "";
    std::vector<std::string> args;

    while (std::getline(iss, s, ' ')) {
      if (pred == "") {
        pred = s;
      } else {
        args.push_back(s);
      }
    }
    std::pair<std::string, std::vector<std::string>> lifted_fact(pred, args);

    fact_to_l_input.insert({fact.get_pair(), lifted_fact});
  }
}

void GooseHeuristic::initialise_facts() {
  if (lifted_goose) {
    initialise_lifted_facts();
  } else {
    initialise_grounded_facts();
  }
}

GooseState GooseHeuristic::fd_state_to_goose_state(const State &ancestor_state) {
  State state = convert_ancestor_state(ancestor_state);

  GooseState goose_state;
  if (lifted_goose) {
    for (FactProxy fact : state) {
      goose_state.append(fact_to_l_input[fact.get_pair()]);
    }
  } else {
    for (FactProxy fact : state) {
      goose_state.append(fact_to_g_input[fact.get_pair()]);
    }
  }
  return goose_state;
}

GooseState GooseHeuristic::fact_pairs_list_to_goose_state(const std::vector<FactPair> &fact_pairs) {
  GooseState goose_state;
  if (lifted_goose) {
    for (FactPair fact_pair : fact_pairs) {
      goose_state.append(fact_to_l_input[fact_pair]);
    }
  } else {
    for (FactPair fact_pair : fact_pairs) {
      goose_state.append(fact_to_g_input[fact_pair]);
    }
  }
  return goose_state;
}

}  // namespace goose_heuristic
