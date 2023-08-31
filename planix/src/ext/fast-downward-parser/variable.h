#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <vector>
using namespace std;

namespace FastDownwardParser {

class Variable {
    vector<string> values;
    string name;
    int layer;
    int level;
    bool necessary;
public:
    Variable(istream &in);
    int get_range() const;
    string get_name() const;
    int get_layer() const {return layer; }
    bool is_derived() const {return layer != -1; }
    void dump() const;
    string get_fact_name(int value) const {return values[value]; }
};

}
#endif
