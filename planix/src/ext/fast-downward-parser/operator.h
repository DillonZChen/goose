#ifndef OPERATOR_H
#define OPERATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

namespace FastDownwardParser {

class Variable;

class Operator {
public:
    struct Prevail {
        Variable *var;
        int prev;
        Prevail(Variable *v, int p) : var(v), prev(p) {}
    };
    struct EffCond {
        Variable *var;
        int cond;
        EffCond(Variable *v, int c) : var(v), cond(c) {}
    };
    struct PrePost {
        Variable *var;
        int pre, post;
        bool is_conditional_effect;
        vector<EffCond> effect_conds;
        PrePost(Variable *v, int pr, int po) : var(v), pre(pr), post(po),
                is_conditional_effect(false)
        { }
        PrePost(Variable *v, vector<EffCond> ecs, int pr, int po)
          : var(v), pre(pr), post(po), is_conditional_effect(true), effect_conds(ecs)
        { }
    };

private:
    string name;
    vector<Prevail> prevail;    // var, val
    vector<PrePost> pre_post; // var, old-val, new-val
    vector<double> cost;
public:
    Operator(istream &in, const vector<Variable *> &variables);

    void dump() const;
    vector<double> const& get_cost() const {return cost; }
    string get_name() const {return name; }
    const vector<Prevail> &get_prevail() const {return prevail; }
    const vector<PrePost> &get_pre_post() const {return pre_post; }
};

}
#endif
