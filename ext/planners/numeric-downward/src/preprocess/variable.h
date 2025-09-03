#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <vector>

using namespace std;

enum numType
{
	unknown = 0, // will be pruned away
	constant = 1, // constants are not altered during the task
	derived = 2, // derived variables appear in the effect of axioms
	instrumentation = 3,
	regular = 4
};

ostream& operator<<(ostream &os, const numType &nt);

class Variable {
    vector<string> values;
    string name;
    int layer;
    int level;
    bool necessary;
    bool comparison;
public:
    Variable(istream &in);
    void set_level(int level);
    void set_necessary();
    int get_level() const;
    bool is_necessary() const;
    int get_range() const;
    bool is_comparison() const;
    void set_comparison();
    string get_name() const;
    int get_layer() const {return layer; }
    void decrement_layer(int decrement);
    bool is_derived() const {return layer != -1;}
    void generate_cpp_input(ofstream &outfile) const;
    void dump() const;
    string get_fact_name(int value) const {return values[value]; }
    void set_fact_name(int value, string new_name); // used to rename comparison axiom facts
};

class NumericVariable {
    string name;
    double value; // initial value
    int layer; // axiom layer of the variable
    int level; // index in numeric_variables vector, determined by causal graph
    bool necessary;
    bool subterm;
    numType ntype;
public:
    NumericVariable(istream &in);
    void set_level(int new_level);
    void set_value(double new_value);
    void set_necessary();
    void set_instrumentation();
    void set_layer(int new_layer);
    bool is_necessary() const;
    int get_level() const;
    bool is_subterm() const;
    void set_subterm();
    string get_name() const;
    int get_layer() const {return layer; }
    bool is_derived() const {return (ntype == derived);}
    numType get_type() const {return ntype; }
    void generate_cpp_input(ofstream &outfile) const;
    void dump() const;
};

#endif
