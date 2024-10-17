#ifndef AXIOMS_H
#define AXIOMS_H

#include "global_state.h"
#include "global_operator.h"

#include <vector>

/**
 * The axiom layers are topological layers build as follows:
 * -1: numeric constants, regular numeric variables, regular propositional/FDR variables are stored in layer -1
 * 0 to k-1: numeric expressions are evaluated by artithmetic axioms
 * k: if numeric constraints are present in the task, the first propositional layer contains the result of comparison axioms
 * k to n: finally, the result of propositional axioms are found in the last layers
 */


class Axiom {
	public:
		int affected_variable; // index of the affected variable, for logic and comparison axioms the index of the logic variables, for assignment axioms the index of a numeric variable
};

class PropositionalAxiom : public Axiom
{
    public:
		int layer;
        std::vector<GlobalCondition> conditions; // var, val
        std::vector<GlobalEffect> effects; // this vector will only have one element (if initialized) nevertheless it is more convenient to use a vector to better reuse code that treats operators and axioms alike

        PropositionalAxiom(std::istream &in);
        void dump() const;

        const std::vector<GlobalCondition> &get_preconditions() const {return conditions; }
        const std::vector<GlobalEffect> &get_effects() const {return effects; }

        bool is_applicable(const GlobalState &state) const
        {
            for(size_t i = 0; i < conditions.size(); ++i)
                if(!conditions[i].is_applicable(state))
                    return false;
            return true;
        }
};


class ComparisonAxiom : public Axiom
{
    public:
        int var_lhs;
        int var_rhs;
        comp_operator op;

        ComparisonAxiom(std::istream &in);

        void dump() const;
};

class AssignmentAxiom : public Axiom
{
    public:
        int var_lhs; // index of numeric left hand side variable
        int var_rhs; // index of numeric right hand side variable
        cal_operator op;

        AssignmentAxiom(std::istream &in);

        void dump() const;
};


class AxiomEvaluator {
    struct AxiomRule;
    struct AxiomLiteral {
        std::vector<AxiomRule *> condition_of;
    };
    struct AxiomRule {
        int condition_count;
        int unsatisfied_conditions;
        int effect_var;
        container_int effect_val;
        AxiomLiteral *effect_literal;
        AxiomRule(int cond_count, int eff_var, int eff_val, AxiomLiteral *eff_literal)
            : condition_count(cond_count), unsatisfied_conditions(cond_count),
              effect_var(eff_var), effect_val(eff_val), effect_literal(eff_literal) {
        }
    };
    struct NegationByFailureInfo {
        int var_no;
        AxiomLiteral *literal;
        NegationByFailureInfo(int var, AxiomLiteral *lit)
            : var_no(var), literal(lit) {}
    };

    std::vector<std::vector<AxiomLiteral>> axiom_literals;
    std::vector<AxiomRule> rules;
    std::vector<std::vector<NegationByFailureInfo>> nbf_info_by_layer;

    // The queue is an instance variable rather than a local variable
    // to reduce reallocation effort. See issue420.
    std::vector<AxiomLiteral *> queue;
private:
    void evaluate_comparison_axioms(PackedStateBin *buffer, std::vector<ap_float> &numeric_state);
    void evaluate_logic_axioms(PackedStateBin *buffer);
public:
    AxiomEvaluator();
    void evaluate(PackedStateBin *buffer, std::vector<ap_float> &numeric_state);
    void evaluate_arithmetic_axioms(std::vector<ap_float> &numeric_state);
};

#endif
