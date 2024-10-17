from __future__ import print_function

from . import conditions
#from . import predicates
from . import f_expression

class Axiom(object):
    def __init__(self, name, parameters, num_external_parameters, condition, is_global_constraint=False):
        # For an explanation of num_external_parameters, see the
        # related Action class. Note that num_external_parameters
        # always equals the arity of the derived predicate.
        assert 0 <= num_external_parameters <= len(parameters)
        self.name = name
        self.parameters = parameters
        self.num_external_parameters = num_external_parameters
        self.condition = condition
        self.uniquify_variables()
        self.is_global_constraint = is_global_constraint # in tasks.add_global_constraints this flag is read to create "the" global constraint
    def parse(alist):
        assert len(alist) == 3
        assert alist[0] == ":derived"
        predicate = predicates.Predicate.parse(alist[1])
        condition = conditions.parse_condition(alist[2])
        return Axiom(predicate.name, predicate.arguments,
                     len(predicate.arguments), condition)
    parse = staticmethod(parse)
    def __repr__(self, *args, **kwargs):
        return "Axiom %s(%s)" % (self.name, ", ".join(args))
    def dump(self):
        args = map(str, self.parameters[:self.num_external_parameters])
        print("Axiom %s(%s)" % (self.name, ", ".join(args)))
        self.condition.dump()
    def uniquify_variables(self):
        self.type_map = dict([(par.name, par.type_name)
                              for par in self.parameters])
        self.condition = self.condition.uniquify_variables(self.type_map)
    def instantiate(self, var_mapping, init_facts, fluent_facts, init_function_vals,
                    fluent_functions, task, new_axiom, new_modules):
        # The comments for Action.instantiate apply accordingly.
        arg_list = [self.name] + [var_mapping[par.name]
                    for par in self.parameters[:self.num_external_parameters]]
        name = "(%s)" % " ".join(arg_list)

        condition = []
        try:            
            self.condition.instantiate(var_mapping, init_facts, fluent_facts, init_function_vals,
                    fluent_functions, task, new_axiom, new_modules, condition)
        except conditions.Impossible:
            return None

        effect_args = [var_mapping.get(arg.name, arg.name)
                       for arg in self.parameters[:self.num_external_parameters]]
        effect = conditions.Atom(self.name, effect_args)
        return PropositionalAxiom(name, condition, effect)

class PropositionalAxiom:
    def __init__(self, name, condition, effect):
        self.name = name
        self.condition = condition
        self.effect = effect
    def clone(self):
        return PropositionalAxiom(self.name, list(self.condition), self.effect)
    def dump(self):
        if self.effect.negated:
            print("not", end=' ')
        print(self.name)
        for fact in self.condition:
            print(" PRE: %s" % fact)
        print(" EFF: %s" % self.effect)
    @property
    def key(self):
        return (self.name, self.condition, self.effect)
    def __lt__(self, other):
        return self.key < other.key
    def __le__(self, other):
        return self.key <= other.key
    def __eq__(self, other):
        return self.key == other.key
    def __repr__(self):
        return '<PropositionalAxiom %s %s -> %s>' % (self.name, self.condition, self.effect)
    
class NumericAxiom(object):
    def __init__(self, name, parameters, op, parts):
        self.name = name
        self.parameters = parameters
        self.op = op
        self.parts = parts # contains NumericAxioms, PrimitiveNumericExpressions or a NumericConstant
        assert parts
        self.ntype = 'D' if op else 'C'
    def __str__(self):
        return "%s: %s(%s)" %(self.__class__.__name__, self.name, ", ".join(map(str, self.parameters)))
    def get_head(self):                
        return f_expression.PrimitiveNumericExpression(self.name,self.parameters, self.ntype)
        
    def dump(self,indent):
        head = "(%s %s)" % (self.name, ", ".join(map(str, self.parameters)))
        op = ""
        if self.op:
            op = self.op + " "
        body = "%s" % " ".join(map(str, self.parts))
        print("%s%s -: %s%s" % (indent,head,op,body))
    def instantiate(self, var_mapping, fluent_functions, init_function_vals, task, new_constant_axioms):
        arg_list = [var_mapping[par] for par in self.parameters]
        name = "(%s %s)" % (self.name, " ".join([arg for arg in arg_list]))
        parts = []
        for part in self.parts:
            if isinstance(part,f_expression.NumericConstant):
                parts.append(part)
            else:
                parts.append(part.instantiate(var_mapping, fluent_functions, 
                         init_function_vals, task, new_constant_axioms))
        assert len(parts) == len(self.parts)
        effect = f_expression.PrimitiveNumericExpression(self.name, arg_list, self.ntype)
        assert parts, "Trying to instantiate axiom without parts %s" %name
        return InstantiatedNumericAxiom(name, self.op, parts, effect)
    
class InstantiatedNumericAxiom(object):
    def __init__(self, name, op, parts, effect):
        self.name = name
        self.op = op
        self.parts = parts # contains InstantiatedNumericAxioms,
                           # PrimitiveNumericExpressions or a NumericConstant
        self.effect = effect
    def __str__(self):
        return self.name
    def __cmp__(self, other):
        return cmp((self.__class__, self.name), (other.__class__, other.name))
    def __hash__(self):
        return hash((self.__class__,self.name))
    def dump(self):
        print("InstantiatedNumericAxiom %s"%self.name)
        print("OP: %s" % self.op)
        for part in self.parts:
            print("PART: %s" % part)
        print("EFF: %s" % self.effect)