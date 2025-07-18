#include "state.h"
#include "helper_functions.h"
#include <cassert>

class Variable;
class NumericVariable;

State::State(istream &in, const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables) {
    check_magic(in, "begin_state");
    for (Variable *var : variables) {
        int value;
        in >> value; //for axioms, this is default value
        values[var] = value;
    }
    check_magic(in, "end_state");
    check_magic(in, "begin_numeric_state");
    for (NumericVariable *numvar : numeric_variables) {
//    	cout << "assigning initial value to numeric variable " << numeric_variables[i]->get_name();
        double num_value;
        in >> num_value;
//        cout << " -> " << num_value << endl;
        numeric_values[numvar] = num_value;
    }
    check_magic(in, "end_numeric_state");
}

int State::operator[](Variable *var) const {
    return values.find(var)->second;
}

double State::get_nv(NumericVariable *var) const {
	assert(var);
	return numeric_values.find(var)->second;
}

void State::dump() const {
    for (const auto &value : values)
        cout << "  " << value.first->get_name() << ": " << value.second << endl;
}
