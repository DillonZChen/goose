#ifndef AXIOM_H
#define AXIOM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

namespace FastDownwardParser {

class Variable;

class Axiom {
public:
    struct Condition {
        Variable *var;
        int cond;
        Condition(Variable *v, int c) : var(v), cond(c) {}
    };
private:
    Variable *effect_var;
    int old_val;
    int effect_val;
    vector<Condition> conditions;    // var, val
public:
    Axiom(istream &in, const vector<Variable *> &variables);

    void dump() const;
    const vector<Condition> &get_conditions() const {return conditions; }
    Variable *get_effect_var() const {return effect_var; }
    int get_old_val() const {return old_val; }
    int get_effect_val() const {return effect_val; }
};

}

#endif
