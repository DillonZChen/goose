#include "variable.h"

#include "helper_functions.h"

#include <sstream>
#include <cassert>
#include <fstream>
#include <iostream>
using namespace std;

ostream& operator<<(ostream &os, const numType &nt) {
    switch (nt) {
        case constant:
            os << "C";
            break;
        case regular:
            os << "R";
            break;
        case derived:
        	os << "D";
        	break;
        case instrumentation:
            os << "I";
            break;
        default:
            cout << (int) nt << " was read" << endl;
            if (nt == unknown)
            	cout << "Type of numeric variable not recognized";
            os << "X"; //
            assert(false);
    }
    return os;
}

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
    comparison = false;
}

void Variable::set_level(int theLevel) {
    assert(level == -1);
    level = theLevel;
}

int Variable::get_level() const {
    return level;
}

void Variable::set_necessary() {
    assert(necessary == false);
    necessary = true;
}

int Variable::get_range() const {
    return values.size();
}

string Variable::get_name() const {
    return name;
}

bool Variable::is_necessary() const {
    return necessary;
}

bool Variable::is_comparison() const {
    return comparison;
}

void Variable::set_comparison() {
    comparison = true;
}

void Variable::dump() const {
    // TODO: Dump values (and other information that might be missing?)
    //       or get rid of this if it's no longer needed.
    cout << name << " [range " << get_range();
    if (level != -1)
        cout << "; level " << level;
    if (is_derived())
        cout << "; derived; layer: " << layer;
    cout << "] {";
    for (const auto &fact : values)
    	cout << fact << ", ";

    cout << "}" << endl;
}

void Variable::generate_cpp_input(ofstream &outfile) const {
    outfile << "begin_variable" << endl
            << name << endl
            << layer << endl
            << values.size() << endl;
    for (size_t i = 0; i < values.size(); ++i)
        outfile << values[i] << endl;
    outfile << "end_variable" << endl;
}

void Variable::set_fact_name(int value, string new_name) {
	assert(value < (int) values.size());
	values[value] = new_name;
}

NumericVariable::NumericVariable(istream &in) {
	// value = 0.0; //initialize to zero
	char nvtype;
    in >> nvtype >> ws; // internal name e.g. numv0
    level = -1;
    ntype = unknown;
    necessary = false;
    if (nvtype == 'C') {ntype = constant;}
    if (nvtype == 'D') ntype = derived;
    string layers;
    getline(in,layers,' '); //
    layer = atoi(layers.c_str());
    getline(in,name);
}

void NumericVariable::set_value(double new_value) {
	value = new_value;
}

int NumericVariable::get_level() const {
	return level;

}

void NumericVariable::set_necessary() {
	assert(necessary == false);
	necessary = true;
	if(ntype == unknown)
		ntype = regular;
}

string NumericVariable::get_name() const {
	return name;
}

bool NumericVariable::is_necessary() const {
	return necessary;
}

bool NumericVariable::is_subterm() const {
	return subterm;
}

void NumericVariable::set_subterm() {
    subterm = true;
}

void NumericVariable::set_level(int new_level) {
    assert(level == -1);
	level = new_level;
}

void NumericVariable::set_layer(int new_layer) {
	layer = new_layer;
}

void NumericVariable::dump() const {
    // TODO: Dump values (and other information that might be missing?)
    //       or get rid of this if it's no longer needed.
    cout << "nv" << level << " : >" << name;
    if (level != -1)
        cout << "; level " << level;
    if (is_derived())
        cout << "; derived; layer: " << layer;
    cout << "<"<< endl;
}

void NumericVariable::generate_cpp_input(ofstream &outfile) const {
	assert(necessary);
	assert(layer >= -1);
    outfile << ntype << " " << layer << " " << name << endl;
}

void NumericVariable::set_instrumentation() {
	assert(necessary == false);
	necessary = true;
	if(ntype == unknown)
		ntype = instrumentation;
}

void Variable::decrement_layer(int decrement) {
	if (layer != -1) { // regular variables are in layer -1; the layers of derived variables have to be adjusted
		layer -= decrement;
	}
}
