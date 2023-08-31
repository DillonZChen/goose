#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "state.h"
#include "variable.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace FastDownwardParser {

class State;
class MutexGroup;
class Operator;
class Axiom;
class DomainTransitionGraph;

//void read_everything
void read_preprocessed_problem_description(istream &in,
                                           vector<std::string> &metrics,
                                           vector<Variable> &internal_variables,
                                           vector<Variable *> &variables,
                                           vector<MutexGroup> &mutexes,
                                           State &initial_state,
                                           vector<pair<Variable *, int>> &goals,
                                           vector<Operator> &operators,
                                           vector<Axiom> &axioms);

//void dump_everything
void dump_preprocessed_problem_description(const vector<Variable *> &variables,
                                           const State &initial_state,
                                           const vector<pair<Variable *, int>> &goals,
                                           const vector<Operator> &operators,
                                           const vector<Axiom> &axioms);

void check_magic(istream &in, string magic);
}
#endif
