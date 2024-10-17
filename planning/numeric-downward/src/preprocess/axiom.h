#ifndef AXIOM_H
#define AXIOM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Variable;
class NumericVariable;

class Axiom_relational {
public:
    struct Condition {
        Variable *var;
        int cond;
        Condition(Variable *v, int c) : var(v), cond(c) {}
    };
private:
    Variable *effect_var;
    int old_val;
    int effect_val;
    vector<Condition> conditions;    // var, val
public:
    Axiom_relational(istream &in, const vector<Variable *> &variables);
    void set_relevant() const;
    bool is_redundant() const;
    string str() const;
    void dump() const;
    int get_encoding_size() const;
    void generate_cpp_input(ofstream &outfile) const;
    const vector<Condition> &get_conditions() const {return conditions; }
    Variable *get_effect_var() const {return effect_var; }
    int get_old_val() const {return old_val; }
    int get_effect_val() const {return effect_val; }
};

class Axiom_functional_comparison
{
    private:
        Variable *effect_var;
        NumericVariable *left_var;
        NumericVariable *right_var;
    public:
        compoperator cop;
        Axiom_functional_comparison(istream &in, const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables);

        bool is_redundant() const;
        string str() const;
        void dump() const;
        void set_relevant() const;
        int get_encoding_size() const;
        void generate_cpp_input(ofstream &outfile) const;
        Variable* get_effect_var() const {
            return effect_var;
        }
        NumericVariable* get_left_var() const {
            return left_var;
        }
        NumericVariable* get_right_var() const {
            return right_var;
        }
};

class Axiom_numeric_computation
{
    private:
        NumericVariable *effect_var;
        NumericVariable *left_var;
        NumericVariable *right_var;
    public:
        foperator fop;
        Axiom_numeric_computation(istream &in, const vector<NumericVariable *> &numeric_variables);
        string str() const;
        bool is_redundant() const;
//        void set_relevant() const;
//        void set_metric() const;
//        void set_instrumentation() const;
        void dump() const;
        int get_encoding_size() const;
        void generate_cpp_input(ofstream &outfile) const;
        NumericVariable* get_effect_var() const {
            return effect_var;
        }
        NumericVariable* get_left_var() const {
            return left_var;
        }
        NumericVariable* get_right_var() const {
            return right_var;
        }
};



extern void strip_axiom_relationals(vector<Axiom_relational> &axioms_rel);
extern void strip_axiom_functional_comparisons(vector<Axiom_functional_comparison> &axioms_fun_comp);
extern void strip_axiom_functional_assignment(vector<Axiom_numeric_computation> &axioms_num);

#endif
