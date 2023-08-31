#include <filesystem>
#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <limits>

// TODO(fwt): replace C code for C++
#include <cstdio>
#include <cstring>

#include "problem.h"


namespace FastDownwardParser {

SasProblem buildFromPDDLFile(std::string const& domain_fname,
                             std::string const& problem_fname,
                             std::string const& translate_bin)
{
  if (!std::filesystem::exists(translate_bin)) {
    std::cerr << "[FastDownwardParser::buildFromPDDLFile] "
              << "unable to find the Fast Downward at " << translate_bin
              << ". Make sure to pass the correct path to buildFromPDDLFile\n";
    assert(std::filesystem::exists(translate_bin));
  }

  std::string cmd = translate_bin + " -q -o - " + domain_fname + " " + problem_fname;

  std::cout << "[FastDownwardParser::buildFromPDDLFile] executing: '" << cmd << "'" << std::endl;

  std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);

  if (!pipe) {
    std::cout << "[FastDownwardParser::buildFromPDDLFile] Failed to open pipe to cmd = '"
              << cmd << "'\n"
              << "Check if translation.py is in the path shown above. "
              // TODO(fwt) FIXME
//              << "If not, use the option '--translate-path' to change it\n"
              << "Quitting" << std::endl;
    exit(-1);
  }

  std::stringstream sstream_cleaned_file;
  bool found_header = false;
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, pipe.get())) != -1) {
    if (!found_header && strncmp(line, "begin_version", 13) == 0) {
      found_header = true;
    }
    if (found_header) {
      sstream_cleaned_file << line;
    }
//    else {
//      std::cout << "Ignoring line '" << line;
//    }
  }
  if (line)
    free(line);

  if (!found_header) {
    std::cout << "[FastDownwardParser::buildFromPDDLFile] Failed to find header.\n"
              << "Check if " << translate_bin << " exists. If not, use the option "
              << "'--translate-path' to change it.\nQuitting" << std::endl;
    exit(-1);
  }

  return SasProblem(sstream_cleaned_file);
}


size_t variableIndex(Variable* v, std::vector<Variable*> const& variables) {
  assert(v != nullptr);
  size_t var_idx = std::distance(variables.begin(),
                                 std::find(variables.begin(), variables.end(), v));
  return var_idx;
}


SasPlus::VariableDomSize variableValue([[maybe_unused]] size_t var_idx, int value,
                                       [[maybe_unused]] std::vector<SasPlus::SasPlusVariable> const& sas_variables)
{
  using SasPlus::VariableDomSize;
  assert(value >= 0);
  // Checking if value is representable otherwise signalling that VariableDomSize needs to change
  assert(value <= std::numeric_limits<VariableDomSize>::max());
  assert(value < sas_variables[var_idx].domain);
  return static_cast<VariableDomSize>(value);
}


SasPlusMultiObjDetPlanProb SasProblem::convertToInternalSasPlus() const {
  using SasPlus::SasPlusVariable;
  using SasPlus::VariableDomSize;
  using SasPlus::SasPlusState;
  using SasPlus::SasPlusPartialState;
  using SasPlus::SasPlusMultiCostAction;

  if (axioms.size() > 0) {
    std::cerr << "Axioms are not supported. Failing with assert.\n";
    assert(axioms.size() == 0);
  }

  std::vector<SasPlusVariable> sas_variables;
  for (auto const& var_ptr : variables) {
    assert(var_ptr != nullptr);
    assert(var_ptr->get_range() >= 0);
    sas_variables.push_back({var_ptr->get_name(),
                             static_cast<VariableDomSize>(var_ptr->get_range())});
  }

  SasPlusState sas_s0(sas_variables.size());
  for (size_t i = 0; i < variables.size(); ++i) {
    Variable* const& var_ptr = variables[i];
    assert(var_ptr != nullptr);
    sas_s0[i] = variableValue(i, initial_state[var_ptr], sas_variables);
  }


  SasPlusPartialState sas_goal;
  for (auto const& p : goals) {
    Variable* const& goal_var = p.first;
    size_t var_idx = variableIndex(goal_var, variables);
    sas_goal[var_idx] = variableValue(var_idx, p.second, sas_variables);
  }

  std::vector<SasPlusMultiCostAction> sas_actions;
  for (Operator const& op : operators) {
    SasPlusPartialState prec;
    for (auto const& prevail : op.get_prevail()) {
      size_t var_idx = variableIndex(prevail.var, variables);
      assert(prec.find(var_idx) == prec.end());
      prec[var_idx] = variableValue(var_idx, prevail.prev, sas_variables);
    }

    SasPlusPartialState eff;
    for (auto const& pre_post : op.get_pre_post()) {
      size_t var_idx = variableIndex(pre_post.var, variables);
      assert(eff.find(var_idx) == eff.end());
      assert(prec.find(var_idx) == prec.end());
      assert(!pre_post.is_conditional_effect);
      if (pre_post.pre != -1) {
        /*
         When prec_value is -1, it means that there is no precondition on that variable. From FD
         documentation (http://www.fast-downward.org/TranslatorOutputFormat):
           > (...) -1 if there is no particular value that the variable must have. (Note that is
           > truly part of the operator precondition and not an effect condition, and having it
           > separated from the operator precondition is somewhat clumsy. Let's call it a historical
           > accident caused by SAS+'s distinction of prevail and preconditions.)
        */
        prec[var_idx] = variableValue(var_idx, pre_post.pre, sas_variables);
      }
      eff[var_idx]  = variableValue(var_idx, pre_post.post, sas_variables);
    }
    sas_actions.push_back({op.get_name(), prec, eff, op.get_cost()});
  }

  return SasPlusMultiObjDetPlanProb(sas_variables, sas_s0, sas_goal, sas_actions);
}

}
