#ifndef GLOBAL_OPERATOR_H
#define GLOBAL_OPERATOR_H

#include "global_state.h"
#include <iostream>
#include <string>
#include <vector>

struct GlobalCondition {
    int var;
    container_int val;
    explicit GlobalCondition(std::istream &in);
    GlobalCondition(int variable, int value);

    bool is_applicable(const GlobalState &state) const {
        return state[var] == val;
    }

    bool operator==(const GlobalCondition &other) const {
        return var == other.var && val == other.val;
    }

    bool operator!=(const GlobalCondition &other) const {
        return !(*this == other);
    }

    void dump() const;
};

struct GlobalEffect {
    int var;
    container_int val;
    std::vector<GlobalCondition> conditions;
    explicit GlobalEffect(std::istream &in);
    GlobalEffect(int variable, container_int value, const std::vector<GlobalCondition> &conds);

    bool does_fire(const GlobalState &state) const {
        for (size_t i = 0; i < conditions.size(); ++i)
            if (!conditions[i].is_applicable(state))
                return false;
        return true;
    }
    void dump() const;
};

struct AssignEffect {
	int aff_var;
	f_operator fop;
	int ass_var;
	bool is_conditional_effect;
	std::vector<GlobalCondition> conditions;
	AssignEffect(std::istream &in);
	AssignEffect(int v, f_operator fotor, int a) :
		aff_var(v), fop(fotor), ass_var(a) {
		is_conditional_effect = false;
	}
	void dump() const;
};

class GlobalOperator {
    bool is_an_axiom;
    std::vector<GlobalCondition> preconditions;
    std::vector<GlobalEffect> effects;
    std::vector<AssignEffect> assign_effects;
//    std::vector<AssignEffect> instrumentation_effects;
    std::string name;
    ap_float cost;
    mutable bool marked; // Used for short-term marking of preferred operators
    void read_pre_post(std::istream &in);
public:
    explicit GlobalOperator(std::istream &in, bool is_axiom);
    explicit GlobalOperator(PropositionalAxiom convert_from_axiom);
    void dump() const;
    const std::string &get_name() const {return name; }

    bool is_axiom() const {return is_an_axiom; }

    const std::vector<GlobalCondition> &get_preconditions() const {return preconditions; }
    const std::vector<GlobalEffect> &get_effects() const {return effects; }
    const std::vector<AssignEffect> &get_assign_effects() const {return assign_effects; }
  //  const std::vector<AssignEffect> &get_instrumentation_effects() const {return instrumentation_effects; }

    bool is_applicable(const GlobalState &state) const {
        for (size_t i = 0; i < preconditions.size(); ++i)
            if (!preconditions[i].is_applicable(state))
                return false;
        return true;
    }

    bool is_marked() const {
        return marked;
    }
    void mark() const {
        marked = true;
    }
    void unmark() const {
        marked = false;
    }

    ap_float get_cost() const {return cost; }
    void set_cost(ap_float init_cost);
};

#endif
