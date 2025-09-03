#include "axioms.h"
#include "global_operator.h"
#include "globals.h"
#include "int_packer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

PropositionalAxiom::PropositionalAxiom(istream &in) : layer(-1){
    check_magic(in, "begin_rule");
    int cond_count;
    in >> cond_count;
    for(int i = 0; i < cond_count; i++) {
        conditions.push_back(GlobalCondition(in));
    }
    int old_value, new_value;
    in >> affected_variable >> old_value >> new_value;
    effects.push_back(GlobalEffect(affected_variable, new_value, vector<GlobalCondition>()));
    check_magic(in, "end_rule");
}

void PropositionalAxiom::dump() const
    {
        for (size_t i = 0; i < conditions.size(); ++i) {
        	cout << "[";
            conditions[i].dump();
            cout << "]";
        }
//        cout << "[" << g_variable_name[affected_variable] << ": "
//            << old_value << "]";
        cout << " => " << g_variable_name[affected_variable] << ": "
            << effects[0].val << endl;
}

ComparisonAxiom::ComparisonAxiom(istream &in) {
	int av, vl, vr;
	comp_operator co;
	in >> av >> co >> vl >> vr;
	affected_variable = av;
	op = co;
	var_lhs = vl;
	var_rhs = vr;
}

AssignmentAxiom::AssignmentAxiom(istream &in) {
	int av, vl, vr;
	cal_operator fo;
	in >> av >> fo >> vl >> vr;
	affected_variable = av;
	op = fo;
	var_lhs = vl;
	var_rhs = vr;
//	if (DEBUG) cout << "Read ass-ax " << av << " :=  " << vl << " " << op << " " << vr << endl;
	assert(av < (int) g_numeric_var_names.size());
	assert(vl < (int) g_numeric_var_names.size());
	assert(vr < (int) g_numeric_var_names.size());
}

void ComparisonAxiom::dump() const {
	cout << g_variable_name[affected_variable] << " = ("
			<< g_variable_name[var_lhs];
    cout << " " << op << " ";
    cout << g_variable_name[var_rhs] << ")" << endl;
}


void AssignmentAxiom::dump() const {
    cout << g_numeric_var_names[affected_variable] << " = ("
        << g_numeric_var_names[var_lhs];
    cout << " " << op << " ";
    cout << g_numeric_var_names[var_rhs] << ")" << endl;
}


AxiomEvaluator::AxiomEvaluator() {
    // Handle axioms in the following order:
    // 1) Arithmetic axioms (layers 0 through k-1)
    // 2) Comparison axioms (layer k)
    // 3) Propositional Logic axioms (layers k+1 through n)

	// Numeric Constants and FDR-Variables are stored in layer -1

    // determine layer where arithmetic axioms end and comparison
    // axioms and logic axioms start

    g_comparison_axiom_layer = -1;
    g_first_logic_axiom_layer = -1;
    g_last_logic_axiom_layer = -1;

    for(size_t i = 0; i < g_axiom_layers.size(); ++i) {
        int layer = g_axiom_layers[i];
        if(layer == -1) // regular variable
            continue;
        g_last_logic_axiom_layer = max(g_last_logic_axiom_layer, layer);
        if(layer < g_first_logic_axiom_layer || g_first_logic_axiom_layer == -1) {
        	assert(layer >= 0);
        	g_first_logic_axiom_layer = layer;
        }
    }
//    if(DEBUG) cout << "comparison axioms in task: " << g_comp_axioms.size() << endl;
    if (g_first_logic_axiom_layer >= 0 && g_comp_axioms.size() > 0) {
    	g_comparison_axiom_layer = g_first_logic_axiom_layer++; // the first propositional layer is occupied by comparison axioms
//    	if(DEBUG) cout << "comparison layer = " << g_comparison_axiom_layer << " last arithmetic  = " << g_last_arithmetic_axiom_layer << endl;
    	assert(g_comparison_axiom_layer == g_last_arithmetic_axiom_layer + 1);
    }
//    if (DEBUG) {
//    	cout << "last arithmetic axiom layer = " << g_last_arithmetic_axiom_layer << endl;
//    	cout << "comparison axiom layer = " << g_comparison_axiom_layer << endl;
//    	cout << "first logic axiom layer = " << g_first_logic_axiom_layer <<endl;
//    	cout << "last logic axiom layer = " << g_last_logic_axiom_layer <<endl;
//    }

    // Initialize literals
    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        axiom_literals.push_back(vector<AxiomLiteral>(g_variable_domain[i]));

//    // Initialize rules
//    for (size_t i = 0; i < g_axioms.size(); ++i) {
//        const GlobalOperator &axiom = g_axioms[i];
//        int cond_count = axiom.get_effects()[0].conditions.size();
//        int eff_var = axiom.get_effects()[0].var;
//        int eff_val = axiom.get_effects()[0].val;
//        AxiomLiteral *eff_literal = &axiom_literals[eff_var][eff_val];
//        rules.push_back(AxiomRule(cond_count, eff_var, eff_val, eff_literal));
//    }
//


    // Initialize rules
    for (const auto & axiom : g_logic_axioms) {
        // using the field "precondition" in NFD instead of conditional effects in regular FastDownward
    	int cond_count = axiom.get_preconditions().size();
        int eff_var = axiom.affected_variable;
        int eff_val = axiom.get_effects()[0].val;
    	AxiomLiteral *eff_literal = &axiom_literals[eff_var][eff_val];
    	if(DEBUG) cout << "Creating Axiom Rule for Literal ["<<eff_var<<"]["<<eff_val <<"] with " << cond_count << " conditions" <<endl;
    	rules.push_back(AxiomRule(cond_count, eff_var, eff_val, eff_literal));

    }

    // Cross-reference rules and literals
    for (size_t i = 0; i < g_logic_axioms.size(); ++i) {
        // using the field "precondition" in NFD instead of conditional effects in regular FastDownward
    	const vector<GlobalCondition> &conditions = g_logic_axioms[i].get_preconditions();
        for (size_t j = 0; j < conditions.size(); ++j) {
            const GlobalCondition &cond = conditions[j];
            axiom_literals[cond.var][cond.val].condition_of.push_back(&rules[i]);
    	}
    }

    // Initialize negation-by-failure information
    int last_layer = -1;
    for (size_t i = 0; i < g_axiom_layers.size(); ++i)
        last_layer = max(last_layer, g_axiom_layers[i]);
    nbf_info_by_layer.resize(last_layer + 1);

    for (size_t var_no = 0; var_no < g_axiom_layers.size(); ++var_no) {
        int layer = g_axiom_layers[var_no];
        if (layer != -1 && layer != last_layer) {
            int nbf_value = g_default_axiom_values[var_no];
            AxiomLiteral *nbf_literal = &axiom_literals[var_no][nbf_value];
            NegationByFailureInfo nbf_info(var_no, nbf_literal);
            nbf_info_by_layer[layer].push_back(nbf_info);
        }
    }
}

void AxiomEvaluator::evaluate(PackedStateBin *buffer, std::vector<ap_float> &numeric_state) {
    if (!has_axioms()) {
    	if (DEBUG) cout << "Task has no axioms -> return" << endl;
        return;
    }
    // numeric  assignment axioms are already evaluated on the fly
    // but comparison axioms have to be evaluated now
    if (has_numeric_axioms()) {
    	evaluate_comparison_axioms(buffer, numeric_state);
    }
    if (has_logic_axioms()) {
//		if (DEBUG) cout << "AxEv evaluating logic axioms ..." << endl;
    	evaluate_logic_axioms(buffer);
    }
}

void AxiomEvaluator::evaluate_arithmetic_axioms(std::vector<ap_float> &numeric_state)
{
//	int current_layer = -1;
	for (const auto & ax : g_ass_axioms) {
		assert(g_numeric_var_names.size() == numeric_state.size());
//		int next_layer = g_numeric_axiom_layers[ax.affected_variable];
//		assert(next_layer >= current_layer);
//		current_layer = next_layer;
		assert(ax.affected_variable < (int) numeric_state.size());
		assert(ax.var_lhs < (int) numeric_state.size());
		assert(ax.var_rhs < (int) numeric_state.size());
		ap_float left = numeric_state[ax.var_lhs];
		ap_float right = numeric_state[ax.var_rhs];
		ap_float result;
//		if(DEBUG) cout << "evaluating numeric assignment axiom: ";
//		if(DEBUG) cout << g_numeric_var_names[ax.affected_variable] << " := "
//				<< g_numeric_var_names[ax.var_lhs] << " " << ax.op << " " << g_numeric_var_names[ax.var_rhs] << endl;
		switch(ax.op) {
		case sum:
			result = left + right;
			numeric_state[ax.affected_variable] = result;
			break;
		case diff:
			result = left - right;
			numeric_state[ax.affected_variable] = result;
			break;
		case mult:
			result = left * right;
			numeric_state[ax.affected_variable] = result;
			break;
		case divi:
			result = left / right;
			numeric_state[ax.affected_variable] = result;
			break;
		default:
			cout << "Error: No assignment operators are allowed here." << endl;
			assert(false);
			break;
		}
	}
}

void AxiomEvaluator::evaluate_comparison_axioms(PackedStateBin *buffer, std::vector<ap_float> &numeric_state)
{
	for(const auto & ax : g_comp_axioms) {
		int var = ax.affected_variable; // logic variable
		assert(ax.var_lhs < (int) numeric_state.size());
		assert(ax.var_rhs < (int) numeric_state.size());
		ap_float left = numeric_state[ax.var_lhs];
		ap_float right = numeric_state[ax.var_rhs];
        bool result;
//        cout << "evaluating comparison axiom: " << endl;
//if(DEBUG) cout << g_variable_name[ax->affected_variable] << " := " << g_numeric_var_names[ax->var_lhs] << " " << ax->op << " " <<g_numeric_var_names[ax->var_rhs];
        switch(ax.op) {
        case lt:
        	result = left < right;
        	break;
        case le:
        	result = left <= right;
        	break;
        case eq:
        	result = left == right;
        	break;
        case ge:
        	result = left >= right;
        	break;
        case gt:
        	result = left > right;
        	break;
        case ue:
        	result = left != right;
        	break;
        default:
        	cout << "Error: No comparison operators are allowed here." << endl;
        	assert(false);
        	result = false;
        	break;
        }
//        cout << " => var "<< var << " = " << (result?0:1) << endl;
    	g_state_packer->set(buffer,var,(result?0:1));
	}
}


// TODO rethink the way this is called: see issue348.
void AxiomEvaluator::evaluate_logic_axioms(PackedStateBin *buffer) {
    if (!has_logic_axioms())
        return;
    assert(queue.empty());
    for (size_t i = 0; i < g_axiom_layers.size(); ++i) { // g_axiom_layers[i] is the layer of the i-th variable

        if (g_axiom_layers[i] == -1) {
        	// non-derived variable -> push_back immediately
        	queue.push_back(&axiom_literals[i][g_state_packer->get(buffer, i)]);
        } else if (g_axiom_layers[i] <= g_last_arithmetic_axiom_layer) {
        	// derived arithmetic variable
        	cerr << "Encountered logic variable in an axiom layer reserved for numeric variables" << endl;
        	cerr << "Variable #" <<i<< " is in layer " << g_axiom_layers[i] << " while numeric layers got up to layer " << g_last_arithmetic_axiom_layer << endl;
        	dump_variable(i);
        	assert(false);
        } else if (g_axiom_layers[i] == g_comparison_axiom_layer) {
        	// variable is the result of a comparison axiom
//        	if(DEBUG) cout << "Handling comparison axiom with value " << g_state_packer->get(buffer, i)<< endl ;
        	queue.push_back(&axiom_literals[i][g_state_packer->get(buffer, i)]);
        } else if (g_axiom_layers[i] <= g_last_logic_axiom_layer) {
        	assert(g_axiom_layers[i] > g_comparison_axiom_layer);
//        	if(DEBUG) cout << "Logic axiom is in layer" << g_axiom_layers[i] << endl ;
//        	if(DEBUG) cout << "Set Axiom to value " << g_default_axiom_values[i] << endl;
            g_state_packer->set(buffer, i, g_default_axiom_values[i]);
        } else {
            // cannot happen
            cout << "Error: Encountered a variable with an axiom layer exceeding "
                << "the maximal computed axiom layer." << endl;
            exit(1);
        }
    }

    for (size_t i = 0; i < rules.size(); ++i) {
        rules[i].unsatisfied_conditions = rules[i].condition_count;

        // TODO: In a perfect world, trivial axioms would have been
        // compiled away, and we could have the following assertion
        // instead of the following block.
        // assert(rules[i].condition_counter != 0);
        if (rules[i].condition_count == 0) {
            // NOTE: This duplicates code from the main loop below.
            // I don't mind because this is (hopefully!) going away
            // some time.
        	if(DEBUG) cout << "Axiom Rule " << i << " triggers " << endl;
            int var_no = rules[i].effect_var;
            container_int val = rules[i].effect_val;
            if (g_state_packer->get(buffer, var_no) != val) {
            	if (DEBUG) cout << "axiom changes " << var_no << " from " << g_state_packer->get(buffer, var_no) << " to " << val << endl;
                g_state_packer->set(buffer, var_no, val);
                queue.push_back(rules[i].effect_literal);
            }
        }
    }

    for (size_t layer_no = 0; layer_no < nbf_info_by_layer.size(); ++layer_no) {
        // Apply Horn rules.
        while (!queue.empty()) {
            AxiomLiteral *curr_literal = queue.back();
            queue.pop_back();
            for (size_t i = 0; i < curr_literal->condition_of.size(); ++i) {
                AxiomRule *rule = curr_literal->condition_of[i];
//                if(DEBUG) cout << "Axiom Eval Queue popped literal which is a condition of [" << rule->effect_var << "][" << rule->effect_val << "]" << endl;
                if (--rule->unsatisfied_conditions == 0) {
                    int var_no = rule->effect_var;
                    container_int val = rule->effect_val;
                    if (g_state_packer->get(buffer, var_no) != val) {
//                    	if (DEBUG) cout << "axiom precons satisfied, it changes " << var_no << " from " << g_state_packer->get(buffer, var_no) << " to " << val << endl;
                        g_state_packer->set(buffer, var_no, val);
                        queue.push_back(rule->effect_literal);
                    }
                }
            }
        }

        // Apply negation by failure rules. Skip this in last iteration
        // to save some time (see issue420, msg3058).
        if (layer_no != nbf_info_by_layer.size() - 1) {
            const vector<NegationByFailureInfo> &nbf_info = nbf_info_by_layer[layer_no];
            for (size_t i = 0; i < nbf_info.size(); ++i) {
                int var_no = nbf_info[i].var_no;
                if (g_state_packer->get(buffer, var_no) == g_default_axiom_values[var_no])
                    queue.push_back(nbf_info[i].literal);
            }
        }
    }
}
