from __future__ import print_function

from . import conditions
#from . import pddl_types
from . import f_expression

def cartesian_product(*sequences):
    # TODO: Also exists in tools.py outside the pddl package (defined slightly
    #       differently). Not good. Need proper import paths.
    if not sequences:
        yield ()
    else:
        for tup in cartesian_product(*sequences[1:]):
            for item in sequences[0]:
                yield (item,) + tup


class Effect(object):
    def __init__(self, parameters, condition, peffect): 
        self.parameters = parameters
        self.condition = condition
        self.peffect = peffect # NFD: primitive effects can be either literals or function assignments
    def __eq__(self, other):
        return (self.__class__ is other.__class__ and
                self.parameters == other.parameters and
                self.condition == other.condition and
                self.peffect == other.peffect)
    def dump(self):
        indent = "  "
        if self.parameters:
            print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
            indent += "  "
        if self.condition != conditions.Truth():
            print("%sif" % indent)
            self.condition.dump(indent + "  ")
            print("%sthen" % indent)
            indent += "  "
        print("%s%s" % (indent, self.peffect))
    def copy(self):
        return Effect(self.parameters, self.condition, self.peffect)
    def uniquify_variables(self, type_map):
        renamings = {}
        self.parameters = [par.uniquify_name(type_map, renamings)
                           for par in self.parameters]
        self.condition = self.condition.uniquify_variables(type_map, renamings)
        self.peffect = self.peffect.rename_variables(renamings)                                 
    def instantiate(self, var_mapping, init_facts, fluent_facts,
                    init_function_vals, fluent_functions, task, new_axiom, new_modules, objects_by_type, result):
        if self.parameters:
            var_mapping = var_mapping.copy() # Will modify this.
            object_lists = [objects_by_type.get(par.type_name, [])
                            for par in self.parameters]
            for object_tuple in cartesian_product(*object_lists):
                for (par, obj) in zip(self.parameters, object_tuple):
                    var_mapping[par.name] = obj
                self._instantiate(var_mapping, init_facts, fluent_facts, init_function_vals,
                          fluent_functions, task, new_axiom, new_modules, result)
        else:
            self._instantiate(var_mapping, init_facts, fluent_facts, init_function_vals,
                          fluent_functions, task, new_axiom, new_modules, result)
    def _instantiate(self, var_mapping, init_facts, fluent_facts, init_function_vals,
                          fluent_functions, task, new_axiom, new_modules, result):
        condition = []
        try:
            self.condition.instantiate(var_mapping, init_facts, fluent_facts, init_function_vals,
                                       fluent_functions, task, new_axiom, new_modules, condition)
        except conditions.Impossible:
            return
        effects = []
        self.peffect.instantiate(var_mapping, init_facts, fluent_facts, 
                             init_function_vals, fluent_functions, task,
                             new_axiom, new_modules, effects)
        assert len(effects) <= 1
        if effects:
            result.append((condition, effects[0]))
    def relaxed(self):
        if self.peffect.negated:
            return None
        else:
            return Effect(self.parameters, self.condition.relaxed(), self.peffect)
    def simplified(self):
        return Effect(self.parameters, self.condition.simplified(), self.peffect)


class ConditionalEffect(object):
    def __init__(self, condition, effect):
        if isinstance(effect, ConditionalEffect):
            self.condition = conditions.Conjunction([condition, effect.condition])
            self.effect = effect.effect
        else:
            self.condition = condition
            self.effect = effect
    def dump(self, indent="  "):
        print("%sif" % (indent))
        self.condition.dump(indent + "  ")
        print("%sthen" % (indent))
        self.effect.dump(indent + "  ")
    def normalize(self):
        norm_effect = self.effect.normalize()
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert isinstance(effect, SimpleEffect) or isinstance(effect, ConditionalEffect) or isinstance(effect, NumericEffect)
                new_effects.append(ConditionalEffect(self.condition, effect))
            return ConjunctiveEffect(new_effects)
        elif isinstance(norm_effect, UniversalEffect):
            child = norm_effect.effect
            cond_effect = ConditionalEffect(self.condition, child)
            return UniversalEffect(norm_effect.parameters, cond_effect)
        else:
            return ConditionalEffect(self.condition, norm_effect)
    def extract_cost(self):
        return None, self

class UniversalEffect(object):
    def __init__(self, parameters, effect):
        if isinstance(effect, UniversalEffect):
            self.parameters = parameters + effect.parameters
            self.effect = effect.effect
        else:
            self.parameters = parameters
            self.effect = effect
    def dump(self, indent="  "):
        print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
        self.effect.dump(indent + "  ")
    def normalize(self):
#         print("Normalizing Universal Effect")
#         self.dump()
        norm_effect = self.effect.normalize()
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert isinstance(effect, SimpleEffect) or isinstance(effect, NumericEffect)\
                     or isinstance(effect, ConditionalEffect) or isinstance(effect, UniversalEffect), "Instance is %s instead" % effect.__class__
                new_effects.append(UniversalEffect(self.parameters, effect))
            return ConjunctiveEffect(new_effects)
        else:
            return UniversalEffect(self.parameters, norm_effect)
    def extract_cost(self):
        return None, self

class ConjunctiveEffect(object):
    def __init__(self, effects):
        flattened_effects = []
        for effect in effects:
            if isinstance(effect, ConjunctiveEffect):
                flattened_effects += effect.effects
            else:
                flattened_effects.append(effect)
        self.effects = flattened_effects
    def dump(self, indent="  "):
        print("%sand" % (indent))
        for eff in self.effects:
            eff.dump(indent + "  ")    
    def normalize(self):
        new_effects = []
        for effect in self.effects:
            new_effects.append(effect.normalize())
        return ConjunctiveEffect(new_effects) # creating a new effect flattens it automatically                                   
    def extract_cost(self):
        new_effects = []
        cost_effect = None
        for part in self.effects:
            if isinstance(part.effect, f_expression.FunctionAssignment) and part.effect.is_cost_assignment():
                assert cost_effect == None, "multiple values assigned to cost effect"                
                cost_effect = part
            new_effects.append(part) # even if the effect is a cost effect it is just a "regular" numeric effect
        return cost_effect, ConjunctiveEffect(new_effects)

class SimpleEffect(object):
    def __init__(self, effect):
        self.effect = effect
        assert isinstance(effect, conditions.Literal)
    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))
    def normalize(self):
        return self
    def extract_cost(self):
        return None, self

# With the support of numeric planning,NumericEffects are FunctionAssignment that subsume Fast Downwards "CostEffect".
class NumericEffect(object):
    def __init__(self, effect):
        assert isinstance(effect, f_expression.FunctionAssignment)
        self.effect = effect
    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))
    def normalize(self):
        return self
    def extract_cost(self): # returns pair (cost_effect, regular_effect)
        if self.effect.is_cost_assignment():
            return self, self # A cost effect is still a regular effect and using the old cost is deprecated but still supported.
        else:
            return None, self  
