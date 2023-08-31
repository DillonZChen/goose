#include "mutex_group.h"

#include "helper_functions.h"
#include "variable.h"

#include <fstream>
#include <iostream>


namespace FastDownwardParser {

MutexGroup::MutexGroup(istream &in, const vector<Variable *> &variables) {
    int size;
    check_magic(in, "begin_mutex_group");
    in >> size;
    for (int i = 0; i < size; ++i) {
        int var_no, value;
        in >> var_no >> value;
        facts.push_back(make_pair(variables[var_no], value));
    }
    check_magic(in, "end_mutex_group");
}

void MutexGroup::dump() const {
    cout << "mutex group of size " << facts.size() << ":" << endl;
    for (const auto &fact : facts) {
        const Variable *var = fact.first;
        int value = fact.second;
        cout << "   " << var->get_name() << " = " << value
             << " (" << var->get_fact_name(value) << ")" << endl;
    }
}


}
