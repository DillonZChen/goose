#include "helper_functions.h"
#include "operator.h"
#include "variable.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

Operator::Operator(istream &in, const vector<Variable *> &variables, const vector<NumericVariable *> &numeric_variables) {
    check_magic(in, "begin_operator");
//    cout << "parsing operator ";
    in >> ws;
    getline(in, name);
//    cout << name << endl;
    int count; // number of prevail conditions
    in >> count;
    for (int i = 0; i < count; i++) {
        int varNo, val;
        in >> varNo >> val;
        prevail.push_back(Prevail(variables[varNo], val));
    }
    in >> count; // number of pre_post conditions
    for (int i = 0; i < count; i++) {
        int eff_conds;
        vector<EffCond> ecs;
        in >> eff_conds;
        for (int j = 0; j < eff_conds; j++) {
            int var, value;
            in >> var >> value;
            ecs.push_back(EffCond(variables[var], value));
        }
        int varNo, val, newVal;
        in >> varNo >> val >> newVal;
        if (eff_conds)
            pre_post.push_back(PrePost(variables[varNo], ecs, val, newVal));
        else
            pre_post.push_back(PrePost(variables[varNo], val, newVal));
    }
    in >> count; // number of assignment effects
//    cout << count << " assign effects" << endl;
    for (int i = 0; i < count; i++) {
        int eff_conds;
        vector<EffCond> ecs;
        in >> eff_conds;
        for (int j = 0; j < eff_conds; j++) {
            int var, value;
            in >> var >> value;
            ecs.push_back(EffCond(variables[var], value));
        }
    	int af_var, ex_var;
    	foperator operato;
    	in >> af_var >> operato >> ex_var >> ws;
//    	cout << "Operator " << name << " has assignment effect on " << af_var << ". Operator >" << operato << "< examined variable : " << ex_var << endl;
    	assign_effects.push_back(NumericEffect(numeric_variables[af_var], operato, numeric_variables[ex_var]));
    }
    double oldcost;
    in >> oldcost >> ws;
    cost = oldcost;
//    string costStr;
//   	getline(in, costStr);
//    cout << "'" << costStr << "' ist der kostenstring";
//    deprecated_cost = atof(costStr.c_str());
//    cout << "Deprecated cost is '" << deprecated_cost << "'" << endl;
    check_magic(in, "end_operator");
    // TODO: Evtl. effektiver: conditions schon sortiert einlesen?
}

void Operator::dump() const {
    cout << name << ":" << endl;
    cout << "prevail:";
    for (const auto &prev : prevail)
        cout << "  " << prev.var->get_name() << " := " << prev.prev;
    cout << endl;
    cout << "pre-post:";
    for (const auto &eff : pre_post) {
        if (eff.is_conditional_effect) {
            cout << "  if (";
            for (const auto &cond : eff.effect_conds)
                cout << cond.var->get_name() << " := " << cond.cond;
            cout << ") then";
        }
        cout << " " << eff.var->get_name() << " : "
             << eff.pre << " -> " << eff.post;
    }
    cout << endl;
}

int Operator::get_encoding_size() const {
    int size = 1 + prevail.size();
    for (const auto &eff : pre_post) {
        size += 1 + eff.effect_conds.size();
        if (eff.pre != -1)
            size += 1;
    }
    return size;
}

void Operator::strip_unimportant_effects() {
    int new_index = 0;
    for (const auto &eff : pre_post) {
        if (eff.var->get_level() != -1)
            pre_post[new_index++] = eff;
    }
    pre_post.erase(pre_post.begin() + new_index, pre_post.end());
    // strip numeric effects
    new_index = 0;
    for (const auto &ass_eff : assign_effects) {
    	if (ass_eff.var->get_level() != -1)
    		assign_effects[new_index++] = ass_eff;
    }
    assign_effects.erase(assign_effects.begin() + new_index, assign_effects.end());
}

bool Operator::is_redundant() const {
	// an Operator is redundant if it has no propositional effects (pre_post is empty)
	// AND if all of its assign effects are instrumentation effects.
	if (pre_post.empty()) {
		for (auto asseff : assign_effects)
			if (asseff.var->get_type() == regular) {
				if (DEBUG) cout << "Operator " << name << " is not redundant because of effect on " << asseff.var->get_name() << endl;
				return false;
			}
		if (DEBUG) cout << "Operator " << name << " is redundant" << endl;
		return true; // operator has no relevant numeric effects either
	} else
		return false; // operator has propositional effects and is therefore NOT redundant
}

void strip_operators(vector<Operator> &operators) {
    int old_count = operators.size();
    int new_index = 0;
    for (Operator &op : operators) {
//    	cout << "striptease for " << op.get_name() << endl;
//    	op.dump();
        op.strip_unimportant_effects();
//    	op.dump();
        if (!op.is_redundant())
            operators[new_index++] = op;
    }
    operators.erase(operators.begin() + new_index, operators.end());
    cout << operators.size() << " of " << old_count << " operators necessary." << endl;
}

void Operator::generate_cpp_input(ofstream &outfile) const {
    //TODO: beim Einlesen in search feststellen, ob leerer Operator
//	cout << "generating cpp input for operator " << name << endl;
//	dump();
    outfile << "begin_operator" << endl;
    outfile << name << endl;

    outfile << prevail.size() << endl;
//    cout << "# prevail " << prevail.size() << endl;
    for (const auto &prev : prevail) {
        assert(prev.var->get_level() != -1);
        if (prev.var->get_level() != -1)
            outfile << prev.var->get_level() << " " << prev.prev << endl;
    }
//    cout << "# prepost " << pre_post.size() << endl;
    outfile << pre_post.size() << endl;
    for (const auto &eff : pre_post) {
        assert(eff.var->get_level() != -1);
        outfile << eff.effect_conds.size();
        for (const auto &cond : eff.effect_conds) {
            outfile << " " << cond.var->get_level()
                    << " " << cond.cond;
        }
        outfile << " " << eff.var->get_level()
                << " " << eff.pre
                << " " << eff.post << endl;
    }
//    cout << "# asseff " << assign_effects.size() << endl;
    outfile << assign_effects.size() << endl;
    for (const auto &neff : assign_effects) {
    	outfile << neff.effect_conds.size();
    	for (EffCond cond : neff.effect_conds) {
    	            outfile << " " << cond.var->get_level()
    	                    << " " << cond.cond;
    	}
    	outfile << " " << neff.var->get_level()
    			<< " " << neff.fop
    			<< " " << neff.foperand->get_level() << endl;
    }
    outfile << cost << endl;
    outfile << "end_operator" << endl;
}
