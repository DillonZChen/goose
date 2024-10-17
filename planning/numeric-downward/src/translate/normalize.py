#! /usr/bin/env python
from __future__ import print_function

import copy
import pddl


import sys # Warning

class ConditionProxy(object):
    def clone_owner(self):
        clone = copy.copy(self)
        clone.owner = copy.copy(clone.owner)
        return clone

class PreconditionProxy(ConditionProxy):
    def __init__(self, action):
        self.owner = action
        self.condition = action.precondition
    def set(self, new_condition):
        self.owner.precondition = self.condition = new_condition
    def register_owner(self, task):
        task.actions.append(self.owner)
    def delete_owner(self, task):
        task.actions.remove(self.owner)
    def build_rules(self, rules, fluent_preds): #, modules):
        action = self.owner        
        rule_head = get_action_predicate(action)
        rule_body = condition_to_rule_body(action.parameters, self.condition)
        rules.append((rule_body, rule_head))
    def get_type_map(self):
        return self.owner.type_map

class EffectConditionProxy(ConditionProxy):
    def __init__(self, action, effect):
        self.action = action
        self.owner = effect
        self.condition = effect.condition
    def set(self, new_condition):
        self.owner.condition = self.condition = new_condition
    def register_owner(self, task):
        self.action.effects.append(self.owner)
    def delete_owner(self, task):
        self.action.effects.remove(self.owner)
    def build_rules(self, rules, fluent_preds): #, modules): 
        # NFD: numeric variables are not completely ignored during translation
        # however, it is assumed that if a numeric variable is assigned *any*
        # value, it can also have the value desired for the precondition.
        # Of course, this is an over approximation, but so is delete free reachability
        # that is used for grounding "reachable" facts

        # FIXME support for modules requires to add some code here, too
        
        effect = self.owner
        rule_head = effect.peffect
        fluent_head = None
        if not isinstance(rule_head,pddl.NegatedAtom): 
            # TODO: add treatment of modules in translation here
            if isinstance(rule_head,pddl.FunctionAssignment):
                fluent = rule_head.fluent
                rule_head = get_function_predicate(fluent)
                fluent_head = pddl.Atom(fluent,fluent.args)                                               
            rule_body = [get_action_predicate(self.action)]
            rule_body += condition_to_rule_body([], self.condition)
            if rule_head:
                rules.append((rule_body, rule_head))
            if fluent_head: 
                rules.append((rule_body, fluent_head))            
            
    def get_type_map(self):
        return self.action.type_map

class AxiomConditionProxy(ConditionProxy):
    def __init__(self, axiom):
        self.owner = axiom
        self.condition = axiom.condition
    def set(self, new_condition):
        self.owner.condition = self.condition = new_condition
    def register_owner(self, task):
        task.axioms.append(self.owner)
    def delete_owner(self, task):
        task.axioms.remove(self.owner)
    def build_rules(self, rules, fluent_preds):
        axiom = self.owner
        app_rule_head = get_axiom_predicate(axiom)
        app_rule_body = condition_to_rule_body(axiom.parameters, self.condition)
        rules.append((app_rule_body, app_rule_head))
        params = axiom.parameters[:axiom.num_external_parameters]
        eff_rule_head = pddl.Atom(axiom.name, [par.name for par in params])
        eff_rule_body = [app_rule_head]
        rules.append((eff_rule_body, eff_rule_head))
    def get_type_map(self):
        return self.owner.type_map

class GoalConditionProxy(ConditionProxy):
    def __init__(self, task):
        self.owner = task
        self.condition = task.goal
    def set(self, new_condition):
        self.owner.goal = self.condition = new_condition
    def register_owner(self, task):
        # this assertion should never trigger, because disjunctive
        # goals are now implemented with axioms
        # (see substitute_complicated_goal)
        assert False, "Disjunctive goals not (yet) implemented."
    def delete_owner(self, task):
        # this assertion should never trigger, because disjunctive
        # goals are now implemented with axioms
        # (see substitute_complicated_goal)
        assert False, "Disjunctive goals not (yet) implemented."
    def build_rules(self, rules, fluent_preds):
        rule_head = pddl.Atom("@goal-reachable", [])
        rule_body = condition_to_rule_body([], self.condition)
        rules.append((rule_body, rule_head))
    def get_type_map(self):
        # HACK!
        # Method uniquify_variables HAS already been called (which is good).
        # We call it here again for its SIDE EFFECT of collecting the type_map
        # (which is bad). Having "top-level conditions" (currently, only goal
        # conditions, but might also include safety conditions and similar)
        # contained in a separate wrapper class that stores a type map might
        # be a better design.
        type_map = {}
        self.condition.uniquify_variables(type_map)
        return type_map

def get_action_predicate(action):
    name = action
    variables = [par.name for par in action.parameters]
    if isinstance(action.precondition, pddl.ExistentialCondition):
        variables += [par.name for par in action.precondition.parameters]
    return pddl.Atom(name, variables)

def get_axiom_predicate(axiom):
    name = axiom
    variables = [par.name for par in axiom.parameters]
    if isinstance(axiom.condition, pddl.ExistentialCondition):
        variables += [par.name for par in axiom.condition.parameters]
    return pddl.Atom(name, variables)

def get_fluent_predicates(task):
    fluent_predicates = set()
    for action in task.actions:
        for effect in action.effects:
            if isinstance(effect.peffect,pddl.Literal):
                fluent_predicates.add(effect.peffect.predicate)
            else:    
                predicate = get_function_predicate(effect.peffect.fluent).predicate
                fluent_predicates.add(predicate)
    for axiom in task.axioms:
        fluent_predicates.add(axiom.name)
    return fluent_predicates 

def all_conditions(task):
    for action in task.actions:
#        print(" all_conditions yields conditions of action %s" % action)
#        print("  yielding precondition %s" % PreconditionProxy(action))
        yield PreconditionProxy(action)
        for effect in action.effects: # NFD: also includes numeric cost effect as regular increas effect        
            yield EffectConditionProxy(action, effect)                                     
    for axiom in task.axioms:
        yield AxiomConditionProxy(axiom)
    yield GoalConditionProxy(task)

# [1] Remove universal quantifications from conditions.
#
# Replace, in a top-down fashion, <forall(vars, phi)> by <not(not-all-phi)>,
# where <not-all-phi> is a new axiom.
#
# <not-all-phi> is defined as <not(forall(vars,phi))>, which is of course
# translated to NNF. The parameters of the new axioms are exactly the free
# variables of <forall(vars, phi)>.

def remove_universal_quantifiers(task):
    def recurse(condition):
        # Uses new_axioms_by_condition and type_map from surrounding scope.
        if isinstance(condition, pddl.UniversalCondition):
            axiom_condition = condition.negate()
            parameters = sorted(axiom_condition.free_variables())
            axiom = new_axioms_by_condition.get(axiom_condition)
            if not axiom:
                typed_parameters = [pddl.TypedObject(v, type_map[v]) for v in parameters]
                condition = recurse(axiom_condition)
                axiom = task.add_axiom(typed_parameters, condition)
                new_axioms_by_condition[condition] = axiom
            return pddl.NegatedAtom(axiom.name, parameters)
        else:
            new_parts = [recurse(part) for part in condition.parts]
            return condition.change_parts(new_parts)

    new_axioms_by_condition = {}
    for proxy in tuple(all_conditions(task)):
        # Cannot use generator because we add new axioms on the fly.
        if proxy.condition.has_universal_part():
            type_map = proxy.get_type_map()
            proxy.set(recurse(proxy.condition))


# [2] Pull disjunctions to the root of the condition.
#
# After removing universal quantifiers, the (k-ary generalization of the)
# following rules suffice for doing that:
# (1) or(phi, or(psi, psi'))      ==  or(phi, psi, psi')
# (2) exists(vars, or(phi, psi))  ==  or(exists(vars, phi), exists(vars, psi))
# (3) and(phi, or(psi, psi'))     ==  or(and(phi, psi), and(phi, psi'))
def build_DNF(task):
    def recurse(condition):
        disjunctive_parts = []
        other_parts = []
        for part in condition.parts:
            part = recurse(part)
            if isinstance(part, pddl.Disjunction):
                disjunctive_parts.append(part)
            else:
                other_parts.append(part)
        if not disjunctive_parts:
            return condition

        # Rule (1): Associativity of disjunction.
        if isinstance(condition, pddl.Disjunction):
            result_parts = other_parts
            for part in disjunctive_parts:
                result_parts.extend(part.parts)
            return pddl.Disjunction(result_parts)

        # Rule (2): Distributivity disjunction/existential quantification.
        if isinstance(condition, pddl.ExistentialCondition):
            parameters = condition.parameters
            result_parts = [pddl.ExistentialCondition(parameters, (part,))
                            for part in disjunctive_parts[0].parts]
            return pddl.Disjunction(result_parts)

        # Rule (3): Distributivity disjunction/conjunction.
        assert isinstance(condition, pddl.Conjunction)
        result_parts = [pddl.Conjunction(other_parts)]
        while disjunctive_parts:
            previous_result_parts = result_parts
            result_parts = []
            parts_to_distribute = disjunctive_parts.pop().parts
            for part1 in previous_result_parts:
                for part2 in parts_to_distribute:
                    result_parts.append(pddl.Conjunction((part1, part2)))
        return pddl.Disjunction(result_parts)

    for proxy in all_conditions(task):
        if proxy.condition.has_disjunction():
            proxy.set(recurse(proxy.condition).simplified())

# [3] Split conditions at the outermost disjunction.
def split_disjunctions(task):
    for proxy in tuple(all_conditions(task)):
        # Cannot use generator directly because we add/delete entries.
        if isinstance(proxy.condition, pddl.Disjunction):
            for part in proxy.condition.parts:
                new_proxy = proxy.clone_owner()
                new_proxy.set(part)
                new_proxy.register_owner(task)
            proxy.delete_owner(task)

# [4] Pull existential quantifiers out of conjunctions and group them.
#
# After removing universal quantifiers and creating the disjunctive form,
# only the following (representatives of) rules are needed:
# (1) exists(vars, exists(vars', phi))  ==  exists(vars + vars', phi)
# (2) and(phi, exists(vars, psi))       ==  exists(vars, and(phi, psi)),
#       if var does not occur in phi as a free variable.
def move_existential_quantifiers(task):
    def recurse(condition):
        existential_parts = []
        other_parts = []
        for part in condition.parts:
            part = recurse(part)
            if isinstance(part, pddl.ExistentialCondition):
                existential_parts.append(part)
            else:
                other_parts.append(part)
        if not existential_parts:
            return condition

        # Rule (1): Combine nested quantifiers.
        if isinstance(condition, pddl.ExistentialCondition):
            new_parameters = condition.parameters + existential_parts[0].parameters
            new_parts = existential_parts[0].parts
            return pddl.ExistentialCondition(new_parameters, new_parts)

        # Rule (2): Pull quantifiers out of conjunctions.
        assert isinstance(condition, pddl.Conjunction)
        new_parameters = []
        new_conjunction_parts = other_parts
        for part in existential_parts:
            new_parameters += part.parameters
            new_conjunction_parts += part.parts
        new_conjunction = pddl.Conjunction(new_conjunction_parts)
        return pddl.ExistentialCondition(new_parameters, (new_conjunction,))

    for proxy in all_conditions(task):
        if proxy.condition.has_existential_part():
            proxy.set(recurse(proxy.condition).simplified())


# [5a] Drop existential quantifiers from axioms, turning them
#      into parameters.

def eliminate_existential_quantifiers_from_axioms(task):
    # Note: This is very redundant with the corresponding method for
    # actions and could easily be merged if axioms and actions were
    # unified.
    for axiom in task.axioms:
        precond = axiom.condition
        if isinstance(precond, pddl.ExistentialCondition):
            # Copy parameter list, since it can be shared with
            # parameter lists of other versions of this axiom (e.g.
            # created when splitting up disjunctive preconditions).
            axiom.parameters = list(axiom.parameters)
            axiom.parameters.extend(precond.parameters)
            axiom.condition = precond.parts[0]


# [5b] Drop existential quantifiers from action preconditions,
#      turning them into action parameters (that don't form part of the
#      name of the action).

def eliminate_existential_quantifiers_from_preconditions(task):
    for action in task.actions:
        precond = action.precondition
        if isinstance(precond, pddl.ExistentialCondition):
            # Copy parameter list, since it can be shared with
            # parameter lists of other versions of this action (e.g.
            # created when splitting up disjunctive preconditions).
            action.parameters = list(action.parameters)
            action.parameters.extend(precond.parameters)
            action.precondition = precond.parts[0]

# [5c] Eliminate existential quantifiers from effect conditions
#
# For effect conditions, we replace "when exists(x, phi) then e" with
# "forall(x): when phi then e.
def eliminate_existential_quantifiers_from_conditional_effects(task):
    for action in task.actions:
        for effect in action.effects:
            condition = effect.condition
            if isinstance(condition, pddl.ExistentialCondition):
                effect.parameters = list(effect.parameters)
                effect.parameters.extend(condition.parameters)
                effect.condition = condition.parts[0]

def remove_arithmetic_expressions(task):
    def recurse(condition):
        if isinstance(condition, pddl.FunctionComparison):
            parts = [task.function_administrator.get_derived_function(exp)
                        for exp in condition.parts]
            if condition.negated:
                return pddl.NegatedFunctionComparison(condition.comparator,parts)
            else:
                return pddl.FunctionComparison(condition.comparator,parts)
        else:
            new_parts = [recurse(part) for part in condition.parts]
            return condition.change_parts(new_parts)

    # remove from conditions
    for proxy in tuple(all_conditions(task)):
        if isinstance(proxy.condition,list):
            condition = []
            for cond in proxy.condition:
                condition.append(recurse(cond))
            proxy.set(condition)
        else:
            proxy.set(recurse(proxy.condition))

    # remove from actions
    admin = task.function_administrator
    for act in task.actions:
        for eff in act.effects:
            if isinstance(eff.peffect,pddl.FunctionAssignment):
                assign = eff.peffect
                assign.expression = admin.get_derived_function(assign.expression)
    
    # remove from metric expression 
    opt_dir, metric_expression = task.metric
    if metric_expression is None: # unit cost -> no metric specified in task        
        assert opt_dir == '<', "No metric expression specified in task, but 'maximize' found"
        metric_expression = -1
    else:
        metric_expression = admin.get_derived_function(metric_expression)
    task.metric = (opt_dir, metric_expression)
                

def substitute_complicated_goal(task):
    goal = task.goal
    if isinstance(goal, pddl.Literal):
        return
    elif isinstance(goal, pddl.Conjunction):
        for item in goal.parts:
            if not isinstance(item, pddl.Literal):
                break
        else:
            return
    new_axiom = task.add_axiom([], goal)
    task.goal = pddl.Atom(new_axiom.name, new_axiom.parameters)

# Combine Steps [1], [2], [3], [4], [5] and do some additional verification
# that the task makes sense.

def normalize(task):
    remove_universal_quantifiers(task)
    substitute_complicated_goal(task)
    build_DNF(task)
    split_disjunctions(task)
    move_existential_quantifiers(task)
    eliminate_existential_quantifiers_from_axioms(task)
    eliminate_existential_quantifiers_from_preconditions(task)
    eliminate_existential_quantifiers_from_conditional_effects(task)
    verify_and_fix_arithmetic_expressions(task) # fix errors from invalid PDDL specifications
    remove_arithmetic_expressions(task) # replace complicated expressions with their derived variable
    verify_axiom_predicates(task)

def verify_and_fix_arithmetic_expressions(task):
    function_names = [fun.name for fun in task.functions]
#     print ("task functions are:")
#     for fun in task.functions:
#         print (fun)
#     print ("task num init is:")
#     for nin in task.num_init:
#         print ("%s of class %s hc %s, tc %s" %(nin, nin.__class__, nin.fluent.__class__, nin.expression.__class__))
        

    def verify_expression(aexp):
#         print("Verifying ",str(aexp))        
        if isinstance(aexp,pddl.PrimitiveNumericExpression):
#             print("- it is a PNE with fluent",aexp.symbol)
            if not aexp.symbol in function_names:
                msg = ("WARNING: Function symbol '%s' appears in a numeric expression but is not defined in domain file.\n"
                       "Adding new symbol '%s' with arity %d.") % (aexp.symbol, aexp.symbol, len(aexp.args))
                print(msg, file=sys.stderr)
                if len(aexp.args) == 0:                                
                    newfunction = pddl.Function(aexp.symbol, [], "number")                    
                    task.functions.append(newfunction)
                    initassignment = pddl.Assign(aexp, pddl.NumericConstant(0.0))
                    task.num_init.append(initassignment)
                else: # if this occurs in an actual domain, maybe we could also determine the type of the parameters of higher-arity functions
                    raise Error("Don't know the parameters of function %s with arity %d" % (aexp.symbol, len(aexp.args)))
                    assert False            
#         elif isinstance(aexp, pddl.NumericConstant):     
#             print("- it is a constant: ",aexp.value)
        elif isinstance(aexp,pddl.AdditiveInverse):
#             print("- it is an additive inverse: ",aexp)
            assert len(parts)==1
            verify_expression(aexp.parts[0])                           
        elif isinstance(aexp,pddl.ArithmeticExpression):
#             print("- it is an arithmetic expression: ",aexp)
            for part in aexp.parts:
                verify_expression(part)
            
    admin = task.function_administrator
    #conditions
    # TODO: check conditions
#     for cond_proxy in all_conditions(task):
#         # do something
#         print("DEBUG: missing check for condition",cond_proxy)
    #actions
    for act in task.actions:
        for eff in act.effects:
            if isinstance(eff.peffect,pddl.FunctionAssignment):
                verify_expression(eff.peffect.expression)                

    #metric
    if (task.metric[1]):
        verify_expression(task.metric[1])

def verify_axiom_predicates(task):
    # Verify that derived predicates are not used in :init or
    # action effects.
    axiom_names = set()
    for axiom in task.axioms:
        axiom_names.add(axiom.name)

    for fact in task.init:
        # Note that task.init can contain the assignment to (total-cost)
        # in addition to regular atoms.
        if getattr(fact, "predicate", None) in axiom_names:
            raise SystemExit(
                "error: derived predicate %r appears in :init fact '%s'" %
                (fact.predicate, fact))

    for action in task.actions:
        for effect in action.effects:
            if isinstance(effect.peffect, pddl.Literal) and effect.peffect.predicate in axiom_names:
                raise SystemExit(
                    "error: derived predicate %r appears in effect of action %r" %
                    (effect.peffect.predicate, action.name))

# [6] Build rules for exploration component.
def build_exploration_rules(task):
    result = []        
    fluent_preds = get_fluent_predicates(task)
    for proxy in all_conditions(task):     
        proxy.build_rules(result, fluent_preds) #, task.modules)

    for axiom in task.function_administrator.get_all_axioms():
    # add rules to determine defined functions
        rule_head = get_function_axiom_predicate(axiom)
        rule_body = []
        for part in axiom.parts:
            if isinstance(part,pddl.PrimitiveNumericExpression):
                rule_body.append(get_function_predicate(part))
            
        result.append((rule_body, rule_head))
        rule_body = [rule_head]
        rule_head = get_function_predicate(axiom.get_head())
        result.append((rule_body, rule_head))
        # add rule to determine fluent functions
        rule_head = get_fluent_function_predicate(axiom.get_head())
        for part in axiom.parts:
            if isinstance(part,pddl.PrimitiveNumericExpression):
                new_rule_body = rule_body + [get_fluent_function_predicate(part)]
                result.append((new_rule_body, rule_head))
    return result

def condition_to_rule_body(parameters, condition):
    result = []
    for par in parameters:
        result.append(par.get_atom())
    if not isinstance(condition, pddl.Truth):
        if isinstance(condition, pddl.ExistentialCondition):
            for par in condition.parameters:
                result.append(par.get_atom())
            condition = condition.parts[0]
        if isinstance(condition, pddl.Conjunction):
            parts = condition.parts
        else:
            parts = (condition,)
        for part in parts:
            assert isinstance(part, pddl.Literal) or isinstance(part, pddl.FunctionComparison), "Condition not normalized (part of class %s)" % part.__class__
            if isinstance(part, pddl.Literal):

#            if isinstance(part, pddl.Falsity):
#                # Use an atom in the body that is always false because
#                # it is not initially true and doesn't occur in the
#                # head of any rule.
#                return [pddl.Atom("@always-false", [])]
#            assert isinstance(part, pddl.Literal), "Condition not normalized: %r" % part

                if not part.negated: 
                    result.append(part)                    
            else:
                primitives = part.primitive_numeric_expressions()             
                for pne in primitives:
                    result.append(get_function_predicate(pne))
    return result



def get_function_predicate(pne):
    name = "defined!%s" % pne.symbol
    return pddl.Atom(name, pne.args)

def get_fluent_function_predicate(pne):
    return pddl.Atom(pne,pne.args)

def get_function_axiom_predicate(axiom): 
    name = axiom
    args = axiom.parameters
    for part in axiom.parts:
        if isinstance(part, pddl.PrimitiveNumericExpression):
            args += part.args
        elif isinstance(part, pddl.NumericAxiom):
            args += part.parameters
    return pddl.Atom(name, args)


if __name__ == "__main__":
    import pddl_parser
    task = pddl_parser.open()
    normalize(task)
    task.dump()
