#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Variable;
class NumericVariable;

class State {
    map<Variable *, int> values;
    map<NumericVariable *, double> numeric_values;
public:
    State() {} // TODO: Entfernen (erfordert kleines Redesign)
    State(istream &in, const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables);
    double get_nv(NumericVariable *var) const;
    int numeric_size() const {return numeric_values.size();}
    int operator[](Variable *var) const;
    void dump() const;
};

#endif
