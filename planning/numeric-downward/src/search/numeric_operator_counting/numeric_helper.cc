#include "numeric_helper.h"

#include <sstream>

#include "../axioms.h"
#include "../task_tools.h"

using namespace std;

namespace numeric_helper {

bool LinearNumericCondition::dominate(LinearNumericCondition &other, double precision) const {
  // if they are linearly dependent, than
  assert(coefficients.size() == other.coefficients.size());
  double ratio = 0;
  for (size_t v = 0; v < coefficients.size(); ++v) {
    double rhs = coefficients[v];
    double lhs = other.coefficients[v];
    if (fabs(lhs) < precision) {
      if (abs(rhs) >= precision) return false;
    } else {
      if (fabs(ratio) < precision) {
        ratio = rhs / lhs;
      }
      if (ratio < precision) return false;
      if (fabs(rhs / lhs - ratio) >= precision) return false;
    }
  }
  if (fabs(constant) - fabs(ratio * other.constant) >= -precision) return true;
  return false;
}

bool LinearNumericCondition::empty() const {
  // if they are linearly dependent, than
  // if (constant != 0) return false;
  for (size_t v = 0; v < coefficients.size(); ++v) {
    double rhs = coefficients[v];
    if (rhs != 0) return false;
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const LinearNumericCondition &lnc) {
  for (size_t num_id = 0; num_id < lnc.coefficients.size(); ++num_id) {
    os << " " << lnc.coefficients[num_id];
  }
  os << " " << lnc.constant;
  return os;
}

NumericVariable::NumericVariable(int id_, int id_at, double lb_, double ub_)
    : id_var(id_),
      id_abstract_task(id_at),
      upper_bound(ub_),
      lower_bound(lb_) {}

bool NumericTaskProxy::redundant_constraints = true;

NumericTaskProxy::NumericTaskProxy(const TaskProxy &task, bool separate_constant_assignment, bool additional, double epsilon, double precision, double infinity)
    : precision(precision), default_epsilon(epsilon) {
  bool numeric = true;
  if (numeric) build_numeric_variables(task);
  if (numeric) build_artificial_variables(task);
  if (numeric) build_numeric_conditions(task);
  if (numeric) build_numeric_goals(task);
  build_propositions(task);
  build_actions(task, separate_constant_assignment);
  if (additional) {
    build_mutex_actions(task);
  }

  if (numeric) {
    calculates_bounds_numeric_variables(infinity);
    calculates_small_m(infinity);
    calculates_epsilons();
  }
  if (additional) {
    if (numeric) calculates_dominance();
  }
}

void NumericTaskProxy::calculates_dominance() {
  int size = numeric_conditions.size();
  dominance_conditions.assign(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    LinearNumericCondition &lnc_i = numeric_conditions[i];
    for (int j = i; j < size; j++) {
      LinearNumericCondition &lnc_j = numeric_conditions[j];
      if (lnc_i.dominate(lnc_j, precision)) dominance_conditions[i][j] = true;
      if (lnc_j.dominate(lnc_i, precision)) dominance_conditions[j][i] = true;
    }
  }
}

void NumericTaskProxy::build_numeric_variables(const TaskProxy &task_proxy) {
  // variables initialization
  // double infinity = 9999999;
  // TODO add check
  NumericVariablesProxy num_variables = task_proxy.get_numeric_variables();
  id_numeric_variable_inv.assign(num_variables.size(), -1);
  for (size_t num_id = 0; num_id < num_variables.size(); ++num_id) {
    if (num_variables[num_id].get_var_type() == regular) {
      NumericVariable nv(numeric_variables.size(), num_id,
                         num_variables[num_id].get_initial_state_value(),
                         num_variables[num_id].get_initial_state_value());
      id_numeric_variable_inv[num_id] = numeric_variables.size();
      numeric_variables.push_back(nv);
    }
  }
  n_numeric_variables = numeric_variables.size();
}

void NumericTaskProxy::build_artificial_variables(const TaskProxy &task_proxy) {
  // variables initialization
  NumericVariablesProxy num_variables = task_proxy.get_numeric_variables();
  AssignmentAxiomsProxy assignment_axioms = task_proxy.get_assignment_axioms();
  artificial_variables.assign(num_variables.size(),
                              LinearNumericCondition(n_numeric_variables));
  for (size_t num_id = 0; num_id < num_variables.size(); ++num_id) {
    // artificial_variables[num_id].coefficients.assign(n_numeric_variables,0);
    if (num_variables[num_id].get_var_type() == regular) {
      artificial_variables[num_id]
          .coefficients[id_numeric_variable_inv[num_id]] = 1;
      // cout << num_id << " regular " << artificial_variables[num_id] << " " <<
      // num_variables[num_id].get_name() << endl;
    } else if (num_variables[num_id].get_var_type()) {
      artificial_variables[num_id].constant =
          num_variables[num_id].get_initial_state_value();
      // cout << num_id << " constant : " <<
      // num_variables[num_id].get_initial_state_value() << " " <<
      // artificial_variables[num_id] << " " << num_variables[num_id].get_name()
      // << endl;
    }
  }

  // populate artificial variables using ass axioms
  // initialize artificial variables
  for (size_t ax_id = 0; ax_id < assignment_axioms.size(); ++ax_id) {
    int affected_variable =
        assignment_axioms[ax_id].get_assignment_variable().get_id();
    int lhs = assignment_axioms[ax_id].get_left_variable().get_id();
    int rhs = assignment_axioms[ax_id].get_right_variable().get_id();

    switch (assignment_axioms[ax_id].get_arithmetic_operator_type()) {
      case sum:
        for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
          artificial_variables[affected_variable].coefficients[num_id] =
              artificial_variables[lhs].coefficients[num_id] +
              artificial_variables[rhs].coefficients[num_id];
        }
        artificial_variables[affected_variable].constant =
            artificial_variables[lhs].constant +
            artificial_variables[rhs].constant;
        break;
      case diff:
        for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
          artificial_variables[affected_variable].coefficients[num_id] =
              artificial_variables[lhs].coefficients[num_id] -
              artificial_variables[rhs].coefficients[num_id];
        }
        artificial_variables[affected_variable].constant =
            artificial_variables[lhs].constant -
            artificial_variables[rhs].constant;
        break;
      case mult:
        assert((num_variables[lhs].get_var_type() != constant) ||
               (num_variables[rhs].get_var_type() != constant));
        if (num_variables[lhs].get_var_type() == constant) {
          for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
            artificial_variables[affected_variable].coefficients[num_id] =
                artificial_variables[lhs].constant *
                artificial_variables[rhs].coefficients[num_id];
          }
          artificial_variables[affected_variable].constant =
              artificial_variables[lhs].constant *
              artificial_variables[rhs].constant;
        } else {
          for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
            artificial_variables[affected_variable].coefficients[num_id] =
                artificial_variables[lhs].coefficients[num_id] *
                artificial_variables[rhs].constant;
          }
          artificial_variables[affected_variable].constant =
              artificial_variables[lhs].constant *
              artificial_variables[rhs].constant;
        }
        break;
      case divi:
        assert(num_variables[rhs].get_var_type() != constant);
        for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
          artificial_variables[affected_variable].coefficients[num_id] =
              artificial_variables[lhs].coefficients[num_id] /
              artificial_variables[rhs].constant;
        }
        artificial_variables[affected_variable].constant =
            artificial_variables[lhs].constant /
            artificial_variables[rhs].constant;
        break;
      default:
        cout << "Error: No assignment operators are allowed here." << endl;
        assert(false);
        break;
    }
  }
}

void NumericTaskProxy::build_numeric_conditions(const TaskProxy &task_proxy) {
  fact_to_axiom_map.assign(task_proxy.get_variables().size(), -1);

  // initialize numeric conditions
  ComparisonAxiomsProxy axioms = task_proxy.get_comparison_axioms();
  n_conditions = 0;
  numeric_conditions_id.resize(axioms.size());
  for (size_t num_id = 0; num_id < axioms.size(); ++num_id) {
    ComparisonAxiomProxy ax = axioms[num_id];
    int left = ax.get_left_variable().get_id();
    int right = ax.get_right_variable().get_id();
    comp_operator comp = ax.get_comparison_operator_type();
    switch (comp) {  //    lt = 0, le = 1, eq = 2, ge = 3, gt = 4, ue = 5
      case lt: {
        LinearNumericCondition lnc =
            artificial_variables[right] - artificial_variables[left];
        lnc.is_strictly_greater = true;
        numeric_conditions.push_back(lnc);
        numeric_conditions_id[num_id].push_back(n_conditions);
        n_conditions++;
        break;
      }
      case le: {
        LinearNumericCondition lnc =
            artificial_variables[right] - artificial_variables[left];
        numeric_conditions.push_back(lnc);
        numeric_conditions_id[num_id].push_back(n_conditions);
        n_conditions++;
        break;
      }
      case eq: {
        numeric_conditions.push_back(artificial_variables[left] -
                                     artificial_variables[right]);
        numeric_conditions_id[num_id].push_back(n_conditions);
        n_conditions++;
        numeric_conditions.push_back(artificial_variables[right] -
                                     artificial_variables[left]);
        numeric_conditions_id[num_id].push_back(n_conditions);
        n_conditions++;
        break;
      }
      case ge: {
        LinearNumericCondition lnc =
            artificial_variables[left] - artificial_variables[right];
        numeric_conditions.push_back(lnc);
        numeric_conditions_id[num_id].push_back(n_conditions);
        n_conditions++;
        break;
      }
      case gt: {
        LinearNumericCondition lnc =
            artificial_variables[left] - artificial_variables[right];
        lnc.is_strictly_greater = true;
        numeric_conditions.push_back(lnc);
        numeric_conditions_id[num_id].push_back(n_conditions);
        n_conditions++;
        break;
      }
      case ue: {
        cout << "Error: Don't know this comparator." << endl;
        assert(false);
        break;
      }
      default: {
        cout << "Error: No other comparators." << endl;
        assert(false);
        break;
      }
    }
    // cout << ax.get_true_fact().get_variable().get_id() << " " << ax.get_id()
    // << endl;
    // TODO add this
    fact_to_axiom_map[ax.get_true_fact().get_variable().get_id()] = ax.get_id();
  }
}

void NumericTaskProxy::build_propositions(const TaskProxy &task) {
  VariablesProxy vars = task.get_variables();
  propositions.resize(vars.size());
  add_effects.resize(vars.size());
  n_propositions = 0;
  int i_var = 0;
  for (VariableProxy var : vars) {
    propositions[i_var] = (vector<int>(var.get_domain_size()));
    add_effects[i_var] = vector<set<int>>(var.get_domain_size());
    // index_constraints.push_back(vector<int>(var.get_domain_size()));
    for (int i = 0; i < var.get_domain_size(); ++i) {
      propositions[var.get_id()][i] = n_propositions++;
      // TODO change not pushback
      propositions_inv.push_back({i_var, i});
      map_vars.push_back(i_var);
    }
    i_var++;
  }
  n_vars = i_var;
}


void NumericTaskProxy::build_precondition(const FactProxy &condition,
                                          std::set<int> &pre_list,
                                          std::set<int> &num_list) {
  int pre_var_id = condition.get_variable().get_id();
  // cout << "\tpre " << pre_var_id << " " << condition.get_value() <<
  // endl;//effect.get_name() << endl;
  // if precondition is a numeric precondition
  if (fact_to_axiom_map[pre_var_id] == -1) {
    pre_list.insert(propositions[pre_var_id][condition.get_value()]);
  } else {
    // cout << op.get_id() << " " << op.get_name() << endl;
    if (condition.get_value() > 0) {
      // ignore negative numeric condittions
      return;
    }
    num_list.insert(fact_to_axiom_map[pre_var_id]);
    // for (auto num_id :
    // numeric_conditions_id[fact_to_axiom_map[pre_var_id]])
    //    cout << "\t" << numeric_conditions[num_id] << ", fact " <<
    //    fact_to_axiom_map[pre_var_id] << endl;
  }
}

void NumericTaskProxy::add_redundant_constraint(int x, int y, std::set<int> &target_list) {
  LinearNumericCondition redundant = numeric_conditions[x] + numeric_conditions[y];
  // cout << numeric_conditions[x] << " + "  <<
  // numeric_conditions[y] << " = " << redundant << endl;
  if (redundant.empty()) return;
  numeric_conditions.push_back(redundant);
  target_list.insert(numeric_conditions_id.size());
  numeric_conditions_id.push_back(list<int>(1, n_conditions));
  fact_to_axiom_map.push_back(-2);
  n_conditions++;
  achievers.push_back(set<int>());
  proposition_names.push_back("");
}

void NumericTaskProxy::build_redundant_constraints(const std::set<int> &original_list,
                                                   std::set<int> &target_list) {
  for (size_t i = 0; i < original_list.size(); ++i) {
    std::set<int>::iterator it_i = original_list.begin();
    std::advance(it_i, i);
    list<int> list_i = numeric_conditions_id[*it_i];
    list<int>::iterator nc_it_i = list_i.begin();
    list<int>::iterator nc_end_i = list_i.end();
    for (; nc_it_i != nc_end_i; ++nc_it_i) {
      int x = *nc_it_i;  //*numeric_conditions_id[*it_i].begin();
      for (size_t j = i + 1; j < original_list.size(); ++j) {
        // add the two
        std::set<int>::iterator it_j = original_list.begin();
        std::advance(it_j, j);
        list<int> list_j = numeric_conditions_id[*it_j];
        list<int>::iterator nc_it_j = list_j.begin();
        list<int>::iterator nc_end_j = list_j.end();
        for (; nc_it_j != nc_end_j; ++nc_it_j) {
          int y = *nc_it_j;
          add_redundant_constraint(x, y, target_list);
        }
      }
    }
  }
}

void NumericTaskProxy::build_redundant_constraints(const std::set<int> &list1, const std::set<int> &list2,
                                                   std::set<int> &target_list) {
  for (int x : list1) {
    for (int y : list2) {
      if (x != y) add_redundant_constraint(x, y, target_list);
    }
  }
}

void NumericTaskProxy::build_action(const TaskProxy &task, const OperatorProxy &op, size_t op_id, bool separate_constant_assignment) {
  vector<int> precondition(task.get_variables().size(), -1);
  actions[op_id].cost = op.get_cost();
  //cout << op_id << " " << op.get_name() <<  " " <<
  //op.get_preconditions().size() <<  " " << op.get_effects().size() << " "
  //<< op.get_ass_effects().size() << endl;
  for (FactProxy condition : op.get_preconditions()) {
    int pre_var_id = condition.get_variable().get_id();
    precondition[pre_var_id] = condition.get_value();
    build_precondition(condition, actions[op_id].pre_list, actions[op_id].num_list);
  }

  if (redundant_constraints) {
    set<int> original_list = actions[op_id].num_list;
    build_redundant_constraints(original_list, actions[op_id].num_list);
  }

  for (EffectProxy effect_proxy : op.get_effects()) {
    FactProxy effect = effect_proxy.get_fact();
    int var = effect.get_variable().get_id();
    int post = effect.get_value();
    // cout << "\teff " << var << " " << post << endl;//effect.get_name() <<
    // endl;
    assert(post != -1);
    // assert(pre != post);
    add_effects[var][post].insert(op_id);
    achievers[propositions[var][post]].insert(op_id);
    proposition_names[propositions[var][post]] = effect.get_name();
    // cout << "--- " << effect.get_name() << endl;

    EffectConditionsProxy conditions = effect_proxy.get_conditions();

    if (conditions.size() == 0) {
      int pre = precondition[var];
      if (pre != -1) {
        actions[op_id].add_list.insert(propositions[var][post]);
        actions[op_id].del_list.insert(propositions[var][pre]);
      } else {
        actions[op_id].add_list.insert(propositions[var][post]);
      }
    } else {
      int index = actions[op_id].n_conditional_eff;
      ++actions[op_id].n_conditional_eff;
      actions[op_id].eff_conditions.push_back(std::set<int>());
      actions[op_id].eff_num_conditions.push_back(std::set<int>());
      vector<int> extended_precondition = precondition;

      for (FactProxy condition : conditions) {
        int pre_var_id = condition.get_variable().get_id();
        extended_precondition[pre_var_id] = condition.get_value();
        build_precondition(condition, actions[op_id].eff_conditions[index],
                           actions[op_id].eff_num_conditions[index]);
      }

      if (redundant_constraints) {
        set<int> original_list = actions[op_id].eff_num_conditions[index];
        build_redundant_constraints(original_list, actions[op_id].eff_num_conditions[index]);
        build_redundant_constraints(original_list, actions[op_id].num_list, actions[op_id].eff_num_conditions[index]);
      }

      int pre = extended_precondition[var];
      if (pre != -1) {
        actions[op_id].conditional_add_list.push_back(propositions[var][post]);
        actions[op_id].conditional_del_list.push_back(propositions[var][pre]);
      } else {
        actions[op_id].conditional_add_list.push_back(propositions[var][post]);
        actions[op_id].conditional_del_list.push_back(-1);
      }
    }
  }

  // add intersection
  set<int>::iterator pre_it = actions[op_id].pre_list.begin();
  set<int>::iterator pre_end = actions[op_id].pre_list.end();
  set<int>::iterator del_it = actions[op_id].del_list.begin();
  set<int>::iterator del_end = actions[op_id].del_list.end();
  set<int> intersect;
  set_intersection(pre_it, pre_end, del_it, del_end,
                   inserter(intersect, intersect.begin()));
  actions[op_id].pre_del_list = intersect;
  // cout << "intersections " << op_id << " " << intersect.size() << " " <<
  // actions[op_id].pre_list.size() << " " << actions[op_id].del_list.size()
  // << endl; numeric effects
  // actions[op_id].eff_list.assign(n_numeric_variables, 0);
  AssEffectsProxy effs = op.get_ass_effects();
  // cout << op.get_name() << " - " << effs.size() << endl;
  for (size_t eff_id = 0; eff_id < effs.size(); ++eff_id) {
    AssEffectProxy eff = effs[eff_id];
    // cout << "\t" << eff.op_index << " " << eff.eff_index << " " <<
    // eff.is_axiom << endl;
    int lhs = eff.get_assignment().get_affected_variable().get_id();
    int rhs = eff.get_assignment().get_assigned_variable().get_id();
    f_operator oper = eff.get_assignment().get_assigment_operator_type();
    AssEffectConditionsProxy conditions = eff.get_conditions();
    LinearNumericCondition &av = artificial_variables[rhs];

    // cout << "effect " << lhs << " " << rhs << " " << oper << endl;
    if (task.get_numeric_variables()[lhs].get_var_type() == instrumentation) {
      // conditional or non-linear cost
      if (conditions.size() > 0 || (oper != increase && oper != decrease)) {
        continue;
      }

      // linear cost
      std::vector<ap_float> coefficients(n_numeric_variables, 0.0);
      bool no_coefficient = true;

      for (size_t var = 0; var < n_numeric_variables; ++var) {
        if (fabs(av.coefficients[var]) >= precision) {
          no_coefficient = false;

          if (oper == decrease) {
            coefficients[var] = -av.coefficients[var];
          } else {
            coefficients[var] = av.coefficients[var];
          }
        }
      }

      if (no_coefficient) {
        if ((oper == decrease && av.constant >= precision) || (oper == increase && av.constant <= -precision)) {
          std::cout << "negative action cost";
          assert(false);
        } else {
          // constant cost
          continue;
        }
      }

      actions[op_id].cost = 0;
      actions[op_id].linear_cost = true;
      ap_float constant = oper == increase ? av.constant : -av.constant;
      actions[op_id].cost_coefficients = coefficients;
      actions[op_id].cost_constant = constant;
      continue;
    }

    int id_num = id_numeric_variable_inv[lhs];
    if (id_num == -1) {
      cout << "Error: variable not constant" << endl;
      assert(false);
    }

    bool has_other_coefficients = false;
    for (size_t var = 0; var < n_numeric_variables; ++var) {
      if (var != static_cast<size_t>(id_num) && fabs(av.coefficients[var]) >= precision) {
        has_other_coefficients = true;
        break;
      }
    }

    bool is_constant_effect = !has_other_coefficients && fabs(av.coefficients[id_num]) < precision;
    bool is_simple_effect = is_constant_effect && (oper == increase || oper == decrease);
    bool is_assignment_effect = separate_constant_assignment && !has_other_coefficients
                                  && ((is_constant_effect && oper == assign)
                                    || (fabs(av.coefficients[id_num] + 1) < precision && oper == increase)
                                    || (fabs(av.coefficients[id_num] - 1) < precision && oper == decrease));

    if (is_simple_effect) {
      if (conditions.size() == 0) {
        if (oper == increase) actions[op_id].eff_list[id_num] = av.constant;
        if (oper == decrease) actions[op_id].eff_list[id_num] = -av.constant;
      } else {
        int index = actions[op_id].n_conditional_num_eff;
        ++actions[op_id].n_conditional_num_eff;
        actions[op_id].num_eff_conditions.push_back(std::set<int>());
        actions[op_id].num_eff_num_conditions.push_back(std::set<int>());
        for (FactProxy condition : conditions) {
          build_precondition(condition, actions[op_id].num_eff_conditions[index],
                             actions[op_id].num_eff_num_conditions[index]);
        }
        if (redundant_constraints) {
          set<int> original_list = actions[op_id].num_eff_num_conditions[index];
          build_redundant_constraints(original_list, actions[op_id].eff_num_conditions[index]);
          build_redundant_constraints(original_list, actions[op_id].num_list, actions[op_id].eff_num_conditions[index]);
        }
        if (oper == increase)
          actions[op_id].conditional_eff_list.push_back(std::make_pair(id_num, av.constant));
        if (oper == decrease)
          actions[op_id].conditional_eff_list.push_back(std::make_pair(id_num, -av.constant));
      }
    } else if (is_assignment_effect) {
      if (conditions.size() == 0) {
        if (oper == increase || oper == assign) {
          actions[op_id].assign_list[id_num] = av.constant;
        } else if (oper == decrease) {
          actions[op_id].assign_list[id_num] = -av.constant;
        } 
        actions[op_id].is_assignment[id_num] = true;
      } else {
        int index = actions[op_id].n_conditional_assign_eff;
        ++actions[op_id].n_conditional_assign_eff;
        actions[op_id].assign_eff_conditions.push_back(std::set<int>());
        actions[op_id].assign_eff_num_conditions.push_back(std::set<int>());
        for (FactProxy condition : conditions) {
          build_precondition(condition, actions[op_id].assign_eff_conditions[index],
                             actions[op_id].assign_eff_num_conditions[index]);
        }
        if (redundant_constraints) {
          set<int> original_list = actions[op_id].assign_eff_num_conditions[index];
          build_redundant_constraints(original_list, actions[op_id].assign_eff_num_conditions[index]);
          build_redundant_constraints(original_list, actions[op_id].num_list, actions[op_id].assign_eff_num_conditions[index]);
        }
        if (oper == increase || oper == assign) {
          actions[op_id].conditional_assign_list.push_back(std::make_pair(id_num, av.constant));
        } else if (oper == decrease) {
          actions[op_id].conditional_assign_list.push_back(std::make_pair(id_num, -av.constant));
        }
      }
    } else {
      int index = actions[op_id].n_linear_eff;
      ++actions[op_id].n_linear_eff;
      actions[op_id].linear_eff_lhs.push_back(id_num);
      actions[op_id].linear_eff_conditions.push_back(std::set<int>());
      actions[op_id].linear_eff_num_conditions.push_back(std::set<int>());

      if (conditions.size() > 0) {
        for (FactProxy condition : conditions) {
          build_precondition(condition, actions[op_id].linear_eff_conditions[index],
                             actions[op_id].linear_eff_num_conditions[index]);
        }
        if (redundant_constraints) {
          set<int> original_list = actions[op_id].linear_eff_num_conditions[index];
          build_redundant_constraints(original_list, actions[op_id].eff_num_conditions[index]);
          build_redundant_constraints(original_list, actions[op_id].num_list, actions[op_id].eff_num_conditions[index]);
        }
      }

      std::vector<ap_float> coefficients(n_numeric_variables, 0.0);
      switch (oper) {
        case (assign): {
          for (size_t var = 0; var < n_numeric_variables; ++var) {
            coefficients[var] = av.coefficients[var];
          }
          actions[op_id].linear_eff_coefficients.push_back(coefficients);
          actions[op_id].linear_eff_constants.push_back(av.constant);
          break;
        }
        case (increase): {
          for (size_t var = 0; var < n_numeric_variables; ++var) {
            coefficients[var] = av.coefficients[var];
            if (static_cast<size_t>(id_num) == var) coefficients[var] += 1.0;
          }
          actions[op_id].linear_eff_coefficients.push_back(coefficients);
          actions[op_id].linear_eff_constants.push_back(av.constant);
          break;
        }
        case (decrease): {
          for (size_t var = 0; var < n_numeric_variables; ++var) {
            coefficients[var] = -av.coefficients[var];
            if (static_cast<size_t>(id_num) == var) coefficients[var] += 1.0;
          }
          actions[op_id].linear_eff_coefficients.push_back(coefficients);
          actions[op_id].linear_eff_constants.push_back(-av.constant);
          break;
        }
        default: {
          cout << "non-linear numeric effect";
          assert(false);
          break;
        }
      }
    }
  }
}

void NumericTaskProxy::build_actions(const TaskProxy &task, bool separate_constant_assignment) {
  OperatorsProxy ops = task.get_operators();
  AxiomsProxy axioms = task.get_axioms();
  actions.assign(ops.size() + axioms.size(), Action(n_numeric_variables));
  achievers.assign(n_propositions + n_conditions, set<int>());
  n_actions = ops.size();
  proposition_names.assign(n_propositions + n_conditions, "");
  for (size_t op_id = 0; op_id < ops.size(); ++op_id)
    build_action(task, ops[op_id], op_id, separate_constant_assignment);
  for (size_t op_id = 0; op_id < axioms.size(); ++op_id)
    build_action(task, axioms[op_id], ops.size() + op_id, separate_constant_assignment);
  generate_possible_achievers(task);
}

void NumericTaskProxy::build_mutex_actions(const TaskProxy &task) {
  // cout << "---"  << endl;
  OperatorsProxy ops = task.get_operators();
  VariablesProxy vars = task.get_variables();
  mutex_actions.assign(ops.size(), set<int>());
  for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
    // two actions are mutex
    const OperatorProxy &op = ops[op_id];
    vector<int> precondition(vars.size(), -1);
    vector<int> postcondition(vars.size(), -1);
    for (FactProxy condition : op.get_preconditions()) {
      int pre_var_id = condition.get_variable().get_id();
      precondition[pre_var_id] = condition.get_value();
    }
    for (EffectProxy effect_proxy : op.get_effects()) {
      FactProxy effect = effect_proxy.get_fact();
      int var = effect.get_variable().get_id();
      postcondition[var] = effect.get_value();
    }
    //
    for (size_t op_a_id = 0; op_a_id < ops.size(); ++op_a_id) {
      if (op_id == op_a_id) continue;
      const OperatorProxy &op_a = ops[op_a_id];
      for (EffectProxy effect_proxy : op_a.get_effects()) {
        FactProxy effect = effect_proxy.get_fact();
        int var = effect.get_variable().get_id();
        int post = effect.get_value();
        // This is a SHAMEFUL HACK when you apply the tnf transformation
        if (op_a.get_name().find("forget") == 0 &&
            ops[op_id].get_name().find("forget") == 0)
          continue;
        if ((precondition[var] != post && precondition[var] != -1) ||
            (postcondition[var] != post)) {
          mutex_actions[op_id].insert(op_a_id);
          // if (op_a.get_name().find("forget")==0 &&
          // ops[op_id].get_name().find("forget")==0)
          //    cout << op_a.get_name() << " " << ops[op_id].get_name() << " are
          //    mutex" << endl;
        }
      }
    }
  }
}

void NumericTaskProxy::generate_possible_achievers(const TaskProxy &task) {
  size_t n_numeric_variables = get_n_numeric_variables();
  for (size_t nc_id = 0; nc_id < get_n_conditions(); ++nc_id) {
    for (size_t op_id = 0; op_id < task.get_operators().size(); ++op_id) {
      const LinearNumericCondition &nc = get_condition(nc_id);
      double cumulative_effect = 0;
      for (size_t v = 0; v < n_numeric_variables; ++v) {
        cumulative_effect +=
            (nc.coefficients[v] * get_action_eff_list(op_id)[v]);
      }

      if (cumulative_effect > 0) {
        achievers[nc_id + n_propositions].insert(op_id);
        actions[op_id].possible_add_list.insert(nc_id + n_propositions);
      }
    }
  }
}

void NumericTaskProxy::build_numeric_goals(const TaskProxy &task) {
  // check other axioms
  numeric_goals.assign(task.get_goals().size(), *(new list<int>()));
  propositional_goals.assign(task.get_goals().size(), *(new list<pair<int, int>>()));
  unordered_map<int, list<int>> axiom_table;
  unordered_map<int, list<pair<int, int>>> fact_table;
  {
    AxiomsProxy axioms = task.get_axioms();
    for (OperatorProxy ax : axioms) {
      for (FactProxy pre : ax.get_preconditions()) {
        // cout << "\tpre" << pre.get_name() << " " << pre.get_variable().get_id() << endl;
        if (ax.get_effects().size() > 1) {
          cout << "non-numerical axioms" << endl;
          assert(false);
        }
        if (ax.get_effects().size() == 0) continue;
        int eff_id = ax.get_effects()[0].get_fact().get_variable().get_id();
        int pre_id = pre.get_variable().get_id();
        if (fact_to_axiom_map[pre_id] == -1) {
          fact_table[eff_id].push_back(make_pair(pre_id, pre.get_value()));
        } else {
          axiom_table[eff_id].push_back(pre_id);
        }
      }
    }
  }

  for (size_t g_id = 0; g_id < task.get_goals().size(); ++g_id) {
    FactProxy goal = task.get_goals()[g_id];
    int goal_id = goal.get_variable().get_id();
    if (axiom_table.find(goal_id) != axiom_table.end()) {
      for (pair<int, int> goal : fact_table[goal_id]) {
        propositional_goals[g_id].push_back(goal);
      }
      for (int id_g : axiom_table[goal_id]) {
        for (int id_n_con : numeric_conditions_id[fact_to_axiom_map[id_g]]) {
          // check if it's not empty
          LinearNumericCondition &check_goal = numeric_conditions[id_n_con];
          if (check_goal.empty()) continue;

          // numeric_conditions_id[fact_to_axiom_map[axiom_table[goal_id]]])
          numeric_goals[g_id].push_back(id_n_con);
          fact_to_axiom_map[goal_id] = -2;  // this is a special number to
                                            // indicate that the goal is numeric
          // cout << "\t" << numeric_conditions[id_n_con] << endl;
        }
      }
    }
  }

  if (redundant_constraints) {
    std::vector<std::list<int>> original_list = numeric_goals;
    for (size_t t = 0; t < original_list.size(); ++t) {
      for (size_t i = 0; i < original_list[t].size(); ++i) {
        std::list<int>::iterator it_i = original_list[t].begin();
        std::advance(it_i, i);
        int x = *it_i;
        for (size_t j = i + 1; j < original_list[t].size(); ++j) {
          // add the two
          list<int>::iterator it_j = original_list[t].begin();
          std::advance(it_j, j);
          int y = *it_j;
          LinearNumericCondition redundant =
              numeric_conditions[x] + numeric_conditions[y];
          if (redundant.empty()) continue;
          //                        cout << "adding "  << redundant << " from "
          //                        << numeric_conditions[x] << " and " <<
          //                        numeric_conditions[y]  << " to goal" <<
          //                        endl; cout << "\tbefore " <<
          //                        numeric_conditions.size() << " " <<
          //                        numeric_conditions_id.size() << endl;
          numeric_conditions.push_back(redundant);
          numeric_goals[t].push_back(n_conditions);
          numeric_conditions_id.push_back(list<int>(n_conditions));
          fact_to_axiom_map.push_back(-2);
          n_conditions++;
          // achievers.push_back(set<int>());
          // proposition_names.push_back("");
          //                        cout << "\tafter " <<
          //                        numeric_conditions.size() << " " <<
          //                        numeric_conditions_id.size() << " " <<
          //                        n_conditions << endl;
        }
      }
    }
  }
}

// this is for numeric simple plans only
void NumericTaskProxy::calculates_bounds_numeric_variables(double infinity) {
  // check if actions with effects on a real variable x : x + c have a
  // precondition - w*x > b, with c > 0, than upperbound is max(current state,
  // b+c)
  for (size_t num_id = 0; num_id < n_numeric_variables; ++num_id) {
    // get all actions
    // cout << "check variable " << num_id << endl;
    for (size_t op_id = 0; op_id < n_actions; ++op_id) {
      double eff = actions[op_id].eff_list[num_id];
      if (fabs(eff) < precision) continue;  // action is not effecting the variable
      // cout << "\teffect on action " << op_id << " " << eff << endl;
      bool bound_for_this_action = false;
      double bound = 0;
      for (size_t c_id : actions[op_id].num_list) {
        // numeric preconditions
        for (int nc_id : numeric_conditions_id[c_id]) {
          // cout <<"\t\t" << nc_id << ":" << numeric_conditions[nc_id] << endl;
          double w = numeric_conditions[nc_id].coefficients[num_id];
          double k = numeric_conditions[nc_id].constant;
          // check if it's a condition of the form x + c : all other
          // coefficients are 0
          if (!numeric_conditions[nc_id].simple_condition(num_id, precision)) continue;
          // we have a condition of the simple type, we calculate the
          // constraints
          if (w * eff > 0) continue;  //
          bound_for_this_action = true;
          bound = -k / w + eff;
          // cout <<"\t\t\twith bound " << bound << endl;
        }
      }
      // check if bound found
      if (bound_for_this_action) {
        // if eff > 0, than the bound is an upper bound, else is a lower bound
        if (eff > 0) {
          double previous_bound = numeric_variables[num_id].upper_bound;
          numeric_variables[num_id].upper_bound = max(bound, previous_bound);
        } else {
          double previous_bound = numeric_variables[num_id].lower_bound;
          numeric_variables[num_id].lower_bound = min(bound, previous_bound);
        }
      } else {
        // bound not found, set to infinity and interrupt cycle (cannot because
        // we might find a lower/upper bound)
        if (eff > 0) {
          numeric_variables[num_id].upper_bound = infinity;
          // cout << "upper bound of " << num_id << " cannot be calculated" <<
          // endl;
        } else {
          numeric_variables[num_id].lower_bound = -infinity;
          // cout << "lower bound of " << num_id << " cannot be calculated" <<
          // endl;
        }
        // break;
      }
    }
    // cout << "numeric variable " << num_id << " [" <<
    // numeric_variables[num_id].lower_bound
    //<< "," << numeric_variables[num_id].upper_bound << "]" << endl;
  }
}

void NumericTaskProxy::calculates_small_m(double infinity) {
  small_m.assign(n_conditions, -infinity);
  for (size_t i = 0; i < n_conditions; ++i) {
    const LinearNumericCondition &lnc = get_condition(i);
    double l_b = lnc.constant;

    for (size_t n_id = 0; n_id < get_n_numeric_variables(); ++n_id) {
      if (lnc.coefficients[n_id] >= precision) {
        auto v_l_b = get_numeric_variable(n_id).lower_bound;
        if (v_l_b > -infinity) {
          l_b += lnc.coefficients[n_id] * v_l_b;
        } else {
          l_b = -infinity;
          break;
        }
      } else if (lnc.coefficients[n_id] <= -precision) {
        auto v_u_b = get_numeric_variable(n_id).upper_bound;
        if (v_u_b < infinity) {
          l_b += lnc.coefficients[n_id] * v_u_b;
        } else {
          l_b = -infinity;
          break;
        }
      }
    }
    small_m[i] = l_b;
  }
}

void NumericTaskProxy::calculates_epsilons() {
  epsilon.assign(n_conditions, 0);
  for (size_t i = 0; i < n_conditions; ++i) {
    const LinearNumericCondition &lnc = get_condition(i);
    if (!lnc.is_strictly_greater) continue;
    double min_epsilon = calculates_epsilon(lnc.constant);
    bool use_default_epsilon = false;
    for (size_t op_id = 0; op_id < get_n_actions(); ++op_id) {
      for (size_t j = 0; j < get_action_n_linear_eff(op_id); ++j) {
        int lhs = get_action_linear_lhs(op_id)[j];
        if (fabs(lnc.coefficients[lhs]) >= precision) {
          use_default_epsilon = true;
          break;
        }
      }

      if (use_default_epsilon) {
        break;
      } else {
        for (size_t n_id = 0; n_id < get_n_numeric_variables(); ++n_id) {
          if (get_action_is_assignment(op_id)[n_id])
            min_epsilon = std::min(min_epsilon, calculates_epsilon(get_action_assign_list(op_id)[n_id]));
        }

        double effect = 0;
        for (size_t n_id = 0; n_id < get_n_numeric_variables(); ++n_id) {
          effect += lnc.coefficients[n_id] * get_action_eff_list(op_id)[n_id];
        }
        min_epsilon = std::min(min_epsilon, calculates_epsilon(effect));
      }
    }
    if (use_default_epsilon)
      epsilon[i] = default_epsilon;
    else
      epsilon[i] = std::max(min_epsilon, default_epsilon);
  }
}

double NumericTaskProxy::calculates_epsilon(double value) const {
  double epsilon = 1.0;
  double fractional_part = fabs(round(value) - value);

  while (fractional_part >= precision) {
    value *= 10;
    epsilon /= 10.0;
    fractional_part = fabs(round(value) - value);
  }

  return epsilon;
}
}  // namespace numeric_helper
