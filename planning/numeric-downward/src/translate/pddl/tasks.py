from __future__ import print_function

import sys
import itertools

from . import actions
from . import axioms
from . import conditions
from . import predicates
from . import pddl_types
from . import functions
from . import f_expression

DEBUG = False

class Task(object):
    FUNCTION_SYMBOLS = dict()     
    def __init__(self, domain_name, task_name, requirements,
                 types, objects, predicates, functions, init, num_init, goal, actions, axioms, metric):
        self.domain_name = domain_name
        self.task_name = task_name
        self.requirements = requirements
        self.types = types
        self.objects = objects
        self.predicates = predicates
        self.functions = functions
        self.init = init
        self.num_init = num_init
        self.goal = goal
        self.actions = actions
        self.axioms = axioms
        self.axiom_counter = 0
        self.function_administrator = DerivedFunctionAdministrator()
        self.metric = metric
        self.global_constraint = None
#        if DEBUG: print("Task.__init__ with metric", metric)

    def add_global_constraints(self): # for each global constraint axioms add a new universal axiom "forall PARAMETERS satisfy AXIOM"
                                      # and finally add a conjunction axiom of all such universal constraints "AND UNIVERSALAXIOMS"
        if DEBUG: print("Adding global constraints")
        universal_constraints = []
        the_global_constraint = conditions.Truth() 
        for axiom in self.axioms:
            if axiom.is_global_constraint:
                axiom.is_global_constraint = False
                universe = conditions.UniversalCondition(axiom.parameters, [axiom.condition])                
                universal_constraints.append(universe)
        if universal_constraints:
            if DEBUG: print("There are %d universal constraints" % len(universal_constraints))
            the_global_constraint = conditions.Conjunction(universal_constraints)
        if DEBUG: print ("Adding axiom for global constraint")
        self.add_axiom([], the_global_constraint)
        self.global_constraint = conditions.Atom(self.axioms[-1].name, [])        
        if DEBUG: print ("The global constraint is : %s"%self.global_constraint)                

    def add_axiom(self, parameters, condition):
        name = "new-axiom@%d" % self.axiom_counter
        self.axiom_counter += 1
        axiom = axioms.Axiom(name, parameters, len(parameters), condition)
        self.predicates.append(predicates.Predicate(name, parameters))
        self.axioms.append(axiom)
        return axiom

#     @staticmethod
#     def parse(domain_pddl, task_pddl):
#         domain_name, domain_requirements, types, constants, predicates, functions, actions, axioms \
#                      = parse_domain(domain_pddl)
#         task_name, task_domain_name, task_requirements, objects, init, num_init, goal, metric = parse_task(task_pddl)
# 
#         assert domain_name == task_domain_name
#         requirements = Requirements(sorted(set(
#                     domain_requirements.requirements +
#                     task_requirements.requirements)))
#         objects = constants + objects
#         check_for_duplicates(
#             [o.name for o in objects],
#             errmsg="error: duplicate object %r",
#             finalmsg="please check :constants and :objects definitions")
#         init += [conditions.Atom("=", (obj.name, obj.name)) for obj in objects]
# 
#         return Task(domain_name, task_name, requirements, types, objects,
#                     predicates, functions, init, num_init, goal, actions, axioms, metric)

    def dump(self):
        print("Problem %s: %s [%s]" % (
            self.domain_name, self.task_name, self.requirements))
        print("Types:")
        for type in self.types:
            print("  %s" % type)
        print("Objects:")
        for obj in self.objects:
            print("  %s" % obj)
        print("Predicates:")
        for pred in self.predicates:
            print("  %s" % pred)
        print("Functions:")
        for func in self.functions:
            print("  %s" % func)
        print("Init:")
        for fact in self.init:
            print("  %s" % fact)
        print("Numeric Init:")
        for fact in self.num_init:
            print("  %s" % fact)            
        print("Goal:")
        self.goal.dump()
        print("Derived Functions:")
        self.function_administrator.dump()                
        print("Actions:")
        for action in self.actions:
            action.dump()
        if self.axioms:
            print("Axioms:")
            for axiom in self.axioms:
                axiom.dump()
        print("Metric:")
        print(self.metric)
            
SEEN_WARNING_PDDL_REQUIREMENT = False
class Requirements(object):    
    def __init__(self, requirements):
        global SEEN_WARNING_PDDL_REQUIREMENT
#        print("Requirements contructur with arg '%s'" % requirements) 
        self.requirements = requirements
        for req in requirements:
            assert req in (
              ":strips", ":adl", ":typing", ":negation", ":equality",
              ":negative-preconditions", ":disjunctive-preconditions",
              ":existential-preconditions", ":universal-preconditions",
              ":quantified-preconditions", ":conditional-effects",
              ":derived-predicates", ":action-costs", ":numeric-fluents", 
              ":object-fluents", 
              ":fluents"), req
            if req == ":fluents":
                msg = ("WARNING: deprecated PDDL option :fluents treated as :numeric-fluents")
                if not SEEN_WARNING_PDDL_REQUIREMENT:
                        print(msg, file=sys.stderr)
                        SEEN_WARNING_PDDL_REQUIREMENT = True 
            if req == ":object-fluents":
                msg = ("WARNING :object-fluents are not entirely supported yet")
                print(msg, file=sys.stderr) 
                assert False, ":object-fluents are not supported yet"                
    def __str__(self):
        return ", ".join(self.requirements)

def prettyprint(mystring):
    if mystring == "-":
        mystring = "difference"       
    if mystring == "+":
        mystring = "sum"       
    if mystring == "*":
        mystring = "product"       
    if mystring == "/":
        mystring = "quotient"       
    return mystring

class DerivedFunctionAdministrator(object):
    #TODO use hash values?
    def __init__(self):
        self.functions = dict() 
           
    def dump(self,indent = "  "):
        for axiom in self.functions.values():
            axiom.dump(indent)
    def get_all_axioms(self):
        return self.functions.values() 
    def get_derived_function(self,exp):
        def get_default_variables(nr):
            varlist = [("?v%s" % varnr) for varnr in range(nr)]
            return varlist
        def get_new_symbol(key):
            # introduce new derived function symbol
            used_names = [axiom.name for axiom in self.functions.values()]
            for counter in itertools.count(1):
                assert counter == 1
                addition = ''.join("%s_" % prettyprint(part) for part in key)
                addition = addition[:-1] # chop last '_'
                new_func_name = "derived!" + addition # + str(counter)
                if new_func_name not in used_names:                    
                    Task.FUNCTION_SYMBOLS[new_func_name]="number"
                    return new_func_name
        if DEBUG:
            print ("tasks.py get derived function for expression ",exp)  
        assert isinstance(exp,f_expression.FunctionalExpression), "expected Functional Expression, got %s "% exp.__class__
        if isinstance(exp,f_expression.PrimitiveNumericExpression):
            return exp
        elif isinstance(exp,f_expression.NumericConstant):            
            if DEBUG: print ("Constant ", exp)
            key = (exp.value,)
            if key not in self.functions:
                symbol = get_new_symbol(key)
                self.functions[key] = axioms.NumericAxiom(symbol,[],None,[exp])                
            args = ()
        elif isinstance(exp,f_expression.AdditiveInverse):
            if DEBUG: print ("additive inverse ", exp)
            subexp = self.get_derived_function(exp.parts[0])
            key = (exp.op, subexp.symbol)
            args = subexp.args
            if key not in self.functions:
                symbol = get_new_symbol(key)
                default_args = get_default_variables(len(subexp.args)) 
                subexp = f_expression.PrimitiveNumericExpression(subexp.symbol, default_args, 'S')
                self.functions[key] = axioms.NumericAxiom(symbol, default_args, exp.op, [subexp])
        else:
            if DEBUG: print ("arithmetic expression ", exp)                
            assert isinstance(exp,f_expression.ArithmeticExpression)
            if (len(exp.parts) != 2):
                assert exp.op in ["+", "*"]
            if exp.op in ("+","*"):
                if DEBUG: print("DEBUG tasks.py checking parts of exp ", exp)
                if DEBUG: print("DEBUG tasks.py exp.parts befor %s" % [exp.parts])
                exp.parts = tuple(sorted(list(exp.parts))) # as addition and multiplication are commutative, we only generate one derived expression
                if DEBUG: print("DEBUG tasks.py exp.parts after %s" % [exp.parts])
            keylist = [exp.op] + [self.get_derived_function(part) for part in exp.parts]
            if DEBUG: print ("keylist = %s"%keylist)
            key = tuple(keylist)
            args = ()
            for df in keylist[1:]:
                args += df.args
            if DEBUG: print ("args = %s"% [args])
            if key not in self.functions:
                symbol = get_new_symbol(key)
                default_args = get_default_variables(len(args))
                argindex = 0
                pnelist = []
                for df in keylist[1:]:
                    pnelist.append(f_expression.PrimitiveNumericExpression(df.symbol, default_args[argindex:argindex+len(df.args)], 'D'))
                    if DEBUG: print("DEBUG tasks.py appending to pnelist %s" % pnelist[-1])
                    argindex += len(df.args)
                assert argindex == len(default_args)
                assert pnelist
                self.functions[key] = axioms.NumericAxiom(symbol, tuple(default_args), exp.op, pnelist)
        pne_symbol = self.functions[key].get_head().symbol
        if DEBUG: print("returning derived function ",pne_symbol)
        return f_expression.PrimitiveNumericExpression(pne_symbol,args, 'D')

def check_atom_consistency(atom, same_truth_value, other_truth_value, atom_is_true=True):
    if atom in other_truth_value:
        raise SystemExit("Error in initial state specification\n" +
                         "Reason: %s is true and false." %  atom)
    if atom in same_truth_value:
        if not atom_is_true:
            atom = atom.negate()
        print("Warning: %s is specified twice in initial state specification" % atom)
    
def check_for_duplicates(elements, errmsg, finalmsg):
    seen = set()
    errors = []
    for element in elements:
        if element in seen:
            errors.append(errmsg % element)
        else:
            seen.add(element)
    if errors:
        raise SystemExit("\n".join(errors) + "\n" + finalmsg)
