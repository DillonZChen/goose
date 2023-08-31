#include "helper_functions.h"
#include "operator.h"
#include "variable.h"

#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;

namespace FastDownwardParser {

Operator::Operator(istream &in, const vector<Variable *> &variables) {
    check_magic(in, "begin_operator");
    in >> ws;
    getline(in, name);
    int count; // number of prevail conditions
    in >> count;
    for (int i = 0; i < count; i++) {
        int varNo, val;
        in >> varNo >> val;
        prevail.push_back(Prevail(variables[varNo], val));
    }
    in >> count; // number of pre_post conditions
    for (int i = 0; i < count; i++) {
        int eff_conds;
        vector<EffCond> ecs;
        in >> eff_conds;
        for (int j = 0; j < eff_conds; j++) {
            int var, value;
            in >> var >> value;
            ecs.push_back(EffCond(variables[var], value));
        }
        int varNo, val, newVal;
        in >> varNo >> val >> newVal;
        if (eff_conds)
            pre_post.push_back(PrePost(variables[varNo], ecs, val, newVal));
        else
            pre_post.push_back(PrePost(variables[varNo], val, newVal));
    }
    in >> count; // number of cost objectives
    for (int i = 0; i < count; i++) {
        int value;
        in >> value;
        cost.push_back(static_cast<double>(value));
    }
    check_magic(in, "end_operator");
    // TODO: Evtl. effektiver: conditions schon sortiert einlesen?
}

void Operator::dump() const {
    cout << name << ":" << endl;
    cout << "prevail:";
    for (const auto &prev : prevail)
        cout << "  " << prev.var->get_name() << " := " << prev.prev;
    cout << endl;
    cout << "pre-post:";
    for (const auto &eff : pre_post) {
        if (eff.is_conditional_effect) {
            cout << "  if (";
            for (const auto &cond : eff.effect_conds)
                cout << cond.var->get_name() << " := " << cond.cond;
            cout << ") then";
        }
        cout << " " << eff.var->get_name() << " : "
             << eff.pre << " -> " << eff.post;
    }
    cout << "costs: ";
    for (int c : cost) {
        cout << c << " ";
    }
    cout << endl;
}


}
