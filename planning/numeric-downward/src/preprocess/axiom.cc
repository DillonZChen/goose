#include "helper_functions.h"
#include "axiom.h"
#include "variable.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
using namespace std;

Axiom_relational::Axiom_relational(istream &in, const vector<Variable *> &variables) {
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

Axiom_numeric_computation::Axiom_numeric_computation(istream &in,
        const vector<NumericVariable *> &numeric_variables)
{
    int varNo, varNo1, varNo2;
    foperator foper;
    in >> varNo;
    in >> foper;
    fop = foper;
    in >> varNo1 >> varNo2 >> ws;
//    cout << "nv" << varNo << " := nv" << varNo1 << " " << foper << " nv" << varNo2 << endl;
    assert((int) numeric_variables.size() > varNo);
    assert((int) numeric_variables.size() > varNo1);
    assert((int) numeric_variables.size() > varNo2);
    effect_var = numeric_variables[varNo];
    effect_var->set_subterm();
    left_var = numeric_variables[varNo1];
    right_var = numeric_variables[varNo2];
}

Axiom_functional_comparison::Axiom_functional_comparison(istream &in,
		const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables)
{
    int varNo, varNo1, varNo2;
    compoperator coper;
    in >> varNo;
    in >> coper;
    cop = coper;
    in >> varNo1 >> varNo2 >> ws;
//    cout << varNo << " " << coper << " " << varNo1 << " ; " << varNo2 << endl;
    assert((int) variables.size() > varNo);
    assert((int) numeric_variables.size() > varNo1);
    assert((int) numeric_variables.size() > varNo2);
    effect_var = variables[varNo];
//    effect_var->dump();
    effect_var->set_comparison();

    left_var = numeric_variables[varNo1];
//    left_var->dump();
    right_var = numeric_variables[varNo2];
//    right_var->dump();
    string comp_string;
    string reverse_comp_string;
    stringify(coper, comp_string, reverse_comp_string);
    effect_var->set_fact_name(0, comp_string + " " + left_var->get_name() + ", " + right_var->get_name());
    effect_var->set_fact_name(1, reverse_comp_string + " " + left_var->get_name() + ", " + right_var->get_name());
}


bool Axiom_relational::is_redundant() const {
    return effect_var->get_level() == -1;
}

bool Axiom_numeric_computation::is_redundant() const {
    return effect_var->get_level() == -1;
}

bool Axiom_functional_comparison::is_redundant() const {
    return effect_var->get_level() == -1;
}


void strip_axiom_relationals(vector<Axiom_relational> &axioms) {
    int old_count = axioms.size();
    int new_index = 0;
    for (const Axiom_relational &axiom : axioms)
        if (!axiom.is_redundant())
            axioms[new_index++] = axiom;
    axioms.erase(axioms.begin() + new_index, axioms.end());
    cout << axioms.size() << " of " << old_count << " axiom rules necessary." << endl;
}

void strip_axiom_functional_assignment(vector<Axiom_numeric_computation> &axioms_fun_ass)
// strips duplicates. pruning of axioms operating on irrelevant variables is done during numeric relevance analysis
{
    int old_count = axioms_fun_ass.size();
    int new_index = 0;
    for(const Axiom_numeric_computation &axiom : axioms_fun_ass)
        if(!axiom.is_redundant())
            axioms_fun_ass[new_index++] = axiom;
    axioms_fun_ass.erase(axioms_fun_ass.begin() + new_index,
            axioms_fun_ass.end());
    cout << axioms_fun_ass.size() << " of " << old_count
        << " axiom_functional assignment rules necessary." << endl;
}

void strip_axiom_functional_comparisons(vector<Axiom_functional_comparison> &axioms_fun_comp)
// strips duplicates. pruning of axioms operating on irrelevant variables is done during numeric relevance analysis
{
    int old_count = axioms_fun_comp.size();
    int new_index = 0;
    for(const Axiom_functional_comparison &axiom : axioms_fun_comp)
        if(!axiom.is_redundant())
            axioms_fun_comp[new_index++] = axiom;
    axioms_fun_comp.erase(axioms_fun_comp.begin() + new_index,
            axioms_fun_comp.end());
    cout << axioms_fun_comp.size() << " of " << old_count
        << " axiom_functional comparison rules necessary." << endl;
}

void Axiom_relational::dump() const {
    cout << "axiom:" << endl;
    cout << "conditions:";
    for (const Condition &condition : conditions)
        cout << "  " << condition.var->get_name() << " := " << condition.cond;
    cout << endl;
    cout << "derived:" << endl;
    cout << effect_var->get_name() << " -> " << effect_val << endl;
    cout << endl;
}

void Axiom_numeric_computation::dump() const {
    cout << "functional assignment axiom:" << endl;
    cout << effect_var->get_name() << " := "
            << left_var->get_name() << " ";
    cout << fop;
    cout << " " << right_var->get_name() << endl;
}

void Axiom_functional_comparison::dump() const {
    cout << "functional comparison axiom:" << endl;
    cout << effect_var->get_name() << " := "
            << left_var->get_name() << " ";
    cout << cop;
    cout << " " << right_var->get_name() << endl;
}

string Axiom_relational::str() const {
	stringstream buf;
	buf << "[AX: " << effect_var->get_level() << " := ";
	for (size_t i=0; i < conditions.size(); ++i) {
		buf << conditions[i].var->get_level() << " & ";
	}
	buf << "]";
	return buf.str().replace(buf.str().length()-4, 3, ""); // return the string and crop the last " & "
}

string Axiom_numeric_computation::str() const {
	stringstream buf;
	buf << "[AX: " << effect_var->get_level() << " := " << left_var->get_level() << " " << fop << " " << right_var->get_level() << "]";
	return buf.str();
}

string Axiom_functional_comparison::str() const {
	stringstream buf;
	buf << "[AX: " << effect_var->get_level() << " := " << left_var->get_level() << " " << cop << " " << right_var->get_level() << "]";
	return buf.str();
}


int Axiom_relational::get_encoding_size() const {
    return 1 + conditions.size();
}

void Axiom_relational::generate_cpp_input(ofstream &outfile) const {
    assert(effect_var->get_level() != -1);
    outfile << "begin_rule" << endl;
    outfile << conditions.size() << endl;
    for (const Condition &condition : conditions) {
        assert(condition.var->get_level() != -1);
        outfile << condition.var->get_level() << " " << condition.cond << endl;
    }
    outfile << effect_var->get_level() << " " << old_val << " " << effect_val << endl;
    outfile << "end_rule" << endl;
}

//void Axiom_relational::set_relevant() const {
//	for (size_t i = 0; i< conditions.size(); ++i) {
//		conditions[i].var->set_relevant();
//	}
//}

void Axiom_numeric_computation::generate_cpp_input(ofstream &outfile) const
{
    assert(effect_var->get_level() != -1);
    assert(left_var->get_level() != -1);
    assert(right_var->get_level() != -1);
    outfile << effect_var->get_level() << " ";
    outfile << fop;
    outfile << " " << left_var->get_level() << " " << right_var->get_level() << endl;
}

void Axiom_functional_comparison::generate_cpp_input(ofstream &outfile) const
{
    assert(effect_var->get_level() != -1);
    outfile << effect_var->get_level() << " ";
    outfile << cop;
    outfile << " " << left_var->get_level() << " " << right_var->get_level() << endl;
}

int Axiom_numeric_computation::get_encoding_size() const {
    return 2;
}

int Axiom_functional_comparison::get_encoding_size() const {
    return 2;
}

//void Axiom_numeric_computation::set_instrumentation() const {
//	if (DEBUG) cout << "Setting Axiom " << str() << " instrumentation." << endl;
//	assert(effect_var->is_derived());
//	effect_var->set_type_at_least(derived_instrumentation);
//	if (left_var->is_derived()) {
//		left_var->set_type_at_least(derived_instrumentation);
//	}
//	else {
//		left_var->set_type_at_least(instrumentation);
//	}
//	if (right_var->is_derived())
//		right_var->set_type_at_least(derived_instrumentation);
//	else
//		right_var->set_type_at_least(instrumentation);
//}
//
//void Axiom_numeric_computation::set_relevant() const {
//	if (DEBUG) cout << "Setting Axiom " << str() << " relevant." << endl;
//	assert(effect_var->is_derived());
//	assert(effect_var->is_necessary());
//	effect_var->set_type_at_least(derived_regular);
//	if (left_var->is_derived())
//		left_var->set_type_at_least(derived_regular);
//	else
//		left_var->set_type_at_least(regular);
//	if (right_var->is_derived())
//		right_var->set_type_at_least(derived_regular);
//	else
//		right_var->set_type_at_least(regular);
//}

//void Axiom_functional_comparison::set_relevant() const {
//	if (DEBUG) cout << "Setting Axiom " << str() << " relevant." << endl;
//	assert(effect_var->is_derived());
//	assert(effect_var->is_necessary());
//	//effect_var->set_relevant();
//	if (left_var->is_derived())
//		left_var->set_type_at_least(derived_regular);
//	else
//		left_var->set_type_at_least(regular);
//	if (right_var->is_derived())
//		right_var->set_type_at_least(derived_regular);
//	else
//		right_var->set_type_at_least(regular);
//}
