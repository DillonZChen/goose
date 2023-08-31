#include "variable.h"

#include "helper_functions.h"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

namespace FastDownwardParser {

Variable::Variable(istream &in) {
    int range;
    check_magic(in, "begin_variable");
    in >> ws >> name >> layer >> range >> ws;
    values.resize(range);
    for (int i = 0; i < range; ++i)
        getline(in, values[i]);
    check_magic(in, "end_variable");
    level = -1;
    necessary = false;
}


int Variable::get_range() const {
    return values.size();
}

string Variable::get_name() const {
    return name;
}

void Variable::dump() const {
    // TODO: Dump values (and other information that might be missing?)
    //       or get rid of this if it's no longer needed.
    cout << name << " [range " << get_range();
    if (level != -1)
        cout << "; level " << level;
    if (is_derived())
        cout << "; derived; layer: " << layer;
    cout << "]" << endl;
}

}
