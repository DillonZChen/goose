#ifndef EXT_SAS_PARSER_PROBLEM
#define EXT_SAS_PARSER_PROBLEM

#include <iostream>
#include <vector>

#include "state.h"
#include "mutex_group.h"
#include "operator.h"
#include "axiom.h"
#include "variable.h"
#include "helper_functions.h"

#include "../../representations/sasplus.h"


// helper class to wrap everything together

namespace FastDownwardParser {

using SasPlus::SasPlusMultiObjDetPlanProb;

struct SasProblem {
  SasProblem() { }
  SasProblem(std::istream& input_s) {
    read_preprocessed_problem_description(input_s, metrics, internal_variables,
                                          variables, mutexes, initial_state,
                                          goals, operators, axioms);
  }

  ~SasProblem() { }

  void dump() const {
    dump_preprocessed_problem_description(variables, initial_state, goals,
                                          operators, axioms);
  }

  SasPlusMultiObjDetPlanProb convertToInternalSasPlus() const;

  std::vector<std::string> metrics;
  std::vector<Variable*> variables;
  std::vector<Variable> internal_variables;
  State initial_state;
  std::vector<pair<Variable*, int>> goals;
  std::vector<MutexGroup> mutexes;
  std::vector<Operator> operators;
  std::vector<Axiom> axioms;
};

SasProblem buildFromPDDLFile(std::string const& domain_fname, std::string const& problem_fname,
                             std::string const& translate_bin = "./translate.py");

} // namespace FastDownwardParser

#endif // EXT_SAS_PARSER_PROBLEM
