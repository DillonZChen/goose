#ifndef PDDL_TO_STRIPS_H_
#define PDDL_TO_STRIPS_H_

#include <iostream>

#include "problems.h"

#include "representations/strips_problems.h"

using STRIPS::StripsMultiObjectiveProblem;
using STRIPS::StripsProblem;
using MdpsimProblem = PPDDL::Problem;

namespace PPDDL {


/// Parse a given PPDDL file containing both the domain and problem and returns the Multi-Object
/// STRIPS problem.
StripsMultiObjectiveProblem parseToStrips(std::string const& domain_and_problem_fname);

/// Parse the pair of PPDDL files (domain first and problem after) and problem and returns the
/// Multi-Object STRIPS problem.
StripsMultiObjectiveProblem parseToStrips(std::string const& domain_fname,
                                          std::string const& problem_fname);

/// Translate a given PPDDL::Problem to STRIPS::StripsMultiObjectiveProblem
StripsMultiObjectiveProblem translateToMoStrips(MdpsimProblem const& problem);

/// Translate a given PPDDL::Problem to STRIPS::StripsProblem
StripsProblem translateToStrips(MdpsimProblem const& problem);

}  // namespace PPDDL
#endif  // PDDL_TO_STRIPS_H_
