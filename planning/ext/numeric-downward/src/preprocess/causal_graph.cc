/* Computes the levels of the variables and returns an ordering of the variables
 * with respect to their levels, beginning with the lowest-level variable.
 *
 * The new order does not contain any variables that are unimportant for the
 * problem, i.e. variables that don't occur in the goal and don't influence
 * important variables (== there is no path in the causal graph from this variable
 * through an important variable to the goal).
 *
 * The constructor takes variables and operators and constructs a graph such that
 * there is a weighted edge from a variable A to a variable B with weight n+m if
 * there are n operators that have A as prevail condition or postcondition or
 * effect condition for B and that have B as postcondition, and if there are
 * m axioms that have A as condition and B as effect.
 * This graph is used for calculating strongly connected components (using "scc")
 * wich are returned in order (lowest-level component first).
 * In each scc with more than one variable cycles are then eliminated by
 * succesively taking out edges with minimal weight (in max_dag), returning a
 * variable order.
 * Last, unimportant variables are identified and eliminated from the new order,
 * which is returned when "get_variable_ordering" is called.
 * Note: The causal graph will still contain the unimportant vars, but they are
 * suppressed in the input for the search programm.
 */

#include "causal_graph.h"
#include "max_dag.h"
#include "operator.h"
#include "axiom.h"
#include "scc.h"
#include "variable.h"

#include <iostream>
#include <cassert>
using namespace std;

bool g_do_not_prune_variables = false;

void CausalGraph::weigh_graph_from_ops(const vector<Variable *> &,
									   const vector<NumericVariable *> &,
                                       const vector<Operator> &operators,
                                       const vector<pair<Variable *, int> > &) {
    for (const Operator &op : operators) {
        const vector<Operator::Prevail> &prevail = op.get_prevail();
        const vector<Operator::PrePost> &pre_posts = op.get_pre_post();
        const vector<Operator::NumericEffect> &ass_effs = op.get_num_eff();
        vector<CGVar> source_vars;
        for (const Operator::Prevail &prev : prevail)
            source_vars.push_back(CGVar(prev.var));
        for (const Operator::PrePost &pre_post : pre_posts)
            if (pre_post.pre != -1)
                source_vars.push_back(CGVar(pre_post.var));
        // propositional variables do not depend on the values of numeric variables that appear in operator effects

        for (const Operator::PrePost &pre_post : pre_posts) {
            CGVar curr_target = CGVar(pre_post.var);
            if (pre_post.is_conditional_effect)
                for (const Operator::EffCond &eff_cond : pre_post.effect_conds)
                    source_vars.push_back(CGVar(eff_cond.var));

            for (CGVar curr_source : source_vars) {
                WeightedSuccessors &weighted_succ = weighted_graph[curr_source];

                if (predecessor_graph.count(curr_target) == 0) {
                    predecessor_graph[curr_target] = Predecessors();
                }
                if (curr_source != curr_target) {
                    if (weighted_succ.find(curr_target) != weighted_succ.end()) {
                        weighted_succ[curr_target]++;
                        predecessor_graph[curr_target][curr_source]++;
                    } else {
                        weighted_succ[curr_target] = 1;
                        predecessor_graph[curr_target][curr_source] = 1;
                    }
//                	if(DEBUG) cout << "Op " << op.get_name() << " induces edge from " << curr_source.get_name() << " to " << curr_target.get_name();
//        			if(DEBUG) cout << " weight " << weighted_succ[curr_target] << " = " << predecessor_graph[curr_target][curr_source] << endl;
                }
            }

            if (pre_post.is_conditional_effect)
                source_vars.erase(source_vars.end() - pre_post.effect_conds.size(),
                                  source_vars.end());
        } // for all pre_post effects
        for (const Operator::NumericEffect &ass_eff : ass_effs) {
        	CGVar curr_target = CGVar(ass_eff.var);
            if (ass_eff.is_conditional_effect)
                for (const Operator::EffCond &eff_cond : ass_eff.effect_conds)
                    source_vars.push_back(CGVar(eff_cond.var));
            source_vars.push_back(CGVar(ass_eff.foperand)); // a numeric variable depends on its numeric assignment
            for (CGVar curr_source : source_vars){
            	WeightedSuccessors &weighted_succ = weighted_graph[curr_source];
            	if (predecessor_graph.count(curr_target) == 0) {
            		predecessor_graph[curr_target] = Predecessors();
            	}
            	if (curr_source != curr_target) {
            		if (weighted_succ.find(curr_target) != weighted_succ.end()) {
            			assert(predecessor_graph[curr_target][curr_source] == weighted_succ[curr_target]);
            			weighted_succ[curr_target]++;
            			predecessor_graph[curr_target][curr_source]++;
            		} else {
            			weighted_succ[curr_target] = 1;
            			predecessor_graph[curr_target][curr_source] = 1;
            		}
//            		if(DEBUG) cout << "NOp " << op.get_name() << " induces edge from " << curr_source.get_name() << " to "
//            				<< curr_target.get_name() << " weight " << weighted_succ[curr_target] << " = "
//            				<< predecessor_graph[curr_target][curr_source] << endl;
            	}
            }
            source_vars.erase(source_vars.end() - ass_eff.effect_conds.size() - 1,
            		source_vars.end()); // at least one variable was added to source vars: the assignment
        }
    }
}

void CausalGraph::weigh_graph_from_axioms(const vector<Variable *> &,
                                          const vector<Axiom_relational> &axioms,
                                          const vector<pair<Variable *, int> > &) {
    for (const Axiom_relational &axiom : axioms) {
        const vector<Axiom_relational::Condition> &conds = axiom.get_conditions();
        vector<CGVar> source_vars;
        for (const Axiom_relational::Condition &cond : conds)
            source_vars.push_back(cond.var);

        for (CGVar curr_source : source_vars) {
            WeightedSuccessors &weighted_succ = weighted_graph[curr_source];
            // only one target var: the effect var of axiom
            CGVar curr_target = CGVar(axiom.get_effect_var());
            if (predecessor_graph.count(curr_target) == 0)
                predecessor_graph[curr_target] = Predecessors();
            if (curr_source != curr_target) {
                if (weighted_succ.find(curr_target) != weighted_succ.end()) {
                    weighted_succ[curr_target]++;
                    predecessor_graph[curr_target][curr_source]++;
                } else {
                    weighted_succ[curr_target] = 1;
                    predecessor_graph[curr_target][curr_source] = 1;
                }
//            	if(DEBUG) cout << "PrAx induces edge from " << curr_source.get_name() << " to " << curr_target.get_name();
//    			if(DEBUG) cout << " weight " << weighted_succ[curr_target] << " = " << predecessor_graph[curr_target][curr_source] << endl;
            }
        }
    }
}

void CausalGraph::weigh_graph_from_comp_axioms(const vector<Axiom_functional_comparison> &comp_axioms) {
	for (const Axiom_functional_comparison &cax : comp_axioms) {
		vector<CGVar> source_vars;
		source_vars.push_back(CGVar(cax.get_left_var()));
		source_vars.push_back(CGVar(cax.get_right_var()));
		CGVar target = CGVar(cax.get_effect_var());

		for (CGVar curr_source : source_vars) {
			WeightedSuccessors &weighted_succ = weighted_graph[curr_source];
			if (predecessor_graph.count(target) == 0)
				predecessor_graph[target] = Predecessors();
			if (weighted_succ.find(target) != weighted_succ.end()) {
				weighted_succ[target]++;
				predecessor_graph[target][curr_source]++;
			} else {
				weighted_succ[target] = 1;
				predecessor_graph[target][curr_source] = 1;
			}
//			if(DEBUG) cout << "CAX induces edge from " << curr_source.get_name() << " to " << target.get_name();
//			if(DEBUG) cout << " weight " << weighted_succ[target] << " = " << predecessor_graph[target][curr_source] << endl;
		}
	}
}

void CausalGraph::weigh_graph_from_ass_axioms(const vector<Axiom_numeric_computation> &ass_axioms) {
	for (const Axiom_numeric_computation &ass_ax : ass_axioms) {
		vector<CGVar> source_vars;
		source_vars.push_back(CGVar(ass_ax.get_left_var()));
		source_vars.push_back(CGVar(ass_ax.get_right_var()));
		CGVar target = CGVar(ass_ax.get_effect_var());

		for (CGVar curr_source : source_vars) {
			WeightedSuccessors &weighted_succ = weighted_graph[curr_source];
			if (predecessor_graph.count(target) == 0)
				predecessor_graph[target] = Predecessors();
			if (curr_source != target) {
				if (weighted_succ.find(target) != weighted_succ.end()) {
					weighted_succ[target]++;
					predecessor_graph[target][curr_source]++;
				} else {
					weighted_succ[target] = 1;
					predecessor_graph[target][curr_source] = 1;
				}
//            	if(DEBUG) cout << "AssAx induces edge from " << curr_source.get_name() << " to " << target.get_name();
//    			if(DEBUG) cout << " weight " << weighted_succ[target] << " = " << predecessor_graph[target][curr_source] << endl;
			}
		}
	}
}

CausalGraph::CausalGraph(const vector<Variable *> &the_variables,
						const vector<NumericVariable *> &the_numeric_variables,
		                const vector<Operator> &the_operators,
		                const vector<Axiom_relational> &the_axioms,
		                const vector<Axiom_numeric_computation> &the_ass_axioms,
		                const vector<Axiom_functional_comparison> &the_comp_axioms,
		                const vector<pair<Variable *, int> > &the_goals,
						const GlobalConstraint &the_global_constraint,
		                NumericVariable * the_metric_var)
    : variables(the_variables), numeric_variables(the_numeric_variables), operators(the_operators),
      axioms(the_axioms), ass_axioms(the_ass_axioms), comp_axioms(the_comp_axioms), goals(the_goals),
	  global_constraint(the_global_constraint), metric_var(the_metric_var), acyclic(false) {

    for (Variable *var : variables)
        weighted_graph[CGVar(var)] = WeightedSuccessors();
    for (NumericVariable *nvar : numeric_variables)
    	weighted_graph[CGVar(nvar)] = WeightedSuccessors();
    weigh_graph_from_ops(variables, numeric_variables, operators, goals);
    weigh_graph_from_axioms(variables, axioms, goals);
    weigh_graph_from_comp_axioms(comp_axioms);
    weigh_graph_from_ass_axioms(ass_axioms);
    //dump();

    Partition sccs;
    get_strongly_connected_components(sccs);

    cout << "The causal graph is "
         << (sccs.size() == (variables.size()+numeric_variables.size()) ? "" : "not ")
         << "acyclic." << endl;
    /*
    if (sccs.size() != variables.size()) {
      cout << "Components: " << endl;
      for(int i = 0; i < sccs.size(); i++) {
        for(int j = 0; j < sccs[i].size(); j++)
          cout << " " << sccs[i][j]->get_name();
        cout << endl;
      }
    }
    */
    calculate_topological_pseudo_sort(sccs);
    calculate_important_vars();

    // cout << "new variable order: ";
    // for(int i = 0; i < ordering.size(); i++)
    //   cout << ordering[i]->get_name()<<" - ";
    // cout << endl;
}

void CausalGraph::calculate_topological_pseudo_sort(const Partition &sccs) {
    map<Variable *, int> goal_map;
    for (const auto &goal : goals)
        goal_map[goal.first] = goal.second;
    for (const vector<CGVar> &curr_scc : sccs) {
        int num_scc_vars = curr_scc.size();
        if (num_scc_vars > 1) {
            // component needs to be turned into acyclic subgraph

            // Map variables to indices in the strongly connected component.
            map<CGVar, int> variableToIndex;
            for (int i = 0; i < num_scc_vars; i++)
                variableToIndex[curr_scc[i]] = i;

            // Compute subgraph induced by curr_scc and convert the successor
            // representation from a map to a vector.
            vector<vector<pair<int, int>>> subgraph;
            for (int i = 0; i < num_scc_vars; i++) {
                // For each variable in component only list edges inside component.
                WeightedSuccessors &all_edges = weighted_graph[curr_scc[i]];
                vector<pair<int, int>> subgraph_edges;
                for (WeightedSuccessors::const_iterator curr = all_edges.begin();
                     curr != all_edges.end(); ++curr) {
                    CGVar target = curr->first;
                    int cost = curr->second;
                    map<CGVar, int>::iterator index_it = variableToIndex.find(target);
                    if (index_it != variableToIndex.end()) {
                        int new_index = index_it->second;
                        if (goal_map.find(target.var) != goal_map.end()) {
                            //TODO: soll das so bleiben? (Zahl taucht in max_dag auf)
                            // target is goal
                            subgraph_edges.push_back(make_pair(new_index, 100000 + cost));
                        }
                        subgraph_edges.push_back(make_pair(new_index, cost));
                    }
                }
                subgraph.push_back(subgraph_edges);
            }

            vector<int> order = MaxDAG(subgraph).get_result();
            for (int i : order) {
                ordering.push_back(curr_scc[i]);
            }
        } else {
            ordering.push_back(curr_scc[0]);
        }
    }
}

void CausalGraph::get_strongly_connected_components(Partition &result) {
    map<CGVar, int> variableToIndex;
    size_t num_vars = variables.size();
    for (size_t i = 0; i < num_vars; i++)
        variableToIndex[variables[i]] = i;
    size_t num_numeric_vars = numeric_variables.size();
    for (size_t i = 0; i < num_numeric_vars; i++)
        variableToIndex[numeric_variables[i]] = num_vars + i;

    vector<vector<int> > unweighted_graph;
    unweighted_graph.resize(num_vars + num_numeric_vars);
    for (const auto &weighted_node : weighted_graph) {
        int index = variableToIndex[weighted_node.first];
        vector<int> &succ = unweighted_graph[index];
        const WeightedSuccessors &weighted_succ = weighted_node.second;
        for (const auto &weighted_succ_node : weighted_succ)
            succ.push_back(variableToIndex[weighted_succ_node.first]);
    }

    vector<vector<int> > int_result = SCC(unweighted_graph).get_result();

    result.clear();
    for (const auto &int_component : int_result) {
        vector<CGVar> component;
        for (int var_id : int_component) {
        	if (var_id < (int) num_vars) {
        		assert(var_id >= 0);
        		component.push_back(CGVar(variables[var_id]));
        	} else {
        		assert((var_id- (int) num_vars) < (int) num_numeric_vars);
        		component.push_back(CGVar(numeric_variables[var_id-num_vars]));
        	}
        }
        result.push_back(component);
    }
}
void CausalGraph::calculate_important_vars() {
    // goals are necessary
	for (const auto &goal : goals) {
        if (!goal.first->is_necessary()) {
            if (DEBUG) cout << "var " << goal.first->get_name() <<" is directly neccessary (goal)." << endl;
            goal.first->set_necessary();
            dfs(CGVar(goal.first)); // recursively set all incoming variables necessary
        }
    }
	if (!global_constraint.var->is_necessary()) {
        if (DEBUG) cout << "var " << global_constraint.var->get_name() <<" is directly neccessary (global constraint)." << endl;
//        if (DEBUG) {
//        	cout << "Predecessors ("<<predecessor_graph[CGVar(global_constraint.var)].size()<<") of GC are: " << endl;
//        	for (const auto &pred : predecessor_graph[CGVar(global_constraint.var)]) {
//        		cout << " - " << pred.first.get_name() << endl;
//        		pred.first.var->dump();
//        	}
//        }
        global_constraint.var->set_necessary();
        dfs(CGVar(global_constraint.var)); // recursively set all incoming variables necessary
	}

	// also set instrumentation variables necessary (but only use the parents from assignment axioms, not from operators)
	set_variable_instrumentation_necessary(metric_var);
	// now set constants necessary which are required to compute the metric
	for (const auto &op : operators) {
		for (const auto &num_eff: op.get_num_eff()) {
			if (num_eff.var->get_type() == instrumentation) {
				assert(num_eff.var->is_necessary());
				set_variable_instrumentation_necessary(num_eff.foperand);
			}
		}
	}

    // extract propositional variables from ordering and leave out unimportant vars
    assert(propositional_ordering.empty());
    assert(numeric_ordering.empty());
    assert(ordering.size() == numeric_variables.size() + variables.size());
    for (CGVar cg_var : ordering)
    	if(cg_var.numeric) {
    		NumericVariable *nvar = cg_var.nvar;
    		if (nvar->is_necessary() || g_do_not_prune_variables)
    			numeric_ordering.push_back(nvar);
    	} else {
    		Variable *var = cg_var.var;
    		if (var->is_necessary() || g_do_not_prune_variables)
    			propositional_ordering.push_back(var);
    	}
    for (size_t i = 0; i < propositional_ordering.size(); i++) {
        propositional_ordering[i]->set_level(i);
    }
    for (size_t i = 0; i < numeric_ordering.size(); i++) {
        numeric_ordering[i]->set_level(i);
    }
    cout << propositional_ordering.size() << " variables of " << variables.size() << " necessary" << endl;
    cout << numeric_ordering.size() << " numeric variables of " << numeric_variables.size() << " necessary" << endl;
}

void CausalGraph::dfs(CGVar from) {
    for (const auto &pred : predecessor_graph[from]) {
        CGVar curr_predecessor = pred.first;
        if (!curr_predecessor.is_necessary()) {
            curr_predecessor.set_necessary();
            if(DEBUG) cout << "var " << curr_predecessor.get_name() <<" is neccessary." << endl;
            dfs(curr_predecessor);
        }
    }
}

void CausalGraph::set_variable_instrumentation_necessary(NumericVariable* inst_var) {
	if(!inst_var->is_necessary()) {
		if (DEBUG) cout << inst_var->get_name() << " is necessary for the metric"<< endl;
		inst_var->set_instrumentation();
	} else {
//		if (DEBUG) cout << inst_var->get_name() << " was necessary before "<< endl;
	}
	for(const auto ass_ax : ass_axioms) {
		if(inst_var == ass_ax.get_effect_var()) {
//			cout << "Ass Ax found for nvar " << inst_var->get_name() << endl;
//			cout << "Set lefthandside necessary " << ass_ax.get_left_var()->get_name() << endl;
			set_variable_instrumentation_necessary(ass_ax.get_left_var());
//			cout << "Set righthandside necessary " << ass_ax.get_right_var()->get_name() << endl;
			set_variable_instrumentation_necessary(ass_ax.get_right_var());
		}
	}
}


const vector<Variable *> &CausalGraph::get_variable_ordering() const {
    return propositional_ordering;
}

const vector<NumericVariable*>& CausalGraph::get_numeric_variable_ordering() const {
	return numeric_ordering;
}

bool CausalGraph::is_acyclic() const {
    return acyclic;
}

void CausalGraph::dump() const {
    for (const auto &source : weighted_graph) {
        cout << "dependent on var " << source.first.get_name() << ": " << endl;
        for (const auto &succ : source.second) {
            cout << "  [" << succ.first.get_name() << ", " << succ.second << "]" << endl;
            //assert(predecessor_graph[succ.first][source.first] == succ.second);
        }
    }
    for (const auto &source : predecessor_graph) {
        cout << "var " << source.first.get_name() << " is dependent of: " << endl;
        for (const auto &succ : source.second)
            cout << "  [" << succ.first.get_name() << ", " << succ.second << "]" << endl;
    }
}

// the index of the metric variable changes when variables are pruned
int CausalGraph::get_metric_index() const {
	return metric_var->get_level();
}

void CausalGraph::generate_cpp_input(ofstream &outfile,
                                     const vector<Variable *> &ordered_vars)
const {
    //TODO: use const iterator!
    vector<WeightedSuccessors *> succs; // will be ordered like ordered_vars
    vector<int> number_of_succ; // will be ordered like ordered_vars
    succs.resize(ordered_vars.size());
    number_of_succ.resize(ordered_vars.size());
    for (const auto &source : weighted_graph) {
    	if (!source.first.numeric) {
    		Variable *source_var = source.first.var;
    		if (source_var->get_level() != -1) {
    			// source variable influences others
    			WeightedSuccessors &curr = (WeightedSuccessors &)source.second;
    			succs[source_var->get_level()] = &curr;
    			// count number of influenced vars
    			int num = 0;
    			for (const auto &succ : curr)
    				if (!succ.first.numeric && succ.first.var->get_level() != -1
    						// && succ.first->get_level() > source_var->get_level()
    				)
    					num++;
    			number_of_succ[source_var->get_level()] = num;
    		}
    	}
    }
    int num_vars = ordered_vars.size();
    for (int i = 0; i < num_vars; i++) {
        WeightedSuccessors *curr = succs[i];
        // print number of variables influenced by variable i
        outfile << number_of_succ[i] << endl;
        for (const auto &succ : *curr) {
            if (!succ.first.numeric && succ.first.var->get_level() != -1
                // && succ.first->get_level() > ordered_vars[i]->get_level()
                )
                // the variable succ.first is important and influenced by variable i
                // print level and weight of influence
                outfile << succ.first.var->get_level() << " " << succ.second << endl;
        }
    }
}

string CGVar::get_name() const {
	if (numeric) {
		assert(nvar);
		return nvar->get_name();
	} else {
		assert(var);
		return var->get_name();
	}
}

void CGVar::set_necessary() {
	if (numeric) {
		assert(nvar);
		nvar->set_necessary();
	} else {
		assert(var);
		var->set_necessary();
	}
}

bool CGVar::is_necessary() const {
	if (numeric) {
		assert(nvar);
		return nvar->is_necessary();
	} else {
		assert(var);
		return var->is_necessary();
	}
}

bool CGVar::operator ==(const CGVar& other) const {
	return numeric == other.numeric && var==other.var && nvar == other.nvar;
}

bool CGVar::operator <(const CGVar& other) const {
	if (numeric == other.numeric)
		if (numeric)
			return nvar < other.nvar;
		else
			return var < other.var;
	else
		if (numeric)
			return false;
		else
			return true;
}
