#ifndef TASK_PROXY_H
#define TASK_PROXY_H

#include "abstract_task.h"

#include "utils/hash.h"
#include "utils/system.h"

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "../utils/countdown_timer.h"


class AxiomsProxy;
class ComparisonAxiomProxy;
class ComparisonAxiomsProxy;
class AssignmentAxiomProxy;
class AssignmentAxiomsProxy;
class CausalGraph;
class ConditionsProxy;
class EffectProxy;
class EffectConditionsProxy;
class EffectsProxy;
class AssEffectProxy;
class AssEffectsProxy;
class FactProxy;
class FactsProxy;
class GoalsProxy;
class OperatorProxy;
class OperatorsProxy;
class PreconditionsProxy;
class State;
class TaskProxy;
class VariableProxy;
class VariablesProxy;
class NumericVariableProxy;
class NumericVariablesProxy;

/*
  Overview of the task interface.

  The task interface is divided into two parts: a set of proxy classes
  for accessing task information (TaskProxy, OperatorProxy, etc.) and
  task implementations (subclasses of AbstractTask). Each proxy class
  knows which AbstractTask it belongs to and uses its methods to retrieve
  information about the task. RootTask is the AbstractTask that
  encapsulates the unmodified original task that the planner received
  as input.

  Example code for creating a new task object and accessing its operators:

      TaskProxy task = new TaskProxy(new RootTask());
      for (OperatorProxy op : task->get_operators())
          cout << op.get_name() << endl;

  Since proxy classes only store a reference to the AbstractTask and some
  indices, they can be copied cheaply and be passed by value instead of
  by reference.

  In addition to the lightweight proxy classes, the task interface
  consists of the State class, which is used to hold state information
  for TaskProxy tasks. The State class provides methods similar to the
  proxy classes, but since State objects own the state data they should
  be passed by reference.

  For now, only the heuristics work with the TaskProxy classes and hence
  potentially on a transformed view of the original task. The search
  algorithms keep working on the original unmodified task using the
  GlobalState, GlobalOperator etc. classes. We therefore need to do two
  conversions: converting GlobalStates to State objects for the heuristic
  computation and converting OperatorProxy objects used by the heuristic
  to GlobalOperators for reporting preferred operators. These conversions
  are done by the Heuristic base class. Until all heuristics use the new
  task interface, heuristics can use Heuristic::convert_global_state() to
  convert GlobalStates to States. Afterwards, the heuristics are passed a
  State object directly. To mark operators as preferred, heuristics can
  use Heuristic::set_preferred() which currently works for both
  OperatorProxy and GlobalOperator objects.

      int FantasyHeuristic::compute_heuristic(const GlobalState &global_state) {
          State state = convert_global_state(global_state);
          set_preferred(task->get_operators()[42]);
          int sum = 0;
          for (FactProxy fact : state)
              sum += fact.get_value();
          return sum;
      }

  For helper functions that work on task related objects, please see the
  task_tools.h module.
*/


// Basic iterator support for proxy classes.

template<class ProxyCollection>
class ProxyIterator {
    const ProxyCollection &collection;
    std::size_t pos;
public:
    ProxyIterator(const ProxyCollection &collection, std::size_t pos)
        : collection(collection), pos(pos) {}
    ~ProxyIterator() = default;

    typename ProxyCollection::ItemType operator*() const {
        return collection[pos];
    }

    ProxyIterator &operator++() {
        ++pos;
        return *this;
    }

    bool operator==(const ProxyIterator &other) const {
        return pos == other.pos;
    }

    bool operator!=(const ProxyIterator &other) const {
        return !(*this == other);
    }
};

template<class ProxyCollection>
inline ProxyIterator<ProxyCollection> begin(ProxyCollection &collection) {
    return ProxyIterator<ProxyCollection>(collection, 0);
}

template<class ProxyCollection>
inline ProxyIterator<ProxyCollection> end(ProxyCollection &collection) {
    return ProxyIterator<ProxyCollection>(collection, collection.size());
}


class FactProxy {
    const AbstractTask *task;
    Fact fact;
public:
    FactProxy(const AbstractTask &task, int var_id, int value);
    FactProxy(const AbstractTask &task, const Fact &fact);
    ~FactProxy() = default;

    VariableProxy get_variable() const;

    int get_value() const {
        return fact.value;
    }

    const std::string &get_name() const {
        return task->get_fact_name(fact);
    }

    bool operator==(const FactProxy &other) const {
        assert(task == other.task);
        return fact == other.fact;
    }

    bool operator!=(const FactProxy &other) const {
        return !(*this == other);
    }

    bool is_mutex(const FactProxy &other) const {
        return task->are_facts_mutex(fact, other.fact);
    }
};


class FactsProxyIterator {
    const AbstractTask *task;
    int var_id;
    int value;
public:
    FactsProxyIterator(const AbstractTask &task, int var_id, int value)
        : task(&task), var_id(var_id), value(value) {}
    ~FactsProxyIterator() = default;

    FactProxy operator*() const {
        return FactProxy(*task, var_id, value);
    }

    FactsProxyIterator &operator++() {
        assert(var_id < task->get_num_variables());
        int num_facts = task->get_variable_domain_size(var_id);
        assert(value < num_facts);
        ++value;
        if (value == num_facts) {
            ++var_id;
            value = 0;
        }
        return *this;
    }

    bool operator==(const FactsProxyIterator &other) const {
        assert(task == other.task);
        return var_id == other.var_id && value == other.value;
    }

    bool operator!=(const FactsProxyIterator &other) const {
        return !(*this == other);
    }
};


/*
  Proxy class for the collection of all facts of a task.

  We don't implement size() because it would not be constant-time.
*/
class FactsProxy {
    const AbstractTask *task;
public:
    explicit FactsProxy(const AbstractTask &task)
        : task(&task) {}
    ~FactsProxy() = default;

    FactsProxyIterator begin() const {
        return FactsProxyIterator(*task, 0, 0);
    }

    FactsProxyIterator end() const {
        return FactsProxyIterator(*task, task->get_num_variables(), 0);
    }
};

class NumAssProxy {
    const AbstractTask *task;

    int aff_var_id;
    f_operator op_type;
    int ass_var_id;

public:
    NumAssProxy(const AbstractTask &task, int aff_var_id, f_operator op_type, int ass_var_id);
    ~NumAssProxy() = default;

    NumericVariableProxy get_affected_variable() const;
    f_operator get_assigment_operator_type() const;
    NumericVariableProxy get_assigned_variable() const;

    bool operator==(NumAssProxy const &other) const {
        assert(task == other.task);
        return aff_var_id == other.aff_var_id &&
        		op_type == other.op_type &&
        		ass_var_id == other.ass_var_id;
    }

    bool operator!=(NumAssProxy other) const {
        return !(*this == other);
    }
};


class ConditionsProxy {
protected:
    const AbstractTask *task;
public:
    using ItemType = FactProxy;
    explicit ConditionsProxy(const AbstractTask &task)
        : task(&task) {}
    virtual ~ConditionsProxy() = default;

    virtual std::size_t size() const = 0;
    virtual FactProxy operator[](std::size_t index) const = 0;

    bool empty() const {
        return size() == 0;
    }
};


class VariableProxy {
    const AbstractTask *task;
    int id;
public:
    VariableProxy(const AbstractTask &task, int id)
        : task(&task), id(id) {}
    ~VariableProxy() = default;

    bool operator==(const VariableProxy &other) const {
        assert(task == other.task);
        return id == other.id;
    }

    bool operator!=(const VariableProxy &other) const {
        return !(*this == other);
    }

    int get_id() const {
        return id;
    }

    const std::string &get_name() const {
        return task->get_variable_name(id);
    }

    int get_domain_size() const {
        return task->get_variable_domain_size(id);
    }

    FactProxy get_fact(int index) const {
        assert(index < get_domain_size());
        return FactProxy(*task, id, index);
    }
};


class VariablesProxy {
    const AbstractTask *task;
public:
    using ItemType = VariableProxy;
    explicit VariablesProxy(const AbstractTask &task)
        : task(&task) {}
    ~VariablesProxy() = default;

    std::size_t size() const {
        return task->get_num_variables();
    }

    VariableProxy operator[](std::size_t index) const {
        assert(index < size());
        return VariableProxy(*task, index);
    }

    FactsProxy get_facts() const {
        return FactsProxy(*task);
    }
};

class NumericVariableProxy {
    const AbstractTask *task;
    int var_id;
public:
    NumericVariableProxy(const AbstractTask &task, int id)
        : task(&task), var_id(id) {}
    ~NumericVariableProxy() = default;

    bool operator==(const NumericVariableProxy &other) const {
        assert(task == other.task);
        return var_id == other.var_id;
    }

    bool operator!=(const NumericVariableProxy &other) const {
        return !(*this == other);
    }

    int get_id() const {
        return var_id;
    }

    const std::string &get_name() const {
        return task->get_numeric_variable_name(var_id);
    }
    
    numType get_var_type() const {
        return task->get_numeric_var_type(var_id);
    }
    
    ap_float get_initial_state_value() const {
        return task->get_initial_state_numeric_values()[var_id];
    }
};

class NumericVariablesProxy {
    const AbstractTask *task;
public:
    using ItemType = NumericVariableProxy;
    explicit NumericVariablesProxy(const AbstractTask &task)
        : task(&task) {}
    ~NumericVariablesProxy() = default;

    std::size_t size() const {
        return task->get_num_numeric_variables();
    }

    NumericVariableProxy operator[](std::size_t index) const {
        assert(index < size());
        return NumericVariableProxy(*task, index);
    }
};

class PreconditionsProxy : public ConditionsProxy {
    int op_index;
    bool is_axiom;
public:
    PreconditionsProxy(const AbstractTask &task, int op_index, bool is_axiom)
        : ConditionsProxy(task), op_index(op_index), is_axiom(is_axiom) {}
    ~PreconditionsProxy() = default;

    std::size_t size() const override {
        return task->get_num_operator_preconditions(op_index, is_axiom);
    }

    FactProxy operator[](std::size_t fact_index) const override {
        assert(fact_index < size());
        return FactProxy(*task, task->get_operator_precondition(
                             op_index, fact_index, is_axiom));
    }
};


class EffectConditionsProxy : public ConditionsProxy {
    int op_index;
    int eff_index;
    bool is_axiom;
public:
    EffectConditionsProxy(
        const AbstractTask &task, int op_index, int eff_index, bool is_axiom)
        : ConditionsProxy(task), op_index(op_index), eff_index(eff_index), is_axiom(is_axiom) {}
    ~EffectConditionsProxy() = default;

    std::size_t size() const override {
        return task->get_num_operator_effect_conditions(op_index, eff_index, is_axiom);
    }

    FactProxy operator[](std::size_t index) const override {
        assert(index < size());
        return FactProxy(*task, task->get_operator_effect_condition(
                             op_index, eff_index, index, is_axiom));
    }
};

class AssEffectConditionsProxy : public ConditionsProxy {
    int op_index;
    int eff_index;
    bool is_axiom;
public:
    AssEffectConditionsProxy(
        const AbstractTask &task, int op_index, int eff_index, bool is_axiom)
        : ConditionsProxy(task), op_index(op_index), eff_index(eff_index), is_axiom(is_axiom) {}
    ~AssEffectConditionsProxy() = default;

    std::size_t size() const override {
        return task->get_num_operator_ass_effect_conditions(op_index, eff_index, is_axiom);
    }

    FactProxy operator[](std::size_t index) const override {
        assert(index < size());
        return FactProxy(*task, task->get_operator_ass_effect_condition(op_index, eff_index, index, is_axiom));
    }
};


class EffectProxy {
    const AbstractTask *task;
    int op_index;
    int eff_index;
    bool is_axiom;
public:
    EffectProxy(const AbstractTask &task, int op_index, int eff_index, bool is_axiom)
        : task(&task), op_index(op_index), eff_index(eff_index), is_axiom(is_axiom) {}
    ~EffectProxy() = default;

    EffectConditionsProxy get_conditions() const {
        return EffectConditionsProxy(*task, op_index, eff_index, is_axiom);
    }

    FactProxy get_fact() const {
        return FactProxy(*task, task->get_operator_effect(
                             op_index, eff_index, is_axiom));
    }
};


class EffectsProxy {
    const AbstractTask *task;
    int op_index;
    bool is_axiom;
public:
    using ItemType = EffectProxy;
    EffectsProxy(const AbstractTask &task, int op_index, bool is_axiom)
        : task(&task), op_index(op_index), is_axiom(is_axiom) {}
    ~EffectsProxy() = default;

    std::size_t size() const {
        return task->get_num_operator_effects(op_index, is_axiom);
    }

    EffectProxy operator[](std::size_t eff_index) const {
        assert(eff_index < size());
        return EffectProxy(*task, op_index, eff_index, is_axiom);
    }
};

class AssEffectProxy {
    const AbstractTask *task;
    int op_index;
    int eff_index;
    bool is_axiom;
public:
    AssEffectProxy(const AbstractTask &task, int op_index, int eff_index, bool is_axiom)
        : task(&task), op_index(op_index), eff_index(eff_index), is_axiom(is_axiom) {}
    ~AssEffectProxy() = default;

    AssEffectConditionsProxy get_conditions() const {
        return AssEffectConditionsProxy(*task, op_index, eff_index, is_axiom);
    }

    NumAssProxy get_assignment() const {
    	AssEffect assignment =
            task->get_operator_ass_effect(op_index, eff_index, is_axiom);
        return NumAssProxy(*task, assignment.aff_var, assignment.op_type, assignment.ass_var);
    }
};


class AssEffectsProxy {
    const AbstractTask *task;
    int op_index;
    bool is_axiom;
public:
    using ItemType = AssEffectProxy;
    AssEffectsProxy(const AbstractTask &task, int op_index, bool is_axiom)
        : task(&task), op_index(op_index), is_axiom(is_axiom) {}
    ~AssEffectsProxy() = default;

    std::size_t size() const {
        return task->get_num_operator_ass_effects(op_index, is_axiom);
    }

    AssEffectProxy operator[](std::size_t eff_index) const {
        assert(eff_index < size());
        return AssEffectProxy(*task, op_index, eff_index, is_axiom);
    }
};


class ComparisonAxiomProxy {
	const AbstractTask *task;
	int index;
public:
	ComparisonAxiomProxy(const AbstractTask &task, int index)
	: task(&task), index(index) {}
	~ComparisonAxiomProxy() = default;

    NumericVariableProxy get_left_variable() const {
    	int varid = task->get_comparison_axiom_argument(index, true);
    	return NumericVariableProxy(*task, varid);
    }

    NumericVariableProxy get_right_variable() const {
    	int varid = task->get_comparison_axiom_argument(index, false);
    	return NumericVariableProxy(*task, varid);
    }

    comp_operator get_comparison_operator_type() const {
    	return task->get_comparison_axiom_operator(index);
    }

    FactProxy get_true_fact() const { // fact that is enabled if the axiom evaluates to true
        return FactProxy(*task, task->get_comparison_axiom_effect(index, true));
    }

    FactProxy get_false_fact() const {
            return FactProxy(*task, task->get_comparison_axiom_effect(index, false));
        }

    int get_id() const {
        return index;
    }
};

class ComparisonAxiomsProxy {
    const AbstractTask *task;
public:
    using ItemType = ComparisonAxiomProxy;
    ComparisonAxiomsProxy(const AbstractTask &task) : task(&task) {}
    ~ComparisonAxiomsProxy() = default;

    std::size_t size() const {
        return task->get_num_cmp_axioms();
    }

    ComparisonAxiomProxy operator[](std::size_t index) const {
        assert(index < size());
        return ComparisonAxiomProxy(*task, index);
    }
};


class AssignmentAxiomProxy {
	const AbstractTask *task;
	int index;
public:
	AssignmentAxiomProxy(const AbstractTask &task, int index)
: task(&task), index(index) {}
	~AssignmentAxiomProxy() = default;

    NumericVariableProxy get_left_variable() const {
    	int varid = task->get_assignment_axiom_argument(index, true);
    	return NumericVariableProxy(*task, varid);
    }

    NumericVariableProxy get_right_variable() const {
    	int varid = task->get_assignment_axiom_argument(index, false);
    	return NumericVariableProxy(*task, varid);
    }

    cal_operator get_arithmetic_operator_type() const {
    	return task->get_assignment_axiom_operator(index);
    }

    NumericVariableProxy get_assignment_variable() const {
    	int varid = task->get_assignment_axiom_effect(index);
    	return NumericVariableProxy(*task, varid);
    }

    int get_id() const {
        return index;
    }
};

class AssignmentAxiomsProxy {
    const AbstractTask *task;
public:
    using ItemType = AssignmentAxiomProxy;
    AssignmentAxiomsProxy(const AbstractTask &task) : task(&task) {}
    ~AssignmentAxiomsProxy() = default;

    std::size_t size() const {
        return task->get_num_ass_axioms();
    }

    AssignmentAxiomProxy operator[](std::size_t index) const {
        assert(index < size());
        return AssignmentAxiomProxy(*task, index);
    }
};

class OperatorProxy {
    const AbstractTask *task;
    int index;
    bool is_an_axiom;
public:
    OperatorProxy(const AbstractTask &task, int index, bool is_axiom)
        : task(&task), index(index), is_an_axiom(is_axiom) {}
    ~OperatorProxy() = default;

    bool operator==(const OperatorProxy &other) const {
        assert(task == other.task);
        return index == other.index && is_an_axiom == other.is_an_axiom;
    }

    bool operator!=(const OperatorProxy &other) const {
        return !(*this == other);
    }

    PreconditionsProxy get_preconditions() const {
        return PreconditionsProxy(*task, index, is_an_axiom);
    }

    EffectsProxy get_effects() const {
        return EffectsProxy(*task, index, is_an_axiom);
    }

    AssEffectsProxy get_ass_effects() const {
        return AssEffectsProxy(*task, index, is_an_axiom);
    }

    ap_float get_cost() const {
        return task->get_operator_cost(index, is_an_axiom);
    }

    bool is_axiom() const {
        return is_an_axiom;
    }

    const std::string &get_name() const {
        return task->get_operator_name(index, is_an_axiom);
    }

    int get_id() const {
        return index;
    }

    const GlobalOperator *get_global_operator() const {
        return task->get_global_operator(index, is_an_axiom);
    }
};


class OperatorsProxy {
    const AbstractTask *task;
public:
    using ItemType = OperatorProxy;
    explicit OperatorsProxy(const AbstractTask &task)
        : task(&task) {}
    ~OperatorsProxy() = default;

    std::size_t size() const {
        return task->get_num_operators();
    }

    bool empty() const {
        return size() == 0;
    }

    OperatorProxy operator[](std::size_t index) const {
        assert(index < size());
        return OperatorProxy(*task, index, false);
    }
};


class AxiomsProxy {
    const AbstractTask *task;
public:
    using ItemType = OperatorProxy;
    explicit AxiomsProxy(const AbstractTask &task)
        : task(&task) {}
    ~AxiomsProxy() = default;

    std::size_t size() const {
        return task->get_num_axioms();
    }

    bool empty() const {
        return size() == 0;
    }

    OperatorProxy operator[](std::size_t index) const {
        assert(index < size());
        return OperatorProxy(*task, index, true);
    }
};


class GoalsProxy : public ConditionsProxy {
public:
    explicit GoalsProxy(const AbstractTask &task)
        : ConditionsProxy(task) {}
    ~GoalsProxy() = default;

    std::size_t size() const override {
        return task->get_num_goals();
    }

    FactProxy operator[](std::size_t index) const override {
        assert(index < size());
        return FactProxy(*task, task->get_goal_fact(index));
    }
};


bool does_fire(EffectProxy effect, const State &state);


class State {
    const AbstractTask *task;
    std::vector<int> values;
    std::vector<ap_float> num_values;
public:
    using ItemType = FactProxy;
    State(const AbstractTask &task, std::vector<int> &&values, std::vector<ap_float> &&num_vals)
        : task(&task), values(std::move(values)), num_values(std::move(num_vals)) {
            //std::cout << "state " << size() << " " << this->task->get_num_variables() << std::endl;
        assert(static_cast<int>(size()) == this->task->get_num_variables());
       if (DEBUG)
            std::cout << "num_values.size = " << num_values.size() << " task.get_num_numeric_vars = " << this->task->get_num_numeric_variables() << std::endl;
        assert(static_cast<int>(num_values.size()) == this->task->get_num_numeric_variables());
    }
    ~State() = default;
    State(const State &) = default;

    State(State &&other)
        : task(other.task), values(std::move(other.values)), num_values(std::move(other.num_values)) {
        other.task = nullptr;
    }

    State(const State &state, std::vector<int> &&values, std::vector<ap_float> &&num_vals)
        : task(state.task), values(std::move(values)), num_values(std::move(num_vals)) {
            //std::cout << "state " << size() << " " << this->task->get_num_variables() << std::endl;
        assert(static_cast<int>(size()) == this->task->get_num_variables());
       if (DEBUG)
            std::cout << "num_values.size = " << num_values.size() << " task.get_num_numeric_vars = " << this->task->get_num_numeric_variables() << std::endl;
        assert(static_cast<int>(num_values.size()) == this->task->get_num_numeric_variables());
    }

    State &operator=(const State &&other) {
        if (this != &other) {
            values = std::move(other.values);
        }
        return *this;
    }

    bool operator==(const State &other) const {
        assert(task == other.task);
        return values == other.values;
    }

    bool operator!=(const State &other) const {
        return !(*this == other);
    }

    std::size_t hash() const {
        std::hash<std::vector<int>> hasher;
        return hasher(values);
    }

    std::size_t size() const {
        return values.size();
    }

    FactProxy operator[](std::size_t var_id) const {
        assert(var_id < size());
        return FactProxy(*task, var_id, values[var_id]);
    }

    FactProxy operator[](VariableProxy var) const {
        return (*this)[var.get_id()];
    }

    ap_float nval(std::size_t var_id) const {
    	assert(var_id < num_values.size());
    	return num_values[var_id];
    }

    State get_successor(OperatorProxy op) const {
        if (task->get_num_axioms() > 0) {
            ABORT("State::apply currently does not support axioms.");
        }
//        assert(!op.is_axiom());
        //assert(is_applicable(op, state));
        std::vector<int> new_values = values;
        for (EffectProxy effect : op.get_effects()) {
            if (does_fire(effect, *this)) {
                FactProxy effect_fact = effect.get_fact();
                new_values[effect_fact.get_variable().get_id()] = effect_fact.get_value();
            }
        }
        std::vector<ap_float> new_num_values = num_values;
        if (task->get_num_numeric_variables() > 0) {
        	ABORT("State::apply currently does not support numeric variables.");
        }
        return State(*task, std::move(new_values), std::move(new_num_values));
    }
};


namespace std {
template<>
struct hash<State> {
    size_t operator()(const State &state) const {
        return state.hash();
    }
};
}


class TaskProxy {
    const AbstractTask *task;
public:
    explicit TaskProxy(const AbstractTask &task)
        : task(&task) {}
    ~TaskProxy() = default;

    VariablesProxy get_variables() const {
        return VariablesProxy(*task);
    }

    NumericVariablesProxy get_numeric_variables() const {
        return NumericVariablesProxy(*task);
    }


    OperatorsProxy get_operators() const {
        return OperatorsProxy(*task);
    }

    AxiomsProxy get_axioms() const {
        return AxiomsProxy(*task);
    }

    ComparisonAxiomsProxy get_comparison_axioms() const {
    	return ComparisonAxiomsProxy(*task);
    }

    AssignmentAxiomsProxy get_assignment_axioms() const {
    	return AssignmentAxiomsProxy(*task);
    }

    GoalsProxy get_goals() const {
        return GoalsProxy(*task);
    }

    State get_initial_state() const {
//    	if(DEBUG) std::cout << "task proxy returning initial state " << std::endl;
        return State(*task, task->get_initial_state_values(), task->get_initial_state_numeric_values());
    }

    State convert_global_state(const GlobalState &global_state) const {
//    	if(DEBUG) std::cout << "task proxy returning converted global state " << std::endl;
        return State(*task, task->get_state_values(global_state), task->get_numeric_state_values(global_state));
    }

    const CausalGraph &get_causal_graph() const;
};


inline FactProxy::FactProxy(const AbstractTask &task, const Fact &fact)
    : task(&task), fact(fact) {
    assert(fact.var >= 0 && fact.var < task.get_num_variables());
    assert(fact.value >= 0 && fact.value < get_variable().get_domain_size());
}

inline FactProxy::FactProxy(const AbstractTask &task, int var_id, int value)
    : FactProxy(task, Fact(var_id, value)) {
}

inline NumAssProxy::NumAssProxy(const AbstractTask &task, int aff_var_id, f_operator op_type, int ass_var_id)
    : task(&task), aff_var_id(aff_var_id), op_type(op_type), ass_var_id(ass_var_id) {
    assert(aff_var_id >= 0 && aff_var_id < task.get_num_numeric_variables());
    assert(ass_var_id >= 0 && ass_var_id < task.get_num_numeric_variables());
}

inline VariableProxy FactProxy::get_variable() const {
    return VariableProxy(*task, fact.var);
}

inline NumericVariableProxy NumAssProxy::get_affected_variable() const {
    return NumericVariableProxy(*task, aff_var_id);
}

inline f_operator NumAssProxy::get_assigment_operator_type() const {
	return op_type;
}

inline NumericVariableProxy NumAssProxy::get_assigned_variable() const {
    return NumericVariableProxy(*task, ass_var_id);
}

inline bool does_fire(EffectProxy effect, const State &state) {
    for (FactProxy condition : effect.get_conditions()) {
        if (state[condition.get_variable()] != condition)
            return false;
    }
    return true;
}

#endif
