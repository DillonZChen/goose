#ifndef MUTEX_GROUP_H
#define MUTEX_GROUP_H

#include <iostream>
#include <vector>
using namespace std;

namespace FastDownwardParser {

class Variable;

class MutexGroup {
    vector<pair<const Variable *, int>> facts;
public:
    MutexGroup(istream &in, const vector<Variable *> &variables);

    void dump() const;
};

}

#endif
