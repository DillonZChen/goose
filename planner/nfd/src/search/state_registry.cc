#include "state_registry.h"

#include "axioms.h"
#include "globals.h"
#include "global_operator.h"
#include "per_state_information.h"
#include "../symmetries/graph_creator.h"
#include <cassert>

using namespace std;

StateRegistry::StateRegistry(int number_of_numeric_constants)
    : state_data_pool(g_state_packer->get_num_bins()),
      numeric_constants(vector<ap_float>(number_of_numeric_constants, 0)),
      numeric_indices(vector<int>(g_initial_state_numeric.size(),-1)),
      registered_states(0,
                        StateIDSemanticHash(state_data_pool),
                        StateIDSemanticEqual(state_data_pool)),
      cached_initial_state(0) {
}


StateRegistry::~StateRegistry() {
    for (set<PerStateInformationBase *>::iterator it = subscribers.begin();
         it != subscribers.end(); ++it) {
        (*it)->remove_state_registry(this);
    }
    delete cached_initial_state;
}

StateID StateRegistry::insert_id_or_pop_state() {
    /*
      Attempt to insert a StateID for the last state of state_data_pool
      if none is present yet. If this fails (another entry for this state
      is present), we have to remove the duplicate entry from the
      state data pool.
    */
    StateID id(state_data_pool.size() - 1);
    pair<StateIDSet::iterator, bool> result = registered_states.insert(id);
    bool is_new_entry = result.second;
    if (!is_new_entry) {
        state_data_pool.pop_back();
    }
    assert(registered_states.size() == state_data_pool.size());
    return *result.first;
}

GlobalState StateRegistry::lookup_state(StateID id) const {
    return GlobalState(state_data_pool[id.value], *this, id);
}

const GlobalState &StateRegistry::get_initial_state() {
    if (cached_initial_state == 0) {
//    	if(DEBUG) cout << "No initial state cached, creating new" << endl;
        PackedStateBin *buffer = new PackedStateBin[g_state_packer->get_num_bins()];
        // Avoid garbage values in half-full bins.
        fill_n(buffer, g_state_packer->get_num_bins(), 0);
        for (size_t i = 0; i < g_initial_state_data.size(); ++i) {
            g_state_packer->set(buffer, i, g_initial_state_data[i]);
        }
//        if(DEBUG) cout << "Initial state data size = " << g_initial_state_data.size() << " numeric = " << g_initial_state_numeric.size() << endl;
        int regular_index = g_initial_state_data.size(); // regular numeric variables are stored after logic variables
        int constant_index = 0;
        int derived_index = 0;
//        int instrumentation_index = 0;
        vector<ap_float> instrumentation_variables = vector<ap_float>();
        assert(instrumentation_variables.empty());
        assert(g_initial_state_numeric.size() == g_numeric_var_types.size());
        assert(g_initial_state_numeric.size() == numeric_indices.size());
        for (size_t i = 0; i < g_initial_state_numeric.size(); ++i) {
          switch (g_numeric_var_types[i]) {
          case instrumentation:
            // instrumentation variables are stored in a PerStateInformation attachment
            assert(numeric_indices[i] == -1);
            numeric_indices[i] = instrumentation_variables.size();
            instrumentation_variables.push_back(g_initial_state_numeric[i]);
            break;
          case constant:
            // constants are stored only once
            assert(numeric_indices[i] == -1);
            numeric_indices[i] = constant_index;
            assert((int) numeric_constants.size() > constant_index && numeric_constants[constant_index] == 0);
            numeric_constants[constant_index++] = g_initial_state_numeric[i];
            break;
          case unknown:
            assert(false);
            break;
          case derived:
            ++derived_index;
            // skipping derived variables, they are evaluated on the fly when needed
            break;
          case regular:
            // only regular variables are stored within the state buffer
            assert(numeric_indices[i] == -1);
            numeric_indices[i] = regular_index;
//        		if(DEBUG) cout << "regular variable numeric_indices[" << i << "] is " << regular_index << " name: " <<g_numeric_var_names[i] <<endl;
            //        	cout << "D->ULL " << g_initial_state_numeric[i] << " -> "<< reinterpretedDouble;// << endl;
            g_state_packer->set(buffer, regular_index++, g_state_packer->packDouble(g_initial_state_numeric[i]));
            break;
          default:
            assert(false);
            break;
          }
        }

        if (DEBUG) cout << "The initial state has " << constant_index << " constants, "
            << instrumentation_variables.size() << " instrumentation variables, "
            << derived_index << " derived variables and "
            << regular_index - g_initial_state_data.size() << " regular numeric variables " << endl;
//        if (DEBUG) cout << "Constants = " << numeric_constants << endl;
//        if (DEBUG) cout << "InstrVars = " << instrumentation_variables << endl;
        g_axiom_evaluator->evaluate_arithmetic_axioms(g_initial_state_numeric);
        g_axiom_evaluator->evaluate(buffer, g_initial_state_numeric); // evaluate logic axioms
        state_data_pool.push_back(buffer);
        // buffer is copied by push_back
        delete[] buffer;
        StateID id = insert_id_or_pop_state();
        cached_initial_state = new GlobalState(lookup_state(id));
        g_cost_information[*cached_initial_state] = instrumentation_variables; // save instrumentation variables in PerStateInformation attachment

        // reset the initial state with updated axioms
        // set g_initial_state_numeric to the state with evaluated axioms:
        g_initial_state_numeric = get_numeric_vars(*cached_initial_state);
        if(DEBUG) {
          cout << "Initial State created. It is: " << endl;
          cached_initial_state->dump_fdr();
        }
    }
    assert(cached_initial_state);
    return *cached_initial_state;
}

void StateRegistry::get_numeric_successor(
    std::vector<ap_float>& predecessor_vals,
    std::vector<ap_float>& metric_part,
    const GlobalOperator &op) {
  auto value_before_change = predecessor_vals;

  for (const auto & ass_eff : op.get_assign_effects()) {
    assert((int) predecessor_vals.size() > ass_eff.aff_var);
    assert((int) predecessor_vals.size() > ass_eff.ass_var);

//		if (DEBUG) cout << "assigning " << ass_eff.fop << " " << g_numeric_var_names[ass_eff.ass_var]
//						<< " to " << g_numeric_var_names[ass_eff.aff_var] << endl;
    ap_float ass_val = predecessor_vals[ass_eff.ass_var];

    if (g_numeric_var_types[ass_eff.ass_var] == regular)
      ass_val = value_before_change[ass_eff.ass_var];

    ap_float result = assign_effect(predecessor_vals[ass_eff.aff_var],ass_eff.fop, ass_val);
    //		if (DEBUG) cout << predecessor_vals[ass_eff.aff_var] << ass_eff.fop << predecessor_vals[ass_eff.ass_var] << " -> " << result << endl;

    switch (g_numeric_var_types[ass_eff.aff_var]) {
    case instrumentation:
      assert((int) metric_part.size() > numeric_indices[ass_eff.aff_var]);
      metric_part[numeric_indices[ass_eff.aff_var]] = result;
      predecessor_vals[ass_eff.aff_var] = result;
      break;
    case constant:
      assert(false); // Assign effect on a variable determined to be constant
      break;
    case regular:
      //    		cout << "state registry successor debug: " << "affvar = " << ass_eff.aff_var << " numeric index = " << numeric_indices[ass_eff.aff_var] << endl;
//				cout << "wrote result " << result << "  to buffer" << endl;
      predecessor_vals[ass_eff.aff_var] = result;
      break;
    default:
      assert(false); //Strange assignment effect in operator
      break;
    }
  }

  g_axiom_evaluator->evaluate_arithmetic_axioms(predecessor_vals);
}

void StateRegistry::get_numeric_successor(
    std::vector<ap_float>& predecessor_vals,
    std::vector<ap_float>& metric_part,
    const GlobalOperator &op,
    PackedStateBin *buffer,
    PackedStateBin const *previous_buffer) {

  for (const auto & ass_eff : op.get_assign_effects()) {
    assert((int) predecessor_vals.size() > ass_eff.aff_var);
    assert((int) predecessor_vals.size() > ass_eff.ass_var);

//		if (DEBUG) cout << "assigning " << ass_eff.fop << " " << g_numeric_var_names[ass_eff.ass_var]
//						<< " to " << g_numeric_var_names[ass_eff.aff_var] << endl;
    ap_float ass_val = predecessor_vals[ass_eff.ass_var];

    if (g_numeric_var_types[ass_eff.ass_var] == regular)
      ass_val = g_state_packer->getDouble(previous_buffer, numeric_indices[ass_eff.ass_var]);

    ap_float result = assign_effect(predecessor_vals[ass_eff.aff_var],ass_eff.fop, ass_val);
    //		if (DEBUG) cout << predecessor_vals[ass_eff.aff_var] << ass_eff.fop << predecessor_vals[ass_eff.ass_var] << " -> " << result << endl;

    switch (g_numeric_var_types[ass_eff.aff_var]) {
    case instrumentation:
      assert((int) metric_part.size() > numeric_indices[ass_eff.aff_var]);
      metric_part[numeric_indices[ass_eff.aff_var]] = result;
      predecessor_vals[ass_eff.aff_var] = result;
      break;
    case constant:
      assert(false); // Assign effect on a variable determined to be constant
      break;
    case regular:
      //    		cout << "state registry successor debug: " << "affvar = " << ass_eff.aff_var << " numeric index = " << numeric_indices[ass_eff.aff_var] << endl;
      g_state_packer->setDouble(buffer, numeric_indices[ass_eff.aff_var], result);
//				cout << "wrote result " << result << "  to buffer" << endl;
      predecessor_vals[ass_eff.aff_var] = result;
      break;
    default:
      assert(false); //Strange assignment effect in operator
      break;
    }
  }

  g_axiom_evaluator->evaluate_arithmetic_axioms(predecessor_vals);
  g_axiom_evaluator->evaluate(buffer, predecessor_vals); // evaluate logic + comparison axioms
}

void StateRegistry::get_canonical_numeric_successor(
    std::vector<ap_float>& predecessor_vals,
    std::vector<ap_float>& metric_part,
    const GlobalOperator &op,
    PackedStateBin *buffer,
    PackedStateBin const *previous_buffer) {
  assert(g_symmetry_graph != nullptr);

  for (const auto & ass_eff : op.get_assign_effects()) {
    assert((int) predecessor_vals.size() > ass_eff.aff_var);
    assert((int) predecessor_vals.size() > ass_eff.ass_var);

//		if (DEBUG) cout << "assigning " << ass_eff.fop << " " << g_numeric_var_names[ass_eff.ass_var]
//						<< " to " << g_numeric_var_names[ass_eff.aff_var] << endl;
    ap_float ass_val = predecessor_vals[ass_eff.ass_var];

    if (g_numeric_var_types[ass_eff.ass_var] == regular)
      ass_val = g_state_packer->getDouble(previous_buffer, numeric_indices[ass_eff.ass_var]);

    ap_float result = assign_effect(predecessor_vals[ass_eff.aff_var],ass_eff.fop, ass_val);
    //		if (DEBUG) cout << predecessor_vals[ass_eff.aff_var] << ass_eff.fop << predecessor_vals[ass_eff.ass_var] << " -> " << result << endl;

    switch (g_numeric_var_types[ass_eff.aff_var]) {
    case instrumentation:
      assert((int) metric_part.size() > numeric_indices[ass_eff.aff_var]);
      metric_part[numeric_indices[ass_eff.aff_var]] = result;
      predecessor_vals[ass_eff.aff_var] = result;
      break;
    case constant:
      assert(false); // Assign effect on a variable determined to be constant
      break;
    case regular:
      //    		cout << "state registry successor debug: " << "affvar = " << ass_eff.aff_var << " numeric index = " << numeric_indices[ass_eff.aff_var] << endl;
      if (buffer) {
        g_state_packer->setDouble(buffer, numeric_indices[ass_eff.aff_var], result);
//				cout << "wrote result " << result << "  to buffer" << endl;
      }
      predecessor_vals[ass_eff.aff_var] = result;
      break;
    default:
      assert(false); //Strange assignment effect in operator
      break;
    }
  }

  bool changed = g_symmetry_graph->to_canonical_state(buffer, predecessor_vals);
  if (changed && buffer) {
    for (size_t i = 0; i < predecessor_vals.size(); ++i) {
      if (g_numeric_var_types[i] == regular) {
        g_state_packer->setDouble(buffer, numeric_indices[i], predecessor_vals[i]);
      }
    }
  } 

  g_axiom_evaluator->evaluate_arithmetic_axioms(predecessor_vals);
  if (buffer) g_axiom_evaluator->evaluate(buffer, predecessor_vals); // evaluate logic + comparison axioms
}

//TODO it would be nice to move the actual state creation (and operator application)
//     out of the StateRegistry. This could for example be done by global functions
//     operating on state buffers (PackedStateBin *).
GlobalState StateRegistry::get_successor_state(const GlobalState &predecessor, const GlobalOperator &op) {
    assert(!op.is_axiom());
    state_data_pool.push_back(predecessor.get_packed_buffer());
    PackedStateBin *buffer = state_data_pool[state_data_pool.size() - 1];
    for (size_t i = 0; i < op.get_effects().size(); ++i) {
        const GlobalEffect &effect = op.get_effects()[i];
        if (effect.does_fire(predecessor))
            g_state_packer->set(buffer, effect.var, effect.val);
    }
//    if (DEBUG) cout << "Determining Successor state. getting predecessor..." << endl;
    vector<ap_float> succ_vals = get_numeric_vars(predecessor);
    vector<ap_float> inst_vals = g_cost_information[predecessor];
//    if (DEBUG) cout << "Predecessor vector = " << succ_vals << endl;
//    if (DEBUG) cout << "Instrumentation vector = " << inst_vals << endl;
    get_numeric_successor(succ_vals, inst_vals, op, buffer, predecessor.get_packed_buffer());
//    if (DEBUG) cout << "Successor vector = " << succ_vals << endl;
//    if (DEBUG) cout << "Instrumentation vector = " << inst_vals << endl;
    StateID id = insert_id_or_pop_state();
    GlobalState successor = lookup_state(id);
    if (id.value == (int) state_data_pool.size()-1) {
//    	if(DEBUG) cout << "New State!!!!" << endl;
      g_cost_information[successor] = inst_vals;
    } else {
      vector<ap_float> old_metric = g_cost_information[successor];
      ap_float old_val = evaluate_metric(get_numeric_vars(predecessor));
      ap_float new_val = evaluate_metric(succ_vals);
//    	if (DEBUG) cout << "Metric of old state = " << old_val << " new = " << new_val << endl;
      if (g_metric_minimizes && old_val < new_val) {
        g_cost_information[successor] = old_metric;
//    			cout << "metric minimizes, so the old metric value retains : " << evaluate_metric(successor);
      } else {
        g_cost_information[successor] = inst_vals;
//    		cout << "metric maximizes or oldval > newval" << endl;
      }

      if (!g_metric_minimizes && old_val > new_val) {
        g_cost_information[successor] = old_metric;
//    		cout << "metric maximizes, so the old metric value retains : " << evaluate_metric(successor);
      } else {
        g_cost_information[successor] = inst_vals;
        //    	else cout << "metric minimizes or oldval < newval" << endl;
      }
    }
//    if (DEBUG) {
//    	cout << "State registry returns successor of " << predecessor.id << " : " << id << " (Operator =" << op.get_name() << ")" << endl;
//    	successor.dump_fdr();
//    }
    return successor;
}

GlobalState StateRegistry::get_canonical_successor_state(const GlobalState &predecessor, const GlobalOperator &op) {
    assert(g_symmetry_graph != nullptr);
    assert(!op.is_axiom());
    state_data_pool.push_back(predecessor.get_packed_buffer());
    PackedStateBin *buffer = state_data_pool[state_data_pool.size() - 1];
    for (size_t i = 0; i < op.get_effects().size(); ++i) {
        const GlobalEffect &effect = op.get_effects()[i];
        if (effect.does_fire(predecessor))
            g_state_packer->set(buffer, effect.var, effect.val);
    }
//    if (DEBUG) cout << "Determining Successor state. getting predecessor..." << endl;
    vector<ap_float> succ_vals = get_numeric_vars(predecessor);
    vector<ap_float> inst_vals = g_cost_information[predecessor];
//    if (DEBUG) cout << "Predecessor vector = " << succ_vals << endl;
//    if (DEBUG) cout << "Instrumentation vector = " << inst_vals << endl;
    get_canonical_numeric_successor(succ_vals, inst_vals, op, buffer, predecessor.get_packed_buffer());
//    if (DEBUG) cout << "Successor vector = " << succ_vals << endl;
//    if (DEBUG) cout << "Instrumentation vector = " << inst_vals << endl;
    StateID id = insert_id_or_pop_state();
    GlobalState successor = lookup_state(id);
    if (id.value == (int) state_data_pool.size()-1) {
//    	if(DEBUG) cout << "New State!!!!" << endl;
      g_cost_information[successor] = inst_vals;
    } else {
      vector<ap_float> old_metric = g_cost_information[successor];
      ap_float old_val = evaluate_metric(get_numeric_vars(predecessor));
      ap_float new_val = evaluate_metric(succ_vals);
//    	if (DEBUG) cout << "Metric of old state = " << old_val << " new = " << new_val << endl;
      if (g_metric_minimizes && old_val < new_val) {
        g_cost_information[successor] = old_metric;
//    			cout << "metric minimizes, so the old metric value retains : " << evaluate_metric(successor);
      } else {
        g_cost_information[successor] = inst_vals;
//    		cout << "metric maximizes or oldval > newval" << endl;
      }

      if (!g_metric_minimizes && old_val > new_val) {
        g_cost_information[successor] = old_metric;
//    		cout << "metric maximizes, so the old metric value retains : " << evaluate_metric(successor);
      } else {
        g_cost_information[successor] = inst_vals;
        //    	else cout << "metric minimizes or oldval < newval" << endl;
      }
    }
//    if (DEBUG) {
//    	cout << "State registry returns successor of " << predecessor.id << " : " << id << " (Operator =" << op.get_name() << ")" << endl;
//    	successor.dump_fdr();
//    }
    return successor;
}

GlobalState StateRegistry::register_state(const std::vector<container_int> &values, std::vector<ap_float> &numeric_values) {
  PackedStateBin *buffer = new PackedStateBin[g_state_packer->get_num_bins()];
    // Avoid garbage values in half-full bins.
    fill_n(buffer, g_state_packer->get_num_bins(), 0);
    for (size_t i = 0; i < values.size(); ++i) {
        g_state_packer->set(buffer, i, values[i]);
    }
    int regular_index = values.size(); // regular numeric variables are stored after logic variables
    // int constant_index = 0;
    int derived_index = 0;
//    int instrumentation_index = 0;
    vector<ap_float> instrumentation_variables = vector<ap_float>();
    assert(instrumentation_variables.empty());
    for (size_t i = 0; i < numeric_values.size(); ++i) {
    	switch (g_numeric_var_types[i]) {
    	case instrumentation:
    		// instrumentation variables are stored in a PerStateInformation attachment
    		assert(numeric_indices[i] == -1);
    		numeric_indices[i] = instrumentation_variables.size();
    		instrumentation_variables.push_back(numeric_values[i]);
    		break;
    	case constant:
    		// constants are stored only once
    		assert(numeric_indices[i] == -1);
    		break;
    	case unknown:
    		assert(false);
    		break;
    	case derived:
    		++derived_index;
    		// skipping derived variables, they are evaluated on the fly when needed
    		break;
    	case regular:
    		// only regular variables are stored within the state buffer
    		assert(numeric_indices[i] == -1);
    		numeric_indices[i] = regular_index;
    		g_state_packer->set(buffer, regular_index++, g_state_packer->packDouble(numeric_values[i]));
    		break;
    	default:
    		assert(false);
    		break;
    	}
    }
    g_axiom_evaluator->evaluate_arithmetic_axioms(numeric_values);
    g_axiom_evaluator->evaluate(buffer, numeric_values); // evaluate logic axioms
    state_data_pool.push_back(buffer);
    // buffer is copied by push_back
    delete[] buffer;
    StateID id = insert_id_or_pop_state();
    GlobalState new_state = lookup_state(id);

    if (id.value == (int) state_data_pool.size()-1) {
//    	if(DEBUG) cout << "New State!!!!" << endl;
    	g_cost_information[new_state] = instrumentation_variables;
    } else {
    	vector<ap_float> old_metric = g_cost_information[new_state];
    	ap_float old_val = evaluate_metric(get_numeric_vars(new_state));
    	ap_float new_val = evaluate_metric(numeric_values);
//    	if (DEBUG) cout << "Metric of old state = " << old_val << " new = " << new_val << endl;
    	if (g_metric_minimizes && old_val < new_val) {
    		g_cost_information[new_state] = old_metric;
//    			cout << "metric minimizes, so the old metric value retains : " << evaluate_metric(successor);
    	} else {
    		g_cost_information[new_state] = instrumentation_variables;
//    		cout << "metric maximizes or oldval > newval" << endl;
    	}

    	if (!g_metric_minimizes && old_val > new_val) {
    		g_cost_information[new_state] = old_metric;
//    		cout << "metric maximizes, so the old metric value retains : " << evaluate_metric(successor);
    	} else {
    		g_cost_information[new_state] = instrumentation_variables;
    		//    	else cout << "metric minimizes or oldval < newval" << endl;
    	}
		}

    return new_state;
}

void StateRegistry::subscribe(PerStateInformationBase *psi) const {
    subscribers.insert(psi);
}

void StateRegistry::unsubscribe(PerStateInformationBase *const psi) const {
    subscribers.erase(psi);
}

ap_float StateRegistry::evaluate_metric(const vector<ap_float> &numeric_state) const {
  assert(g_metric_fluent_id >= 0);
  assert(g_metric_fluent_id < (int) numeric_state.size());
//	if(DEBUG) cout << "Metric (fluent id= "<<g_metric_fluent_id<<") evaluates to " << numeric_state[g_metric_fluent_id] << endl;
  return numeric_state[g_metric_fluent_id];
}

ap_float StateRegistry::assign_effect(ap_float aff_value, f_operator fop, ap_float ass_value) {
  ap_float result = aff_value;
  switch(fop) {
  case assign:
    result = ass_value;
    break;
  case scale_up:
    result *= ass_value;
    break;
  case scale_down:
    result /= ass_value;
    break;
  case increase:
    result += ass_value;
    break;
  case decrease:
    result -= ass_value;
    break;
  default:
    cout << "Error: Unknown assignment effect (" << fop << ")."<< endl;
    assert(false);
    break;
  }
  return result;
}

std::vector<ap_float> StateRegistry::get_numeric_vars(const GlobalState &state) const {
  vector<ap_float> result = vector<ap_float>();
//	if(DEBUG) cout << "Retrieving numeric state variables from StateRegistry" <<endl;
    vector<ap_float> instrumentation_variables = g_cost_information[state];
//    if(DEBUG) cout << "instrumentation variables " << instrumentation_variables << endl;
    assert(g_initial_state_numeric.size() == g_numeric_var_types.size());
    assert(g_initial_state_numeric.size() == numeric_indices.size());
    const PackedStateBin *buffer = state.get_packed_buffer();
    for (size_t i = 0; i < g_numeric_var_types.size(); ++i) {
      assert(i < numeric_indices.size());
      switch (g_numeric_var_types[i]) {
      case instrumentation:
//    		if (DEBUG) cout << "instrumentation_variables.size()" << instrumentation_variables.size()
//    				<< "numeric_indices["<<i<<"] " << numeric_indices[i]<< endl;
        assert((int) instrumentation_variables.size() > numeric_indices[i]);
        result.push_back(instrumentation_variables[numeric_indices[i]]);
        break;
      case constant:
        assert((int) numeric_constants.size() > numeric_indices[i]);
        result.push_back(numeric_constants[numeric_indices[i]]);
        break;
      case derived:
        result.push_back(0); // default value, axioms will be evaluated right after this for loop
        break;
      case unknown:
        assert(false);
        break;
      case regular:
//    		if (DEBUG) cout << "variable #" << i << " has buffer index " << numeric_indices[i] << endl;
//    		cout << "unpacked double is " << g_state_packer->getDouble(buffer, numeric_indices[i]) << endl;
        result.push_back(g_state_packer->getDouble(buffer, numeric_indices[i]));
        break;
      default:
        assert(false);
        break;
      }
    }
    assert(result.size() == g_initial_state_numeric.size());
//    if (DEBUG) cout << "numeric vars before evaluating axioms\n"<< result << endl;
    if(has_numeric_axioms()) {
//    	if (DEBUG) cout << "evaluating numeric axioms..." << endl;
      g_axiom_evaluator->evaluate_arithmetic_axioms(result);
    }
//    if (DEBUG) cout << "numeric vars after evaluating axioms\n"<< result << endl;
    return result;
}
