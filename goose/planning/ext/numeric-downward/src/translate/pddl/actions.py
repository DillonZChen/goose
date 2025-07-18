from __future__ import print_function

import copy

from . import conditions
from . import effects
from effects import NumericEffect
from . import pddl_types
from . import f_expression


class Action(object):
    def __init__(self, name, parameters, num_external_parameters,
                 precondition, effects, cost):
        assert 0 <= num_external_parameters <= len(parameters)
        self.name = name
        self.parameters = parameters
        # num_external_parameters denotes how many of the parameters
        # are "external", i.e., should be part of the grounded action
        # name. Usually all parameters are external, but "invisible"
        # parameters can be created when compiling away existential
        # quantifiers in conditions.
        self.num_external_parameters = num_external_parameters
        self.precondition = precondition
        self.effects = effects
        assert(isinstance(cost, NumericEffect))
        self.cost = cost # cost is an effects.NumericEffect increase effect of fluent total-cost  
        self.uniquify_variables() # TODO: uniquify variables in cost?
    def __repr__(self):
        return "<Action %s %s>" % (self.name,map(str, self.parameters))
    def parse(alist):
        iterator = iter(alist)
        action_tag = next(iterator)
        assert action_tag == ":action"
        name = next(iterator)
        parameters_tag_opt = next(iterator)
        if parameters_tag_opt == ":parameters":
            parameters = pddl_types.parse_typed_list(next(iterator),
                                                     only_variables=True)
            precondition_tag_opt = next(iterator)
        else:
            parameters = []
            precondition_tag_opt = parameters_tag_opt
        if precondition_tag_opt == ":precondition":
            precondition_list = next(iterator)
            if not precondition_list:
                # Note that :precondition () is allowed in PDDL.
                precondition = conditions.Conjunction([])
            else:
                precondition = conditions.parse_condition(precondition_list)
                precondition = precondition.simplified()
            effect_tag = next(iterator)
        else:
            precondition = conditions.Conjunction([])
            effect_tag = precondition_tag_opt
        assert effect_tag == ":effect"
        effect_list = next(iterator)
        eff = []
        if effect_list:
            try:
                cost = effects.parse_effects(effect_list, eff)
            except ValueError as e:
                raise SystemExit("Error in Action %s\nReason: %s." % (name, e))
        for rest in iterator:
            assert False, rest
        if eff:
            return Action(name, parameters, len(parameters),
                          precondition, eff, cost)
        else:
            return None
    parse = staticmethod(parse)
    def dump(self):
        print("%s(%s)" % (self.name, ", ".join(map(str, self.parameters))))
        print("Precondition:")
        self.precondition.dump()
        print("Effects:")
        for eff in self.effects:
            eff.dump()
        print("Cost:")
        if(self.cost):
            self.cost.dump()
        else:
            print("  None")
    def uniquify_variables(self):
        self.type_map = dict([(par.name, par.type_name)
                              for par in self.parameters])
        self.precondition = self.precondition.uniquify_variables(self.type_map)
        for effect in self.effects:
#            print("actions.uniquify_variables uniquifies effect: ", effect  )
            effect.uniquify_variables(self.type_map)
    def relaxed(self):
        new_effects = []
        for eff in self.effects:
            relaxed_eff = eff.relaxed()
            if relaxed_eff:
                new_effects.append(relaxed_eff)
        return Action(self.name, self.parameters, self.num_external_parameters,
                      self.precondition.relaxed().simplified(),
                      new_effects)
    def untyped(self):
        # We do not actually remove the types from the parameter lists,
        # just additionally incorporate them into the conditions.
        # Maybe not very nice.
        result = copy.copy(self)
        parameter_atoms = [par.to_untyped_strips() for par in self.parameters]
        new_precondition = self.precondition.untyped()
        result.precondition = conditions.Conjunction(parameter_atoms + [new_precondition])
        result.effects = [eff.untyped() for eff in self.effects]
        return result
    def instantiate(self, var_mapping, init_facts, fluent_facts, init_function_vals,
                    fluent_functions, task, new_axiom, new_modules, objects_by_type):        
        """Return a PropositionalAction which corresponds to the instantiation of
        this action with the arguments in var_mapping. Only fluent parts of the
        conditions (those in fluent_facts) are included. init_facts are evaluated
        while instantiating.
        Precondition and effect conditions must be normalized for this to work.
        Returns None if var_mapping does not correspond to a valid instantiation
        (because it has impossible preconditions or an empty effect list.)"""
        arg_list = [var_mapping[par.name]
                    for par in self.parameters[:self.num_external_parameters]]
        name = "(%s %s)" % (self.name, " ".join(arg_list))

        precondition = []
        try:
            self.precondition.instantiate(var_mapping, init_facts, fluent_facts, 
                                       init_function_vals, fluent_functions, task,
                                       new_axiom, new_modules, precondition)            
        except conditions.Impossible:
            return None
        effect_list = []
        for eff in self.effects:
            eff.instantiate(var_mapping, init_facts, fluent_facts, 
                            init_function_vals, fluent_functions, task, 
                            new_axiom, new_modules, objects_by_type, effect_list)
        if effect_list:
#            print("DEBUG: actions.Action.instantiate: metric %s" % [task.metric])
            # task.metric is a tuple where metric[0] in {'<','>'} and metric[1] the PNE to be minimized/maximized            
            if (task.metric[1] != -1):   
                if self.cost is None:
                    cost = 0.0 # With NFD the cost field stores an evaluation of the 
                               # numeric fluents after applying the action in the initial state.
                               # In tasks where action costs are used, but no cost
                               # is specified for the action, its assumed cost is zero.   
                else:
                    assert(isinstance(self.cost, effects.NumericEffect))  
                    cost = self.cost.effect.instantiate_cost(var_mapping, fluent_functions, init_function_vals, task)
#                    cost = self.cost.instantiate_cost(var_mapping, fluent_functions, init_function_vals, task)
            else: # no metric
                cost = 1.0        
            return PropositionalAction(name, precondition, effect_list, cost)
        else: # no effect_list
            return None

class PropositionalAction:
    def __init__(self, name, precondition, effects, cost):
        self.name = name
        self.precondition = precondition
        self.add_effects = []
        self.del_effects = []
        self.assign_effects = [] # NFD action also contain assign effects. Cassic propositional planning tasks with :action-costs contain exactly one assign effect (the increase effect on total-cost)
        for (condition, effect) in effects:
            if isinstance(effect,f_expression.FunctionAssignment): # NFD
                self.assign_effects.append((condition, effect))
            elif not effect.negated:
                self.add_effects.append((condition, effect))
        # Warning: This is O(N^2), could be turned into O(N).
        # But that might actually harm performance, since there are
        # usually few effects.
        # TODO: Measure this in critical domains, then use sets if acceptable.
        for (condition, effect) in effects:
            if not isinstance(effect,f_expression.FunctionAssignment) and effect.negated and (condition, effect.negate()) not in self.add_effects:
                self.del_effects.append((condition, effect.negate()))
        self.cost = cost # deprecated float, cost effects are regular assignment effects now

    def __repr__(self):
        return "<PropositionalAction %r at %#x>" % (self.name, id(self))
    def dump(self):
        print(self.name)
        for fact in self.precondition:
            print("PRE: %s" % fact)
        for cond, fact in self.add_effects:
            print("ADD: %s -> %s" % (", ".join(map(str, cond)), fact))
        for cond, fact in self.del_effects:
            print("DEL: %s -> %s" % (", ".join(map(str, cond)), fact))
        for cond, fact in self.assign_effects:
            print ("ASS: %s -> %s" % (", ".join(map(str, cond)), fact))            
        print("cost:", self.cost)