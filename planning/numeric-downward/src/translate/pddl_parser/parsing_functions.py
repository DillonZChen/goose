# -*- coding: utf-8 -*-

from __future__ import print_function

import sys

import graph
import pddl

DEBUG = False

def parse_typed_list(alist, only_variables=False,
                     constructor=pddl.TypedObject,
                     default_type="object"):    
#     if DEBUG: print("parsing typed list %s with default type %s" % ([alist],default_type))    
    result = []
    while alist:
        try:
            separator_position = alist.index("-")
        except ValueError:
            items = alist
            _type = default_type
            alist = []
        else:
            items = alist[:separator_position]
            _type = alist[separator_position + 1]
            alist = alist[separator_position + 2:]
        for item in items:
            assert not only_variables or item.startswith("?"), \
                   "Expected item to be a variable: %s in (%s)" % (
                item, " ".join(items))
            entry = constructor(item, _type)
            result.append(entry) 
    return result


def set_supertypes(type_list):
    # TODO: This is a two-stage construction, which is perhaps
    # not a great idea. Might need more thought in the future.
    type_name_to_type = {}
    child_types = []
    for pddltype in type_list:
        pddltype.supertype_names = []
        type_name_to_type[pddltype.name] = pddltype
        if pddltype.basetype_name:
            child_types.append((pddltype.name, pddltype.basetype_name))
    for (desc_name, anc_name) in graph.transitive_closure(child_types):
        type_name_to_type[desc_name].supertype_names.append(anc_name)


def parse_predicate(alist):
    name = alist[0]
#     if DEBUG: print("parsing predicate: ", name)
    arguments = parse_typed_list(alist[1:], only_variables=True)
    return pddl.Predicate(name, arguments)


def parse_function(alist, type_name):
    name = alist[0]
    arguments = parse_typed_list(alist[1:])        
    pddl.Task.FUNCTION_SYMBOLS[name] = type_name; # add function name to function symbol dictionary
    return pddl.Function(name, arguments, type_name)


def parse_condition(alist, type_dict, predicate_dict):
#    if DEBUG: print("DEBUG Parsing condition ", alist)
    condition = parse_condition_aux(alist, False, type_dict, predicate_dict)
    # TODO: The next line doesn't appear to do anything good,
    # since uniquify_variables doesn't modify the condition in place.
    # Conditions in actions or axioms are uniquified elsewhere, but
    # it looks like goal conditions are never uniquified at all
    # (which would be a bug).
    condition.uniquify_variables({})
    return condition

def parse_condition_aux(alist, negated, type_dict, predicate_dict):
    """Parse a PDDL condition. The condition is translated into NNF on the fly."""
#    if DEBUG: print ("parsing condition aux %s" % [alist])
    tag = alist[0]
    if is_function_comparison(alist): # NFD conditions are always comparisons between 2 numeric expressions
        args = [parse_expression(arg) for arg in alist[1:]]
        assert len(args) == 2, args
        if negated:
            return pddl.NegatedFunctionComparison(tag, args, True)
        else:
            return pddl.FunctionComparison(tag, args, True)
    elif tag in ("and", "or", "not", "imply"):
        args = alist[1:]
        if tag == "imply":
            assert len(args) == 2
        if tag == "not":
            assert len(args) == 1
            return parse_condition_aux(
                args[0], not negated, type_dict, predicate_dict)
    elif tag in ("forall", "exists"):
        parameters = parse_typed_list(alist[1])
        args = alist[2:]
        assert len(args) == 1
    else:
#         if is_object_comparison(alist):
#              print("DEBUG: Object comparison!")
#              print(alist)        
        return parse_literal(alist, type_dict, predicate_dict, negated=negated)
    if tag == "imply":
        parts = [parse_condition_aux(
                args[0], not negated, type_dict, predicate_dict),
                 parse_condition_aux(
                args[1], negated, type_dict, predicate_dict)]
        tag = "or"
    else:
        parts = [parse_condition_aux(part, negated, type_dict, predicate_dict)
                 for part in args]

    if tag == "and" and not negated or tag == "or" and negated:
        return pddl.Conjunction(parts)
    elif tag == "or" and not negated or tag == "and" and negated:
        return pddl.Disjunction(parts)
    elif tag == "forall" and not negated or tag == "exists" and negated:
        return pddl.UniversalCondition(parameters, parts)
    elif tag == "exists" and not negated or tag == "forall" and negated:
        return pddl.ExistentialCondition(parameters, parts)

# numeric Fast Downward: 
# checks if the condition compares two numeric expressions
def is_function_comparison(alist):    
    tag = alist[0]
    if tag in (">","<",">=","<="):
        return True
    if not tag == "=":
        return False
    # tag is '=' and could also be an object comparison which is not implemented yet
    symbol = alist[1]
    if isinstance(symbol,list):
        if symbol[0] in ("+","/","*","-"):
            return True
        symbol = symbol[0]        
    if (pddl.Task.FUNCTION_SYMBOLS.get(symbol,"object")=="number" or 
        symbol.replace(".","").isdigit()): 
        return True # numeric expression comparison
    return False # object comparison 

def is_object_comparison(alist):
    if not alist[0] == "=":
        return False    
    if len(alist) != 3:
        return False
    o1 = pddl.Task.FUNCTION_SYMBOLS.get(alist[1],"object")
    o2 = pddl.Task.FUNCTION_SYMBOLS.get(alist[2],"object")
    if o1 != "number" and o2 != "number":
        return True
    return False
         
    

def parse_literal(alist, type_dict, predicate_dict, negated=False):
#    if DEBUG: print ("parsing literal %s" % [alist])
    if alist[0] == "not":
        assert len(alist) == 2
        alist = alist[1]
        negated = not negated

    pred_id, arity = _get_predicate_id_and_arity(
        alist[0], type_dict, predicate_dict)

    if arity != len(alist) - 1:
        raise SystemExit("predicate used with wrong arity: (%s)"
                         % " ".join(alist))

    if negated:
        return pddl.NegatedAtom(pred_id, alist[1:])
    else:
        return pddl.Atom(pred_id, alist[1:])


SEEN_WARNING_TYPE_PREDICATE_NAME_CLASH = False
def _get_predicate_id_and_arity(text, type_dict, predicate_dict):
    global SEEN_WARNING_TYPE_PREDICATE_NAME_CLASH

    the_type = type_dict.get(text)
    the_predicate = predicate_dict.get(text)

    if the_type is None and the_predicate is None:        
        raise SystemExit("Undeclared predicate: %s" % text)
        assert False        
    elif the_predicate is not None:
        if the_type is not None and not SEEN_WARNING_TYPE_PREDICATE_NAME_CLASH:
            msg = ("Warning: name clash between type and predicate %r.\n"
                   "Interpreting as predicate in conditions.") % text
            print(msg, file=sys.stderr)
            SEEN_WARNING_TYPE_PREDICATE_NAME_CLASH = True
        return the_predicate.name, the_predicate.get_arity()
    else:
        assert the_type is not None
        return the_type.get_predicate_name(), 1


def parse_effects(alist, result, type_dict, predicate_dict):
    """Parse a PDDL effect (any combination of simple, conjunctive, conditional, and universal)."""
#     if DEBUG: print("Parsing Effects %s" % alist)
    tmp_effect = parse_effect(alist, type_dict, predicate_dict)
    normalized = tmp_effect.normalize()
    cost_eff, rest_effect = normalized.extract_cost()
    add_effect(rest_effect, result)
    if cost_eff:
        return cost_eff # cost_eff is an effect of class NumericEffect 
    else:
        return None

def add_effect(tmp_effect, result): # adds effect from tmp_effect to result 
    """tmp_effect has the following structure:
       [ConjunctiveEffect] [UniversalEffect] [ConditionalEffect] SimpleEffect."""

    if isinstance(tmp_effect, pddl.ConjunctiveEffect):
        for effect in tmp_effect.effects:
            add_effect(effect, result)
        return
    else:
        parameters = []
        condition = pddl.Truth()
        if isinstance(tmp_effect, pddl.UniversalEffect):
#             print ("universal effect: ", tmp_effect)
            parameters = tmp_effect.parameters
            if isinstance(tmp_effect.effect, pddl.ConditionalEffect):
                condition = tmp_effect.effect.condition
                assert isinstance(tmp_effect.effect.effect, pddl.SimpleEffect)\
                     or isinstance(tmp_effect.effect.effect, pddl.NumericEffect)           
                effect = tmp_effect.effect.effect.effect
            else:
                assert isinstance(tmp_effect.effect, pddl.SimpleEffect)\
                     or isinstance(tmp_effect.effect, pddl.NumericEffect), "Effect is not primitive but %s" % tmp_effect.effect.effect.__class__           
                effect = tmp_effect.effect.effect
        elif isinstance(tmp_effect, pddl.ConditionalEffect):
#             print ("conditional effect: ", tmp_effect)
            condition = tmp_effect.condition
            assert isinstance(tmp_effect.effect, pddl.SimpleEffect) or isinstance(tmp_effect.effect, pddl.NumericEffect)
            effect = tmp_effect.effect.effect                    
        elif isinstance(tmp_effect, pddl.SimpleEffect):
#             print ("simple effect: ", tmp_effect)
            effect = tmp_effect.effect
            assert isinstance(effect, pddl.Literal)
        else:             
#             print ("numeric effect: ", tmp_effect)
            effect = tmp_effect.effect
            assert isinstance(effect, pddl.FunctionAssignment)
        # Check for contradictory effects            
        condition = condition.simplified()        
        new_effect = pddl.Effect(parameters, condition, effect)                                 
        if isinstance(effect, pddl.FunctionAssignment):
            # The check for multiple effects on the same variable loops through all effects over and over again.
            # If this raises performance issues it has to be reconsidered
#            print("Check for same effect on numeric variable %s" % effect.fluent)
            conflict = False
            for other_effect in result:                
                if isinstance(other_effect, pddl.FunctionAssignment):
#                    print("comparing to %s -> Conflict? %s" % (other_effect.fluent, (effect.fluent == other_effect.fluent)))
                    if (effect.fluent == other_effect.fluent):
                        conflict = True                        
                        break
            if conflict:
                print("Warning, multiple effects on numeric variable %s, ignoring %s" % (other_effect.fluent, effect.fluent))                         
            else:            
                result.append(new_effect)            
        else:
            assert isinstance(effect, pddl.Literal)            
            contradiction = pddl.Effect(parameters, condition, effect.negate())
            if not contradiction in result:
                result.append(new_effect)
            else:
                # We use add-after-delete semantics, keep positive effect
                if isinstance(contradiction.peffect, pddl.NegatedAtom):
                    result.remove(contradiction)
                    result.append(new_effect)
                                

def parse_effect(alist, type_dict, predicate_dict):
#     if DEBUG: print ("parsing effect %s" % [alist])    
    tag = alist[0]
    if tag == "and":
        return pddl.ConjunctiveEffect(
            [parse_effect(eff, type_dict, predicate_dict) for eff in alist[1:]])
    elif tag == "forall":
        assert len(alist) == 3
        parameters = parse_typed_list(alist[1])
        effect = parse_effect(alist[2], type_dict, predicate_dict)
        return pddl.UniversalEffect(parameters, effect)
    elif tag == "when":
        assert len(alist) == 3
        condition = parse_condition(
            alist[1], type_dict, predicate_dict)
        effect = parse_effect(alist[2], type_dict, predicate_dict)
        return pddl.ConditionalEffect(condition, effect)
    elif tag in ("scale-up", "scale-down", "increase", "decrease"):        
        return pddl.NumericEffect(parse_assignment(alist))
    elif tag == "assign":
        symbol = alist[1]
        if isinstance(symbol,list):
            symbol = symbol[0]
        return pddl.NumericEffect(parse_assignment(alist))           
    else:
        # We pass in {} instead of type_dict here because types must
        # be static predicates, so cannot be the target of an effect.
        return pddl.SimpleEffect(parse_literal(alist, {}, predicate_dict))
   
#def parse_expression(exp):
#    if isinstance(exp, list):
#        functionsymbol = exp[0]
#        return pddl.PrimitiveNumericExpression(functionsymbol, exp[1:])
#    elif exp.replace(".", "").isdigit():
#        return pddl.NumericConstant(float(exp))
#    elif exp[0] == "-":
#        raise ValueError("Negative numbers are not supported")
#    else:
#        return pddl.PrimitiveNumericExpression(exp, [])

def parse_expression(exp):
    if isinstance(exp, list):
        operator_or_functionsymbol = exp[0]         
        if operator_or_functionsymbol in ("+","/","*","-"): # NFD
            # TODO: print warning if only :action-cost is used in PDDL requirements, but not :numeric-fluents
            args = [parse_expression(arg) for arg in exp[1:]]
            operator = operator_or_functionsymbol
        else:
            # TODO: if only :action-costs are used, operator_or_functionsymbol MUST be "total-cost"            
            # TODO: Support terms. right now a numeric variable is expected here             
            return pddl.PrimitiveNumericExpression(operator_or_functionsymbol, exp[1:])        
        if operator == "+":
            return pddl.Sum(args)
        elif operator == "/":
            assert len(args) == 2
            return pddl.Quotient(args)
        elif operator == "*":
            return pddl.Product(args)
        else:
            if len(args) == 1:
                return pddl.AdditiveInverse(args)
            else:
                assert len(args) == 2
                return pddl.Difference(args)
    elif isFloat(exp):
        # TODO: another place where a warning might be useful if only :action-costs but 
        #   not :numeric-fluents are used. Negative numbers are only supported with :numeric-fluents
        return pddl.NumericConstant(float(exp))
    else:
        return pddl.PrimitiveNumericExpression(exp, [])    

def parse_assignment(alist):
    assert len(alist) == 3
    op = alist[0]
    head = parse_expression(alist[1])
    exp = parse_expression(alist[2])
    if op == "assign" or op == "=":
        return pddl.Assign(head, exp)
    elif op == "scale-up":
        return pddl.ScaleUp(head, exp)
    elif op == "scale-down":
        return pddl.ScaleDown(head, exp)
    elif op == "increase":
        return pddl.Increase(head, exp)
    elif op == "decrease":
        return pddl.Decrease(head, exp)
    else:
        assert False, "Assignment operator not supported."

def parse_action(alist, type_dict, predicate_dict):
    if DEBUG: print ("parsing action %s" % [alist])
    iterator = iter(alist)
    action_tag = next(iterator)
    assert action_tag == ":action", "Expected ':action' got '%s'" % action_tag
    name = next(iterator)
    parameters_tag_opt = next(iterator)
    if parameters_tag_opt == ":parameters":
        parameters = parse_typed_list(next(iterator),
                                      only_variables=True)
        precondition_tag_opt = next(iterator)
    else:
        parameters = []
        precondition_tag_opt = parameters_tag_opt
    if precondition_tag_opt == ":precondition":
        precondition_list = next(iterator)
        if not precondition_list:
            # Note that :precondition () is allowed in PDDL.
            precondition = pddl.Conjunction([])
        else:
            precondition = parse_condition(
                precondition_list, type_dict, predicate_dict)
            precondition = precondition.simplified()
        effect_tag = next(iterator)
    else:
        precondition = pddl.Conjunction([])
        effect_tag = precondition_tag_opt
    assert effect_tag == ":effect"
    effect_list = next(iterator)
    eff = []
    if effect_list:
        try:
            cost = parse_effects(effect_list, eff, type_dict, predicate_dict)                        
            if cost is None and eff:      
                if DEBUG: print ("adding artificial effect (increase total-cost 1) to %s" % name)          
                cost = pddl.NumericEffect(parse_assignment(['increase', 'total-cost', 1])) # artificially add a numeric effect that increases the total cost by one
                add_effect(cost, eff)
            assert(isinstance(cost, pddl.NumericEffect)), "instance of cost is %s " % cost.__class__ 
        except ValueError as e:
            raise SystemExit("Error in Action %s\nReason: %s." % (name, e))
    for rest in iterator:
        assert False, rest
    if eff:
        return pddl.Action(name, parameters, len(parameters),
                           precondition, eff, cost)
    else:
        return None

def parse_global_constraint(alist, type_dict, predicate_dict): # global constraints are parsed as special Axioms
    if DEBUG: print ("parsing constraint %s" % [alist])
    iterator = iter(alist)
    action_tag = next(iterator)
    assert action_tag == ":constraint", "Expected ':constraint' got '%s'" % action_tag
    name = next(iterator)
    parameters_tag_opt = next(iterator)
    if parameters_tag_opt == ":parameters":
        parameters = parse_typed_list(next(iterator),
                                      only_variables=True)
        condition_tag = next(iterator)
    else:
        parameters = []
        condition_tag = parameters_tag_opt
    assert condition_tag == ":condition", "Expected ':condition' got '%s'" % action_tag
    condition_list = next(iterator)
    assert condition_list, "Empty conditions are not allowed for global constraints"
    condition = parse_condition(condition_list, type_dict, predicate_dict)
    condition = condition.simplified()
    for rest in iterator:
        assert False, rest
    return pddl.Axiom(name, parameters, len(parameters), condition, True)

def parse_axiom(alist, type_dict, predicate_dict):
    if DEBUG: print("parsing axiom...")
    assert len(alist) == 3
    assert alist[0] == ":derived"
    predicate = parse_predicate(alist[1])
    condition = parse_condition(
        alist[2], type_dict, predicate_dict)
    return pddl.Axiom(predicate.name, predicate.arguments,
                      len(predicate.arguments), condition)


def parse_task(domain_pddl, task_pddl):
#     if DEBUG:
#         print("domain_pddl %s" % domain_pddl)
#        print("task_pddl %s" % task_pddl)         
    domain_name, domain_requirements, types, type_dict, constants, predicates, predicate_dict,\
        functions, actions, axioms = parse_domain_pddl(domain_pddl)
    task_name, task_domain_name, task_requirements, objects, init, num_init, goal, metric \
		= parse_task_pddl(task_pddl, type_dict, predicate_dict)
#    if DEBUG: 
#        print("Init= %s"% init)
    if not domain_name == task_domain_name:
        msg = "\nWarning: Domain name in domain file %s differs from domain name in task file %s" % (domain_name, task_domain_name) 
        print(msg, file=sys.stderr)
    #assert domain_name == task_domain_name
    requirements = pddl.Requirements(sorted(set(
                domain_requirements.requirements +
                task_requirements.requirements)))
    objects = constants + objects
    check_for_duplicates( # This will remove duplicates now instead of outright aborting.
        objects,
        errmsg="error: duplicate object %r",
        finalmsg="please check :constants and :objects definitions")
    
    init += [pddl.Atom("=", (obj.name, obj.name)) for obj in objects]
#    print("parsing_funtions parse_task returns num_init", num_init)
#    for ini in num_init:
#        ini.dump()
#    print("parsing_funtions parse_task returns metric", metric)   
    return pddl.Task(
        domain_name, task_name, requirements, types, objects,
        predicates, functions, init, num_init, goal, actions, axioms, metric)

def parse_domain_pddl(domain_pddl):                
    def typesplit(alist):   # recurse nested lists and replace "-type" by "-", "type"
                            # an error which occurs in some sloppyly modeled domains
        ix=0
        while ix < len(alist):
            el = alist[ix]
#             print("checking element %s"%el)
            if isinstance(el,list):
                typesplit(alist[ix])
            elif len(el) > 1 and el[0] == "-":
                try:
                    float(el)
                except ValueError:
                    msg = ("\nWARNING: %s seems to be a 'type' definition missing a space.\n" "Splitting Element into '-' and '%s'") % (el, el[1:])
                    print(msg, file=sys.stderr)               
                    alist[ix:ix+1] = el[0], el[1:]
            ix += 1
            
    iterator = iter(domain_pddl)
    define_tag = next(iterator)
    assert define_tag == "define"
    domain_line = next(iterator)
    assert domain_line[0] == "domain" and len(domain_line) == 2
    yield domain_line[1]

    ## We allow an arbitrary order of the requirement, types, constants,
    ## predicates and functions specification. The PDDL BNF is more strict on
    ## this, so we print a warning if it is violated.
    requirements = pddl.Requirements([":strips"])
    the_types = [pddl.Type("object")]
    constants, the_functions = [], []
    the_predicates = [pddl.Predicate("=", 
                                     [pddl.TypedObject("?x", "object"),
                                      pddl.TypedObject("?y", "object")])]
#    the_free_functions = [] ## support for global constraints with free functions is not implemented yet
    correct_order = [":requirements", ":types", ":constants", ":predicates",
                     ":functions"]#, ":free_functions"]
    seen_fields = []
    first_action = None
    for opt in iterator:
#         print("Options before: ",opt)
        typesplit(opt) # fix for missing space between dash '-' and type identifier
#         print("Options after: ",opt)         
        field = opt[0]
        if field not in correct_order:
            first_action = opt
            break
        if field in seen_fields:
            raise SystemExit("Error in domain specification\n" +
                             "Reason: two '%s' specifications." % field)
        if (seen_fields and
            correct_order.index(seen_fields[-1]) > correct_order.index(field)):
            msg = "\nWARNING: %s specification not allowed here (cf. PDDL BNF)" % field
            print(msg, file=sys.stderr)
        seen_fields.append(field)
        if field == ":requirements":
            requirements = pddl.Requirements(opt[1:])
        elif field == ":types":
            the_types.extend(parse_typed_list(
                    opt[1:], constructor=pddl.Type))
        elif field == ":constants":
            constants = parse_typed_list(opt[1:])
        elif field == ":predicates":
            the_predicates += [parse_predicate(entry)
                              for entry in opt[1:]]
        elif field == ":functions":
            the_functions = parse_typed_list(
                opt[1:],
                constructor=parse_function,
                default_type="number")
#         elif field == ":free_functions":
#             the_free_functions = parse_typed_list(
#                 opt[1:],
#                 constructor=parse_function,
#                 default_type="number")    
    set_supertypes(the_types)
    yield requirements
    yield the_types
    type_dict = dict((pddltype.name, pddltype) for pddltype in the_types)
    yield type_dict
    yield constants
    yield the_predicates
    predicate_dict = dict((pred.name, pred) for pred in the_predicates)
    yield predicate_dict    
    total_cost_fluent = pddl.Function("total-cost", [], "number")
    the_functions.append(total_cost_fluent)
#    the_functions.append(the_free_functions)
    yield the_functions

    entries = []
    if first_action is not None:
        entries.append(first_action)
    entries.extend(iterator)

    the_axioms = []
    the_actions = []
    for entry in entries:
#         if DEBUG: print("Entries before: ",entry)
        typesplit(entry) # fix for missing space between dash '-' and type identifier
#         if DEBUG: print("Entries after: ",entry)         

        if entry[0] == ":derived":
            axiom = parse_axiom(entry, type_dict, predicate_dict)
            the_axioms.append(axiom)
        elif entry[0] == ":action":
            action = parse_action(entry, type_dict, predicate_dict)
            if action is not None:
                the_actions.append(action)
        elif entry[0] == ":constraint": ## support for global constraints is new in NFD
            global_constraint = parse_global_constraint(entry, type_dict, predicate_dict)
            the_axioms.append(global_constraint)
        else:
            print("%s could not be parsed" % entry[0])
            if entry[0] != ":free_functions":
                assert False
    yield the_actions
    yield the_axioms

def parse_task_pddl(task_pddl, type_dict, predicate_dict):
    iterator = iter(task_pddl)

    define_tag = next(iterator)
    assert define_tag == "define"
    problem_line = next(iterator)
    assert problem_line[0] == "problem" and len(problem_line) == 2
    yield problem_line[1]
    domain_line = next(iterator)
    assert domain_line[0] == ":domain" and len(domain_line) == 2
    yield domain_line[1]

    requirements_opt = next(iterator)
    if requirements_opt[0] == ":requirements":
        requirements = requirements_opt[1:]
        objects_opt = next(iterator)
    else:
        requirements = []
        objects_opt = requirements_opt
    yield pddl.Requirements(requirements)

    if objects_opt[0] == ":objects":
        yield parse_typed_list(objects_opt[1:])
        init = next(iterator)
    else:
        yield []
        init = objects_opt

    assert init[0] == ":init"
    initial = []
    num_initial = []
    initial_true = set()
    initial_false = set()
    initial_assignments = dict()
    for fact in init[1:]:
#        if DEBUG: print("Analyzing fact %s"%fact)
        if fact[0] == "=":
#            if DEBUG: print("Fact is '=' assignment")
            try:
                assignment = parse_assignment(fact)
            except ValueError as e:
                raise SystemExit("Error in initial state specification\n" +
                                 "Reason: %s." %  e)
            if not isinstance(assignment.expression,
                              pddl.NumericConstant):
                raise SystemExit("Illegal assignment in initial state " +
                    "specification:\n%s" % assignment)
            if assignment.fluent in initial_assignments:
                prev = initial_assignments[assignment.fluent]
                if assignment.expression == prev.expression:
                    print("Warning: %s is specified twice" % assignment,
                          "in initial state specification")
                else:
                    raise SystemExit("Error in initial state specification\n" +
                                     "Reason: conflicting assignment for " +
                                     "%s." %  assignment.fluent)
            else:
                initial_assignments[assignment.fluent] = assignment
                num_initial.append(assignment)
        elif fact[0] == "not":
#            if DEBUG: print("Fact is negation")
            atom = pddl.Atom(fact[1][0], fact[1][1:])
            check_atom_consistency(atom, initial_false, initial_true, False)
            initial_false.add(atom)
        else:
#            if DEBUG: print("Fact is positive atom")
            atom = pddl.Atom(fact[0], fact[1:])
            check_atom_consistency(atom, initial_true, initial_false)
            initial_true.add(atom)
#    if DEBUG: print("initial_true is %s"%sorted(initial_true))
    initial.extend(initial_true)
    if DEBUG: initial.sort(key = lambda x: x.__repr__()) # makes the result deterministic
#    print("initial is s%s"%initial)
    yield initial
    # yielding num_initial delayed (metric section may add artificial fluent 'total-cost'
        
    # goal
    goal = next(iterator)
    assert goal[0] == ":goal" and len(goal) == 2

    #metric
    metric_symbol = '<' # minimize
    metric_expression = None # unit cost 
    for entry in iterator:
        if entry[0] == ":metric":
            if entry[1]=="minimize":
                metric_symbol = '<'
            else:
                assert entry[1]=="maximize", "Unknown metric, 'minimize' or 'maximize' expected."
                metric_symbol = '>'
#            try: 
            metric_expression = parse_expression(entry[2])
#            except:
#                raise SystemExit("Cannot parse metric expression in %s" % entry[2])           
    if metric_expression is None:
        metric_expression = pddl.PrimitiveNumericExpression('total-cost', [], 'I')        
        num_initial.append(parse_assignment(['=', 'total-cost', 0]))

#    print("parsing_functions yields num_initial:", num_initial)
    yield num_initial
    yield parse_condition(goal[1], type_dict, predicate_dict)    
    yield (metric_symbol, metric_expression)

    for entry in iterator:
        assert False, entry

def isFloat(astring):
    try:
        float(astring)
    except ValueError:
        return False
    return True

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
        if element.name in seen:
            errors.append(element)
#            errors.append(errmsg % element)
        else:
            seen.add(element.name)
    if errors:
        msg = "\n".join([errmsg % e for e in errors]) + "\n" + finalmsg
        print(msg, file=sys.stderr)
        for e in errors:
            elements.remove(e)
#        raise SystemExit(msg)
