#include "helper_functions.h"
#include "axiom.h"
#include "variable.h"

#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

namespace FastDownwardParser {
Axiom::Axiom(istream &in, const vector<Variable *> &variables) {
    check_magic(in, "begin_rule");
    int count; // number of conditions
    in >> count;
    for (int i = 0; i < count; i++) {
        int varNo, val;
        in >> varNo >> val;
        conditions.push_back(Condition(variables[varNo], val));
    }
    int varNo, oldVal, newVal;
    in >> varNo >> oldVal >> newVal;
    effect_var = variables[varNo];
    old_val = oldVal;
    effect_val = newVal;
    check_magic(in, "end_rule");
}


void Axiom::dump() const {
    cout << "axiom:" << endl;
    cout << "conditions:";
    for (const Condition &condition : conditions)
        cout << "  " << condition.var->get_name() << " := " << condition.cond;
    cout << endl;
    cout << "derived:" << endl;
    cout << effect_var->get_name() << " -> " << effect_val << endl;
    cout << endl;
}

}
