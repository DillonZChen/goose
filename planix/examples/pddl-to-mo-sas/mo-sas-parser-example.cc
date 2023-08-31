#include <iostream>
#include <functional>
#include <random>

#include "../../src/ext/fast-downward-parser/problem.h"
#include "../../src/representations/sasplus.h"
#include "../../src/representations/translator.h"

using SasPlus::SasPlusAction;
using SasPlus::CostTransformer;


int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "\nUsage: " << argv[0] << " <pddl-domain> <pddl-problem>\n\n"
              << "Example: " << argv[0] << " ../pddl-to-mo-strips/mo_bw_domain.pddl ../pddl-to-mo-strips/mo_bw-5.pddl"
              << "\n\n";
    return 1;
  }

  using FastDownwardParser::SasProblem;
  SasProblem prob = FastDownwardParser::buildFromPDDLFile(argv[1], argv[2],
      // If 3rd argument is omitted then ./translate.py is used
      "../../src/ext/fast-downward-translator/translate.py"
  );
  //prob.dump();

  using SasPlus::SasPlusMultiObjDetPlanProb;
  SasPlusMultiObjDetPlanProb internal_sas = prob.convertToInternalSasPlus();

  std::cout << "Translated problem\n";
  std::cout << "Variables:\n";
  for (auto const& v : internal_sas.variables()) {
    std::cout << "  " << v.name << " -- domain size: " << v.domain << "\n";
  }

  std::cout << "The initial state is: " << internal_sas.initialState() << std::endl;
  std::cout << "The goal formula is: " << internal_sas.goal() << std::endl;

  std::cout << "Actions:\n";
  for (auto const& a : internal_sas.allActions()) {
    std::cout << "name: " << a.name() << "\n";
    std::cout << "  prec: " << a.precondition() << "\n";
    std::cout << "  eff:  " << a.effect() << "\n";
    std::cout << "  cost: " << "[";
    for (auto const& c : a.cost()) {
      std::cout << c << ", ";
    }
    std::cout << "]\n";
  }
}
