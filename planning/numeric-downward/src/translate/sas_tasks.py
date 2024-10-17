from __future__ import print_function

SAS_FILE_VERSION = 4

DEBUG = False


class SASTask:
    """Planning task in finite-domain representation.

    The user is responsible for making sure that the data fits a
    number of structural restrictions. For example, conditions should
    generally be sorted and mention each variable at most once. See
    the validate methods for details."""

    def __init__(self, variables, numeric_variables, mutexes, init, goal, 
                 operators, axioms, comp_axioms, numeric_axioms, 
                 global_constraint, metric, init_constant_predicates, 
                 init_constant_numerics):
        self.variables = variables
        self.numeric_variables = numeric_variables
        self.mutexes = mutexes
        self.init = init
        self.goal = goal
        self.operators = sorted(operators, key=lambda op: (
            op.name, op.prevail, op.pre_post))
        self.axioms = sorted(axioms, key=lambda axiom: (
            axiom.condition, axiom.effect))
        self.comp_axioms = comp_axioms
        self.numeric_axioms = numeric_axioms
        self.global_constraint = global_constraint # the global constraint is a var value pair of the global constraint derived axiom variable
        self.metric = metric # tuple containing '<' : minimize or '>' : maximize and the index of the (numeric) metric variable
                             # (in classic domains it will be always ('<', 0) where 0 is the index of the only numeric variable "total-cost"
                             # or ('<', -1) for unit cost domains (-1 is used as special index to indicate that no numeric cost fluent is used)                
        self.init_constant_predicates = init_constant_predicates
        self.init_constant_numerics = init_constant_numerics        
        if DEBUG:
            self.validate()

    def validate(self):
        """Fail an assertion if the task is invalid.

        A task is valid if all its components are valid. Valid tasks
        are almost in a kind of "canonical form", but not quite. For
        example, operators and axioms are permitted to be listed in
        any order, even though it would be possible to require some
        kind of canonical sorting.

        Note that we require that all derived variables are binary.
        This is stricter than what later parts of the planner are
        supposed to handle, but some parts of the translator rely on
        this. We might want to consider making this a general
        requirement throughout the planner.

        Note also that there is *no* general rule on what the init (=
        fallback) value of a derived variable is. For example, in
        PSR-Large #1, it can be either 0 or 1. While it is "usually"
        1, code should not rely on this.
        """
        self.variables.validate()
        for mutex in self.mutexes:
            mutex.validate(self.variables)
        self.init.validate(self.variables)
        self.goal.validate(self.variables)
        for op in self.operators:
            op.validate(self.variables)
        for axiom in self.axioms:
            axiom.validate(self.variables, self.init)
        assert self.metric[0] in ['<', '>']
        assert self.metric[1]
        assert isinstance(self.global_constraint, tuple), "%s of type %s" % (self.global_constraint, type(self.global_constraint))
        (gcv, zero) = self.global_constraint
        assert zero == 0

    def dump(self):
        print("variables:")
        self.variables.dump()
        print("%d mutex groups:" % len(self.mutexes))
        for mutex in self.mutexes:
            print("group:")
            mutex.dump()
        print("init:")
        self.init.dump()
        print("goal:")
        self.goal.dump()
        print("%d operators:" % len(self.operators))
        for operator in self.operators:
            operator.dump()
        print("%d axioms:" % len(self.axioms))
        for axiom in self.axioms:
            axiom.dump()
        print("metric: %s" % self.metric)

    def output(self, stream):
        print("begin_version", file=stream)
        print(SAS_FILE_VERSION, file=stream)
        print("end_version", file=stream)
        print("begin_metric", file=stream)
        print(self.metric[0], int(self.metric[1]), file=stream)
        print("end_metric", file=stream)
        self.variables.output(stream)
        self.numeric_variables.output(stream)        
        print(len(self.mutexes), file=stream)
        for mutex in self.mutexes:
            mutex.output(stream)
        self.init.output(stream)
        self.goal.output(stream)
        print(len(self.operators), file=stream)
        for op in self.operators:
            op.output(stream)
        print(len(self.axioms), file=stream)
        for axiom in self.axioms:
            axiom.output(stream)
        print(len(self.comp_axioms), file=stream)
        print("begin_comparison_axioms", file=stream)
        for cax in self.comp_axioms:
            cax.output(stream)
        print("end_comparison_axioms", file=stream)            
        print(len(self.numeric_axioms), file=stream)
        print("begin_numeric_axioms", file=stream)
        for nax in self.numeric_axioms:
            nax.output(stream) 
        print("end_numeric_axioms", file=stream)
        print("begin_global_constraint", file=stream)             
        print("%s %s"%self.global_constraint, file=stream)
        print("end_global_constraint", file=stream)        
#        print("DEBUG: constant numerics %s " % self.init_constant_numerics)

    def get_encoding_size(self):
        task_size = 0
        task_size += self.variables.get_encoding_size()
        for mutex in self.mutexes:
            task_size += mutex.get_encoding_size()
        task_size += self.goal.get_encoding_size()
        for op in self.operators:
            task_size += op.get_encoding_size()
        for axiom in self.axioms:
            task_size += axiom.get_encoding_size()
        for cax in self.comp_axioms:
            task_size += cax.get_encoding_size()
        for nax in self.numeric_axioms:
            task_size += nax.get_encoding_size()
        return task_size


class SASVariables:
    def __init__(self, ranges, axiom_layers, value_names, comp_axiom_layer):        
        self.ranges = ranges
        self.axiom_layers = axiom_layers
        self.value_names = value_names
        self.comp_axiom_layer = comp_axiom_layer # id of the comparison axiom layer -> needed to validate that logic axioms can only have 2 values

    def validate(self):
        """Validate variables.

        All variables must have range at least 2, and derived
        variables must have range exactly 2. See comment on derived
        variables in the docstring of SASTask.validate.
        """
        assert len(self.ranges) == len(self.axiom_layers) == len(
            self.value_names)
        for (var_range, layer, var_value_names) in zip(
                self.ranges, self.axiom_layers, self.value_names):
            assert var_range == len(var_value_names)
            assert var_range >= 2
            assert layer == -1 or layer >= 0
            if layer > self.comp_axiom_layer: # logic axiom                 
                assert var_range == 2

    def validate_fact(self, fact):
        """Assert that fact is a valid (var, value) pair."""
        var, value = fact
        assert 0 <= var < len(self.ranges)
        assert 0 <= value < self.ranges[var], "value= %d, self.ranges[var]=%d" % (value, self.ranges[var])

    def validate_condition(self, condition):
        """Assert that the condition (list of facts) is sorted, mentions each
        variable at most once, and only consists of valid facts."""
        last_var = -1
        for (var, value) in condition:
            self.validate_fact((var, value))
            assert var > last_var
            last_var = var

    def dump(self):
        for var, (rang, names, axiom_layer) in enumerate(
                zip(self.ranges, self.value_names, self.axiom_layers)):
            if axiom_layer != -1:
                axiom_str = " [axiom layer %d]" % axiom_layer
            else:
                axiom_str = ""
            print("v%d in {%s}%s" % (var, list("%d:%s"% t for t in zip(range(rang),names)), axiom_str))

    def output(self, stream):
        print(len(self.ranges), file=stream)
        for var, (rang, axiom_layer, values) in enumerate(zip(
                self.ranges, self.axiom_layers, self.value_names)):
            print("begin_variable", file=stream)
            print("var%d" % var, file=stream)
            print(axiom_layer, file=stream)
            print(rang, file=stream)
            assert rang == len(values), (rang, values)
            for value in values:
                print(value, file=stream)
            print("end_variable", file=stream)

    def get_encoding_size(self):
        # A variable with range k has encoding size k + 1 to also give the
        # variable itself some weight.
        return len(self.ranges) + sum(self.ranges)

class SASNumericVariables:
    def __init__(self, variable_names, axiom_layers, types):        
        self.variable_names = variable_names
        self.axiom_layers = axiom_layers
        self.types = types
    def dump(self):
        v = 0
        for nv in self.variable_names:            
            print("numv%d: %s" % (v, nv))
            v+=1
    def output(self, stream):
        print(len(self.variable_names), file=stream)
        print("begin_numeric_variables", file=stream)
        v = 0        
        for nv in self.variable_names:            
            print("%s %d %s" % (self.types[v], self.axiom_layers[v], nv), file=stream)
            v+=1
        print("end_numeric_variables", file=stream)

        
class SASMutexGroup:
    def __init__(self, facts):
        self.facts = sorted(facts)

    def validate(self, variables):
        """Assert that the facts in the mutex group are sorted and unique
        and that they are all valid."""
        for fact in self.facts:
            variables.validate_fact(fact)
        assert self.facts == sorted(set(self.facts))

    def dump(self):
        for var, val in self.facts:
            print("v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_mutex_group", file=stream)
        print(len(self.facts), file=stream)
        for var, val in self.facts:
            print(var, val, file=stream)
        print("end_mutex_group", file=stream)

    def get_encoding_size(self):
        return len(self.facts)


class SASInit:
    def __init__(self, values, numeric_values):
        self.values = values
        self.num_values = numeric_values
                   
    def validate(self, variables):
        """Validate initial state.

        Assert that the initial state contains the correct number of
        values and that all values are in range.
        """

        assert len(self.values) == len(variables.ranges), "length of values= %s length of varranges= %s" % (len(self.values), len(variables.ranges))
        for fact in enumerate(self.values):
            variables.validate_fact(fact)

    def dump(self):
        for var, val in enumerate(self.values):
            if val != -1:
                print("v%d: %d" % (var, val))
        for var, val in enumerate(self.num_values):
            if val != -1:
                print("nv%d: %f" % (var, val))

    def output(self, stream):
        print("begin_state", file=stream)
        for val in self.values:
            print(val, file=stream)
        print("end_state", file=stream)
        print("begin_numeric_state", file=stream)
        for val in self.num_values:
            print(val, file=stream)
        print("end_numeric_state", file=stream)


class SASGoal:
    def __init__(self, pairs):
        self.pairs = sorted(pairs)

    def validate(self, variables):
        """Assert that the goal is nonempty and a valid condition."""
        assert self.pairs
        variables.validate_condition(self.pairs)

    def dump(self):
        for var, val in self.pairs:
            print("v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_goal", file=stream)
        print(len(self.pairs), file=stream)
        for var, val in self.pairs:
            print(var, val, file=stream)
        print("end_goal", file=stream)

    def get_encoding_size(self):
        return len(self.pairs)


class SASOperator:
    def __init__(self, name, prevail, pre_post, assign_effect, cost):
        self.name = name
        self.prevail = sorted(prevail)
        self.pre_post = self._canonical_pre_post(pre_post)
        self.assign_effects = sorted(assign_effect)
        self.cost = cost

    def _canonical_pre_post(self, pre_post):
        # Return a sorted and uniquified version of pre_post. We would
        # like to just use sorted(set(pre_post)), but this fails because
        # the effect conditions are a list and hence not hashable.
        def tuplify(entry):
            var, pre, post, cond = entry
            return var, pre, post, tuple(cond)
        def listify(entry):
            var, pre, post, cond = entry
            return var, pre, post, list(cond)
        pre_post = map(tuplify, pre_post)
        pre_post = sorted(set(pre_post))
        pre_post = list(map(listify, pre_post))
        return pre_post

    def validate(self, variables):
        """Validate the operator.

        Assert that
        1. Prevail conditions are valid conditions (i.e., sorted and
           all referring to different variables)
        2. The pre_post list is sorted by (var, pre, post, cond), and the
           same (var, pre, post, cond) 4-tuple is not repeated.
        3. Effect conditions are valid conditions and do not contain variables
           from the pre- or prevail conditions.
        4. Variables occurring in pre_post rules do not have a prevail
           condition.
        5. Preconditions in pre_post are -1 or valid facts.
        6. Effects are valid facts.
        7. Effect variables are non-derived.
        8. If a variable has multiple pre_post rules, then pre is
           identical in all these rules.
        9. There is at least one effect.
        10. Costs are non-negative integers.

        Odd things that are *not* illegal:
        - Effect conditions that contradict a precondition or prevail
          condition are permitted.
        - The effect in a pre_post rule may be identical to the
          precondition or to an effect condition of that effect.

        TODO/open question:
        - It is currently not very clear what the semantics of operators
          should be when effects "conflict", i.e., when multiple effects
          trigger and want to set a given variable to two different
          values. In the case where both are unconditional effects, we
          should make sure that our representation doesn't actually
          contain two such effects, but when at least one of them is
          conditional, things are not so easy.

          To make our life simpler when generating SAS+ tasks from
          PDDL tasks, it probably makes most sense to generalize the
          PDDL rule in this case: there is a value order where certain
          values "win" over others in this situation. It probably
          makes sense to say the "highest" values should win in this
          case, because that's consistent with the PDDL rules if we
          say false = 0 and true = 1, and also with our sort order of
          effects it means we get the right result if we just apply
          effects in sequence.

          But whatever we end up deciding, we need to be clear about it,
          document it and make sure that all of our code knows the rules
          and follows them.
        """

        variables.validate_condition(self.prevail)
        assert self.pre_post == self._canonical_pre_post(self.pre_post)
        prevail_vars = set(var for (var, value) in self.prevail)
        pre_values = {}
        for var, pre, post, cond in self.pre_post:
            variables.validate_condition(cond)
            assert var not in prevail_vars
            if pre != -1:
                variables.validate_fact((var, pre))
            variables.validate_fact((var, post))
            assert variables.axiom_layers[var] == -1
            if var in pre_values:
                assert pre_values[var] == pre
            else:
                pre_values[var] = pre
        for var, pre, post, cond in self.pre_post:
            for cvar, cval in cond:
                assert(cvar not in pre_values or pre_values[cvar] == -1)
                assert(cvar not in prevail_vars)
        if not self.pre_post:            
            assert self.assign_effects

    def dump(self):
        print(self.name)
        print("Prevail:")
        for var, val in self.prevail:
            print("  v%d: %d" % (var, val))
        print("Pre/Post:")
        for var, pre, post, cond in self.pre_post:
            if cond:
                cond_str = " [%s]" % ", ".join(
                    ["%d: %d" % tuple(c) for c in cond])
            else:
                cond_str = ""
            print("  v%d: %d -> %d%s" % (var, pre, post, cond_str))
        for var, ass_op, ass_var, cond in self.assign_effects:
            if cond:
                cond_str = " [%s]" % ", ".join(["%d: %d" % tuple(c) for c in cond])
            else:
                cond_str = ""
            print("  nv%d: %s nv%d%s" % (var, ass_op, ass_var, cond_str)) # ass_op in {+-*/=}
        print("Cost:") # Cost stores the evaluated value of the metric expression (for state dependent action costs this field stores the expression evaluated in the initial state) 
        print("  %f" % self.cost)                

    def output(self, stream):
        print("begin_operator", file=stream)
        print(self.name[1:-1], file=stream)
        print(len(self.prevail), file=stream)
        for var, val in self.prevail:
            print(var, val, file=stream)
        print(len(self.pre_post), file=stream)
        for var, pre, post, cond in self.pre_post:
            print(len(cond), end=' ', file=stream)
            for cvar, cval in cond:
                print(cvar, cval, end=' ', file=stream)
            print(var, pre, post, file=stream)
        print(len(self.assign_effects), file=stream)
        for nvar, op, ass_var, cond in self.assign_effects:
            print(len(cond), end=' ', file=stream)
            for cvar, cval in cond:
                print(cvar, cval, end=' ', file=stream)
            print(nvar, op, ass_var, file=stream)
        print(self.cost, file=stream)
        print("end_operator", file=stream)

    def get_encoding_size(self):
        size = 1 + len(self.prevail)
        for var, pre, post, cond in self.pre_post:
            size += 1 + len(cond)
            if pre != -1:
                size += 1
        return size

    def get_applicability_conditions(self):
        """Return the combined applicability conditions
        (prevail conditions and preconditions) of the operator.

        Returns a sorted list of (var, value) pairs. This is
        guaranteed to contain at most one fact per variable and
        must hence be non-contradictory."""
        conditions = {}
        for var, val in self.prevail:
            assert var not in conditions
            conditions[var] = val
        for var, pre, post, cond in self.pre_post:
            if pre != -1:
                assert var not in conditions or conditions[var] == pre
                conditions[var] = pre
        return sorted(conditions.items())


class SASAxiom:
    def __init__(self, condition, effect):
        self.condition = sorted(condition)
        self.effect = effect
        assert self.effect[1] in (0, 1)

        for _, val in condition:
            assert val >= 0, condition

    def validate(self, variables, init):

        """Validate the axiom.

        Assert that the axiom condition is a valid condition, that the
        effect is a valid fact, that the effect variable is a derived
        variable, and that the layering condition is satisfied.

        See the docstring of SASTask.validate for information on the
        restriction on derived variables. The layering condition boils
        down to:

        1. Axioms always set the "non-init" value of the derived
           variable.
        2. Derived variables in the condition must have a lower of
           equal layer to derived variables appearing in the effect.
        3. Conditions with equal layer are only allowed when the
           condition uses the "non-init" value of that variable.

        TODO/bug: rule #1 is currently disabled because we currently
        have axioms that violate it. This is likely due to the
        "extended domain transition graphs" described in the Fast
        Downward paper, Section 5.1. However, we want to eventually
        changes this. See issue454. For cases where rule #1 is violated,
        "non-init" should be "init" in rule #3.
        """

        variables.validate_condition(self.condition)
        variables.validate_fact(self.effect)
        eff_var, eff_value = self.effect
        eff_layer = variables.axiom_layers[eff_var]
        assert eff_layer >= 0
        eff_init_value = init.values[eff_var]
        ## The following rule is currently commented out because of
        ## the TODO/bug mentioned in the docstring.
        # assert eff_value != eff_init_value
        for cond_var, cond_value in self.condition:
            cond_layer = variables.axiom_layers[cond_var]
            if cond_layer != -1:
                assert cond_layer <= eff_layer
                if cond_layer == eff_layer:
                    cond_init_value = init.values[cond_var]
                    ## Once the TODO/bug above is addressed, the
                    ## following four lines can be simplified because
                    ## we are guaranteed to land in the "if" branch.
                    if eff_value != eff_init_value:
                        assert cond_value != cond_init_value
                    else:
                        assert cond_value == cond_init_value

    def dump(self):
        print("Condition:")
        for var, val in self.condition:
            print("  v%d: %d" % (var, val))
        print("Effect:")
        var, val = self.effect
        print("  v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_rule", file=stream)
        print(len(self.condition), file=stream)
        for var, val in self.condition:
            print(var, val, file=stream)
        var, val = self.effect
        print(var, 1 - val, val, file=stream)
        print("end_rule", file=stream)

    def get_encoding_size(self):
        return 1 + len(self.condition)


class SASCompareAxiom:
    def invert_comparator(self):
        inv_comp={'>=': '<',
                  '<': '>=', 
                  '<=' : '>', 
                  '>' : '<=',
                  '=' : '!=',
                  '!=' : '='}                         
        return SASCompareAxiom(inv_comp[self.comp], self.parts, self.effect)
    def __init__(self, comp, parts, effect):
        self.comp = comp
        self.parts = parts
        self.effect = effect
    def __repr__(self):      
        return "%s %d %d" % (self.comp, self.parts[0], self.parts[1])
    def pretty_print(self, numeric_variables):
        print("CAX %s '%s' '%s'"%(self.comp, numeric_variables.variable_names[self.parts[0]],
                                  numeric_variables.variable_names[self.parts[1]]))#,variables.value_names[self.effect][0]))
    def dump(self):
        values = (self.effect, self.comp, 
                  " ".join([str(var) for var in self.parts]))
        print("v%d: %s %s" % values)
    def output(self, stream):
        values = (self.effect, self.comp, 
                  " ".join([str(var) for var in self.parts]))
        print("%d %s %s" % values, file=stream)
    def get_encoding_size(self):
        return 1 + len(self.parts)


class SASNumericAxiom: # takes an arithmetic operator ("+", "-", "*", "/") and 3 numeric fluents where the 3rd represents the computation of the first two.
    def __init__(self, op, parts, effect):
        self.op = op
        self.parts = parts
        self.effect = effect
    def dump(self):
        values = (self.effect, self.op, 
                  " ".join([str(var) for var in self.parts]))
        print("nv%d: %s %s" % values)

    def output(self, stream):
        values = (self.effect, self.op, 
                  " ".join([str(var) for var in self.parts]))
        print("%d %s %s" % values, file=stream)
    def get_encoding_size(self):
        return 1 + len(self.parts)
