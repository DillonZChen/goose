/*
 * Main program.
 *
 * Copyright 2003-2005 Carnegie Mellon University and Rutgers University
 * Copyright 2007 Håkan Younes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

#include "pybind_goal_count.h"

#include "ext/mdpsim-parser/pddl_to_strips.h"
#include "representations/strips_problems.h"

using STRIPS::StripsState;
using STRIPS::StripsAction;


std::vector<std::string> tokenise(const std::string str) {
  std::istringstream iss(str);
  std::string s;
  std::vector<std::string> ret;
  while (std::getline(iss, s, ' ')) {
    ret.push_back(s);
  }
  return ret;
}

/* The main program. */
int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cout
      << "Usage: " << argv[0] << " <single-ppddl-file|domain-file problem-file> <plan-file>\n"
      << "  where single-ppddl-file is a single file containing a PPDDL domain and problem\n"
      << "  alternatively, a pair of domain-file and problem-file can be used.\n\n";
    return 0;
  }

  // Parse in problem with MdpSim first
  PPDDL::Problem const* problem = nullptr;
  std::string path;
  if (argc == 3) {
    std::cout << "PDDL file: " << argv[1] << std::endl;
    problem = PPDDL::parsePPDDL(argv[1]);
    path = argv[2];
  } else {
    assert(argc == 4);
    problem = PPDDL::parsePPDDL(argv[1], argv[2]);
    path = argv[3];
  }
  assert(problem);

  // If only the STRIPS problem is needed, then call PPDDL::parseToStrips
  StripsProblem strips_problem = PPDDL::translateToStrips(*problem);
  StripsState state = strips_problem.initialState();
  std::map<std::string, StripsAction> name_to_action;
  for (auto const &action : strips_problem.allActions()) {
    name_to_action[action.name()] = action;
  }


  std::string line;
  std::vector<std::string> toks;
  std::ifstream infile(path);

  // collect graph information
  while (std::getline(infile, line)) {
    toks = tokenise(line);
    if (toks[0] == ";") {
      break;
    }

  }


  return 0;
}
