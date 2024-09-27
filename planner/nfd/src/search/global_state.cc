#include "global_state.h"

#include "../utils/strings.h"
#include "globals.h"
#include "state_registry.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
using namespace std;

GlobalState::GlobalState(const PackedStateBin *buffer_, const StateRegistry &registry_,
                         StateID id_)
    : buffer(buffer_), registry(&registry_), id(id_) {
  assert(buffer);
  assert(id != StateID::no_state);
}

GlobalState::~GlobalState() {}

container_int GlobalState::operator[](size_t index) const {
  return g_state_packer->get(buffer, index);
}

bool GlobalState::same_values(const GlobalState &state) const {
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    if (this->operator[](i) != state[i])
      return false;
  }
  std::vector<ap_float> this_numeric_values = this->get_numeric_vars();
  std::vector<ap_float> numeric_values = state.get_numeric_vars();
  for (size_t i = 0; i < this_numeric_values.size(); ++i) {
    if (g_numeric_var_types[i] == regular) {
      if (std::fabs(this_numeric_values[i] - numeric_values[i]) > 0.00001)
        return false;
    }
  }
  return true;
}

bool GlobalState::same_values(const std::vector<container_int> &values,
                              const std::vector<ap_float> &numeric_values) const {
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    if (this->operator[](i) != values[i])
      return false;
  }
  std::vector<ap_float> this_numeric_values = this->get_numeric_vars();
  for (size_t i = 0; i < this_numeric_values.size(); ++i) {
    if (g_numeric_var_types[i] == regular) {
      if (std::fabs(this_numeric_values[i] - numeric_values[i]) > 0.00001)
        return false;
    }
  }
  return true;
}

void GlobalState::dump_pddl() const {
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    const string &fact_name = g_fact_names[i][(*this)[i]];
    if (fact_name != "<none of those>")
      cout << fact_name << endl;
  }
}

// std::vector<ap_float> GlobalState::get_instrumentation_vars() const {
//	vector<ap_float> instvars;
//	assert(g_initial_state_data.size() == g_variable_domain.size());
//	assert(g_initial_state_numeric.size() == g_numeric_var_types.size());
//	for (size_t i = g_initial_state_data.size(); i< g_variable_domain.size()
//+ g_initial_state_numeric.size(); ++i) {
//		if(g_numeric_var_types[i-g_initial_state_data.size()] ==
// instrumentation) {
//			instvars.push_back(g_state_packer->unpackDouble((*this)[i]));
//		}
//	}
//	return instvars;
// }

std::string GlobalState::get_str_facts_and_fluents() const {
  std::cout << "Getting facts and fluents from NFD" << std::endl;
  std::string ret = "";
  ret += "__START_HERE__";
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    for (const string &fact_name : g_fact_names[i]) {
      if (fact_name != "<none of those>" &&
          fact_name.find("derived!") == std::string::npos &&
          fact_name.find("new-axiom@") == std::string::npos) {
        if (fact_name.find("NegatedAtom ") != std::string::npos) {
          ret += fact_name.substr(12) + "?";
        } else {
          ret += fact_name.substr(5) + "?";
        }
      }
    }
  }
  ret += ";";
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    std::string num_name = g_numeric_var_names[i];
    if (num_name != "<none of those>" &&
        num_name.find("derived!") == std::string::npos &&
        num_name.find("new-axiom@") == std::string::npos &&
        num_name.find("total-cost()") == std::string::npos) {
      if (num_name == "total-cost") {
        continue;
      } else if (num_name.find("PNE ") == std::string::npos) {
        std::cout << "error: PNE not found in var " << num_name << std::endl;
        exit(-1);
      }
      ret += num_name.substr(4) + "?";
    }
  }
  return ret;
}

void GlobalState::dump_facts_and_fluents() const {
  std::cout << get_str_facts_and_fluents() << std::endl;
}

void GlobalState::dump_readableB() const {
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    const string &fact_name = g_fact_names[i][(*this)[i]];
    if (fact_name != "<none of those>" &&
        fact_name.find("derived!") == std::string::npos &&
        fact_name.find("new-axiom@") == std::string::npos) {
      if (fact_name.find("Atom ") != std::string::npos and
          fact_name.find("NegatedAtom ") == std::string::npos) {
        std::cout << fact_name.substr(5) << "?";
      }
    }
  }
  std::cout << ";";
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    std::string num_name = g_numeric_var_names[i];
    if (num_name != "<none of those>" &&
        num_name.find("derived!") == std::string::npos &&
        num_name.find("new-axiom@") == std::string::npos &&
        num_name.find("total-cost()") == std::string::npos) {
      if (num_name.find("PNE ") == std::string::npos) {
        std::cout << "error: PNE not found in var " << num_name << std::endl;
        exit(-1);
      }
      std::cout << num_name.substr(4) << ":" << numeric_vals[i] << "?";
    }
  }
  // std::cout << std::endl;
}

void GlobalState::dump_readable() const {
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    const string &fact_name = g_fact_names[i][(*this)[i]];
    if (fact_name != "<none of those>" &&
        fact_name.find("derived!") == std::string::npos &&
        fact_name.find("new-axiom@") == std::string::npos) {
      if (fact_name.find("NegatedAtom ") != std::string::npos) {
        std::cout << i << ":" << fact_name.substr(12) << " -> " << false << std::endl;
      } else {
        std::cout << i << ":" << fact_name.substr(5) << " -> " << true << std::endl;
      }
    }
  }
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    std::string num_name = g_numeric_var_names[i];
    if (num_name != "<none of those>" &&
        num_name.find("derived!") == std::string::npos &&
        num_name.find("new-axiom@") == std::string::npos &&
        num_name.find("total-cost()") == std::string::npos) {
      if (num_name.find("PNE ") == std::string::npos) {
        std::cout << "error: PNE not found in var " << num_name << std::endl;
        exit(-1);
      }
      std::cout << i << ":" << num_name.substr(4) << " -> " << numeric_vals[i]
                << std::endl;
    }
  }
}

void GlobalState::dump_fdr() const {
  for (size_t i = 0; i < g_variable_domain.size(); ++i)
    cout << "  #" << i << " [" << g_variable_name[i] << "] -> "
         << g_fact_names[i][(*this)[i]] << " (" << (*this)[i] << ")" << endl;
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    cout << "  #" << g_variable_domain.size() + i << " [" << g_numeric_var_names[i]
         << "] -> " << numeric_vals[i] << endl;
  }
}

std::vector<string> GlobalState::get_facts(
    const std::vector<std::vector<std::string>> &fdr_pair_to_name,
    const std::vector<std::vector<bool>> &fdr_pair_to_is_true) const {
  std::vector<string> ret;
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    const int j = (*this)[i];
    const std::string &fact_name = fdr_pair_to_name[i][j];
    if ((fact_name.size() > 0) && fdr_pair_to_is_true[i][j]) {
      ret.push_back(fact_name);
    }
  }
  return ret;
}

std::vector<ap_float>
GlobalState::get_num_values(const std::vector<int> &indices) const {
  std::vector<ap_float> ret;
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (const int i : indices) {
    ret.push_back(numeric_vals[i]);
  }
  return ret;
}

std::vector<ap_float> GlobalState::get_numeric_vars() const {
  return registry->get_numeric_vars(*this);
}

std::string GlobalState::dump_plan_vis_log() const {
  stringstream outstream;
  for (size_t i = 0; i < g_variable_domain.size(); ++i)
    outstream << "{\"" << i << "\":" << (*this)[i] << "},";
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    outstream << " {\"" << g_variable_domain.size() + i << "\":" << numeric_vals[i]
              << "},";
  }
  string returnstring = outstream.str();
  returnstring.pop_back();
  return returnstring;
}

std::string GlobalState::get_numeric_state_vals_string() const {
  stringstream outstream;
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i)
    if (g_numeric_var_types[i] == regular) {
      outstream << fixed << g_numeric_var_names[i] << "="
                << registry->get_numeric_vars(*this)[i] << ";";
    }
  string returnstring = outstream.str();
  if (returnstring.length() > 0)
    returnstring.pop_back();
  return returnstring;
}

std::string GlobalState::dump_plan_vis_log(const GlobalState &parent) const {
  stringstream outstream;
  for (size_t i = 0; i < g_variable_domain.size(); ++i) {
    if ((*this)[i] != parent[i])
      outstream << "{\"" << i << "\":" << (*this)[i] << "},";
  }
  vector<ap_float> numeric_vals = registry->get_numeric_vars(*this);
  for (size_t i = 0; i < g_numeric_var_names.size(); ++i) {
    if (numeric_vals[i] != parent.get_numeric_vars()[i])
      outstream << "{\"" << g_variable_domain.size() + i << "\":" << numeric_vals[i]
                << "},";
  }
  string returnstring = outstream.str();
  returnstring.pop_back();
  return returnstring;
}
