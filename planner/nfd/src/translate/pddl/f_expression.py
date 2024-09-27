from __future__ import print_function
#import string 

#import pddl

class FunctionalExpression(object):
    def __init__(self, parts):
        self.parts = tuple(parts)
        self.hash = hash((self.__class__, self.parts))
    def __ne__(self, other):
        return not self == other
    def free_variables(self):
        result = set()
        for part in self.parts:
            result |= part.free_variables()
        return result
    def change_parts(self, parts):
        return self
    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))
        for part in self.parts:
            part.dump(indent + "  ")
    def __repr__(self, *args, **kwargs):
        return "FunctExp %s" % [str(p) for p in self.parts]
    def _dump(self):
        return self.__class__.__name__
    def _postorder_visit(self, method_name, *args):
        part_results = [part._postorder_visit(method_name, *args)
                        for part in self.parts] 
        method = getattr(self, method_name, self._propagate)
        return method(part_results, *args)
    def _propagate(self, parts, *args):
        return self.change_parts(parts)
    def primitive_numeric_expressions(self):
        result = set()
        for part in self.parts:
            result |= part.primitive_numeric_expressions()                                    
        return result
    def compile_objectfunctions_aux(self,used_variables, recurse_object_terms=True):
        typed_vars = []
        conjunction_parts = []
        new_parts = []
        for part in self.parts:
            typed,parts,new_part = part.compile_objectfunctions_aux(used_variables,
                                                                    recurse_object_terms)
            typed_vars += typed
            conjunction_parts += parts
            new_parts.append(new_part)
        return (typed_vars,conjunction_parts,self.__class__(new_parts))    
    def  instantiate(self, var_mapping, fluent_functions, 
                        init_function_vals, task, new_axioms=[]):
        print (self.__class__.__name__)
        raise ValueError("Cannot instantiate condition: not normalized")

class ArithmeticExpression(FunctionalExpression):
    def __str__(self, *args, **kwargs):
        return "ArExp " + self.op + "%s" % [str(p) for p in self.parts]
    def __eq__(self,other):
        return (self.hash == other.hash and
                self.__class__ == other.__class__ and
                self.parts == other.parts)
    def rename_variables(self, renamings={}):
        return self.__class__([part.rename_variables(renamings)
                               for part in self.parts])
    def change_parts(self, parts):
        return self.__class__(parts)
#    def remove_duration_variable(self, action, time, duration, pnes):
#        return self.__class__([part.remove_duration_variable(action, time, duration, pnes)
#                               for part in self.parts])

class Difference(ArithmeticExpression):
    op = "-"
    def __init__(self,parts):
        assert len(parts)==2
        ArithmeticExpression.__init__(self,parts)
    def _simplified(self, parts):
        if isinstance(parts[1], NumericConstant) and parts[1].value == 0:
            return parts[0]
        else:
            return self._propagate(parts)

class AdditiveInverse(ArithmeticExpression):
    op = "-"
    def __init__(self,parts):
        assert len(parts)==1
        ArithmeticExpression.__init__(self,parts)
    def _simplified(self, parts):
        return self._propagate(parts)

class Sum(ArithmeticExpression):
    op = "+"
    def _simplified(self, parts):
        result_parts = []
        for part in parts:
            if isinstance(part, Sum):
                result_parts += part.parts
            elif not (isinstance(part, NumericConstant) and part.value == 0):
                result_parts.append(part)
        if not result_parts:
            return NumericConstant(0.0)
        if len(result_parts) == 1:
            return result_parts[0]
        return Sum(result_parts)

class Product(ArithmeticExpression):
    op = "*"
    def _simplified(self, parts):
        result_parts = []
        for part in parts:
            if isinstance(part, Product):
                result_parts += part.parts
            elif isinstance(part, NumericConstant) and part.value == 0:
                return NumericConstant(0.0)
            elif not (isinstance(part, NumericConstant) and part.value == 1):
                result_parts.append(part)
        if not result_parts:
            return NumericConstant(1.0)
        if len(result_parts) == 1:
            return result_parts[0]
        return Product(result_parts)

class Quotient(ArithmeticExpression):
    op = "/"
    def __init__(self,parts):
        assert len(parts)==2
        ArithmeticExpression.__init__(self,parts)
    def _simplified(self, parts):
        if isinstance(parts[1], NumericConstant) and parts[1].value == 0:
            raise ValueError('Division by Zero')
        else:
            return self._propagate(parts)

class NumericConstant(FunctionalExpression):
    parts = ()
    def __init__(self, value):
        self.value = value
    def __eq__(self, other):
        return (self.__class__ == other.__class__ and self.value == other.value)
    def __str__(self):
        return "%s %s" % (self.__class__.__name__, self.value)
    def _dump(self):
        return str(self)
    def rename_variables(self, type_map, renamings={}):
        return self
    def instantiate(self, var_mapping, fluent_functions, 
                        init_function_vals, task, new_axioms=[]):
        return self

class PrimitiveNumericExpression(FunctionalExpression):
    parts = ()
    def __init__(self, symbol, args, ntype='R'):
        self.symbol = symbol
        self.args = tuple(args)
        self.hash = hash((self.__class__, self.symbol, self.args))
        self.ntype = ntype # 'R': regular 'C': constant 'I': instrumentation
        assert(ntype in ['C', 'D', 'I', 'R']), "Type is %s" %ntype       
        if self.symbol == "total-cost" and ntype != 'I':
            self.ntype = 'I'
    
    def __hash__(self):
        return self.hash
    def __eq__(self, other):
        return (self.__class__ == other.__class__ and self.symbol == other.symbol
                and self.args == other.args)
    def __str__(self):
        return "%s %s(%s)" % ("PNE", self.symbol, ", ".join(map(str, self.args)))
    def __repr__(self, *args, **kwargs):
        return self.__str__()
    def dump(self, indent="  "):
        print("%s%s '%s'" % (indent, self._dump(), self.ntype))
    def _dump(self):
        return str(self)
    def rename_variables(self, renamings):
        new_args = [renamings.get(arg, arg) for arg in self.args]
        return self.__class__(self.symbol, new_args, self.ntype)    
    def free_variables(self):
        return set(arg for arg in self.args if arg[0] == "?")
    def instantiate(self, var_mapping, fluent_functions, 
                        init_function_vals, task, new_axioms): #=set()): ## removed default constructor for debug reasons
        args = [var_mapping.get(arg, arg) for arg in self.args]
        pne = PrimitiveNumericExpression(self.symbol, args, self.ntype)
        # TODO check whether this PNE is fluent. Otherwise substitute it by the
        # corresponding constant
        if fluent_functions!=None:
            if pne not in fluent_functions and not pne.symbol.startswith("derived!"):
                if pne not in init_function_vals:
                    raise ValueError("Cannot instantiate non-fluent PNE: no initial value given %s" % pne)
                constant =  init_function_vals[pne]
                new_axiom_predicate = task.function_administrator.get_derived_function(constant)
                new_axiom = task.function_administrator.functions[(constant.value,)]
                new_axiom = new_axiom.instantiate(var_mapping, fluent_functions,init_function_vals,
                            task, new_axioms)
                new_axioms.add(new_axiom)
#                ground_name = pne.symbol + " " + str(len(pne.args)) + " " + " ".join(map(lambda x: x, list(pne.args)))
#                print("DEBUG info: new_axioms = %s" % new_axioms)
#                print("DEBUG info: new_axiom = %s" % new_axiom)
#                print("DEBUG info: new_axiom_predicate = %s" % new_axiom_predicate)
#                pddl.Task.CONSTANT_MAPPING[ground_name] = new_axiom_predicate  
                return new_axiom_predicate
        return pne        
    def primitive_numeric_expressions(self):
        return set([self])
        
class FunctionAssignment(object):
    def __init__(self, fluent, expression):
        self.fluent = fluent
        self.expression = expression
        self.hash = hash((self.__class__.__name__, self.fluent, self.expression))        
    def __str__(self):
        return "%s %s %s" % (self.__class__.__name__, self.fluent, self.expression)
    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))
        self.fluent.dump(indent + "  ")
        self.expression.dump(indent + "  ")
    def _dump(self):
        return self.__class__.__name__
    def rename_variables(self, renamings): # NFD: FunctionAssignments are no longer restricted to "total-cost" 
        return self.__class__(self.fluent.rename_variables(renamings),
                              self.expression.rename_variables(renamings))
    def instantiate(self, var_mapping, init_facts, fluent_facts,
                        init_function_vals, fluent_functions, task, new_axioms, modules, result):
        if not isinstance(self.expression,PrimitiveNumericExpression):
#            self.dump()
#            print("self.expression = %s" % self.expression)
#            print("self.expression is not a PNE but %s" % self.expression.__class__)
            raise ValueError("Cannot instantiate assignment: not normalized")
        fluent = self.fluent.instantiate(var_mapping, fluent_functions, 
                                         init_function_vals, task, new_axioms)
        try:
            expression = self.expression.instantiate(var_mapping, fluent_functions, 
                                             init_function_vals, task, new_axioms)
            result.append(self.__class__(fluent,expression))
        except:
            return
    def instantiate_cost(self,var_mapping, fluent_functions, init_function_vals, task):
        assert self.is_cost_assignment(), "trying to instantiate cost from non cost function assignment"
        try:
            expression = self.expression
            if isinstance(self.expression, PrimitiveNumericExpression): 
                args = [var_mapping.get(arg, arg) for arg in self.expression.args]
                instantiated_pne = PrimitiveNumericExpression(self.expression.symbol, args) # instantiates the current PNE
                expression = init_function_vals[instantiated_pne] # assumes that the expression of the PNE is a NumericConstant
                                                                  # defined in the initial state in the problem file of an PDDL task. 
            assert isinstance(expression, NumericConstant) # either expression was a NumericConstant defined in the operator already, 
                                                           # or the above if statement set it to the value defined in the initial state                  
            return expression.value                            
        except:            
#            print("Cost assignment is too complex, using unit cost as fallback")         
            return 1.0            
    def is_cost_assignment(self):
#         print("Checking whether %s is a cost assignment" % self.fluent.symbol) 
        if (self.fluent.symbol == "total-cost"):
            return True 
        return False

class Assign(FunctionAssignment):
    symbol = "="    
    def __str__(self):
        return "%s := %s" % (self.fluent, self.expression)

class Increase(FunctionAssignment):
    symbol = "+"
    pass

class ScaleUp(FunctionAssignment):
    symbol = "*"
    pass

class ScaleDown(FunctionAssignment):
    symbol = "/"
    pass

class Decrease(FunctionAssignment):
    symbol = "-"
    pass
