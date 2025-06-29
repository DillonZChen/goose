#! /usr/bin/env python

from __future__ import print_function

from collections import defaultdict

import build_model
import pddl_to_prolog
import pddl
import timers
import normalize

DEBUG = False


def get_fluent_facts(task, model):
    fluent_predicates = normalize.get_fluent_predicates(task)
    return set([fact for fact in model if fact.predicate in fluent_predicates])

def get_fluent_functions(model):
    fluent_pnes = set()
    for atom in model:
        if isinstance(atom.predicate, pddl.PrimitiveNumericExpression):
#             print("instantiate.get_fluent_functions adds atom" ,atom)            
#            assert(atom.predicate.ntype != 'U') , "Atom is falsch %s" % atom
            fluent_pnes.add(pddl.PrimitiveNumericExpression(atom.predicate.symbol,atom.args, atom.predicate.ntype))    
    return fluent_pnes

def get_objects_by_type(typed_objects, types):
    result = defaultdict(list)
    supertypes = {}
    for type in types:
        supertypes[type.name] = type.supertype_names
    for obj in typed_objects:
        result[obj.type_name].append(obj.name)
        for type in supertypes[obj.type_name]:
            result[type].append(obj.name)
    return result

def init_function_values(init_facts):
    assignments = [func_assign for func_assign in init_facts 
                        if isinstance(func_assign, pddl.FunctionAssignment)]
    init_values = {}
    for assignment in assignments:
    
        init_values[assignment.fluent] = assignment.expression
    return init_values

def instantiate(task, model):
    relaxed_reachable = False
    fluent_facts = get_fluent_facts(task, model)
#     print("Fluent facts = ")
#     for ffct in sorted(fluent_facts, key= lambda fact:repr(fact)):
#         print("- %s" % ffct) 
    
    # NFD: fluents adds numeric fluents
    fluent_functions = get_fluent_functions(model)
#     print("instantiate.instatiate fluent_functions : %s" % len(fluent_functions))
#     for fluf in fluent_functions:
#         fluf.dump()      
            
    init_facts = set(task.init + task.num_init)
    init_function_vals = init_function_values(init_facts)
    # Determine initial facts, that are not fluents => constant facts, that a module might need
    init_constant_fluents = set(init_function_vals)
    init_constant_fluents.difference_update(fluent_functions)   # all fluents that are in init, but are NOT a fluent -> constant
    # Now get the assigned values from the init_facts for the constant fluents
    init_constant_numeric_facts = set()   # This will hold Assigns that assign the fluents
    for i in init_constant_fluents:
        for j in init_facts:
            if isinstance(j, pddl.Assign):
                if isinstance(j.fluent, pddl.PrimitiveNumericExpression):
                    if j.fluent is i:     # Assign in init_fact assign this (i) fluent
                        init_constant_numeric_facts.add(j) 
     
    # Now get predicates that are in init, but are not fluent_facts
    init_constant_predicate_facts = set()
    for i in init_facts:
        if isinstance(i, pddl.Atom):  # do NOT consider PNEs, etc.
            if i not in fluent_facts:   # only consider non-fluents
                if i.predicate is not "=":    # hack to remove the intermediate '=' fluents
                    init_constant_predicate_facts.add(i)
    
    type_to_objects = get_objects_by_type(task.objects, task.types)

    instantiated_actions = []
    instantiated_axioms = []
    instantiated_numeric_axioms = set()
    new_constant_numeric_axioms = set()
    reachable_action_parameters = defaultdict(list)
    instantiated_modules = set()    
                
    for atom in model:
        if isinstance(atom.predicate, pddl.Action):
            action = atom.predicate
#            print("About to instantiate Action: ", action)            
            parameters = action.parameters
            inst_parameters = atom.args[:len(parameters)]
            # Note: It's important that we use the action object
            # itself as the key in reachable_action_parameters (rather
            # than action.name) since we can have multiple different
            # actions with the same name after normalization, and we
            # want to distinguish their instantiations.
            reachable_action_parameters[action].append(inst_parameters)
            variable_mapping = dict([(par.name, arg)
                                     for par, arg in zip(parameters, atom.args)])
            inst_action = action.instantiate(variable_mapping, init_facts, fluent_facts,
                                       init_function_vals, fluent_functions,
                                       task, new_constant_numeric_axioms, instantiated_modules, type_to_objects)
            if inst_action:
                instantiated_actions.append(inst_action)
        elif isinstance(atom.predicate, pddl.Axiom):
            axiom = atom.predicate
            variable_mapping = dict([(par.name, arg)
                                     for par, arg in zip(axiom.parameters, atom.args)])
            inst_axiom = axiom.instantiate(variable_mapping, init_facts, fluent_facts, init_function_vals,
                    fluent_functions, task, new_constant_numeric_axioms, instantiated_modules)
            if inst_axiom:
                instantiated_axioms.append(inst_axiom)
        elif isinstance(atom.predicate, pddl.NumericAxiom):
            axiom = atom.predicate
            variable_mapping = dict([(par, arg) 
                                     for par, arg in zip(axiom.parameters, atom.args)])
            inst_axiom = axiom.instantiate(variable_mapping, fluent_functions, init_function_vals, 
                                     task, new_constant_numeric_axioms)
            instantiated_numeric_axioms.add(inst_axiom)                                
        elif atom.predicate == "@goal-reachable":
            relaxed_reachable = True 
    instantiated_numeric_axioms |= new_constant_numeric_axioms
    return (relaxed_reachable, fluent_facts, fluent_functions,
            instantiated_actions, sorted(instantiated_axioms), instantiated_numeric_axioms,
            init_constant_predicate_facts, init_constant_numeric_facts,
            reachable_action_parameters) 
    
def explore(task):
    if DEBUG: print("DEBUG: Exploring Task Step [1]: create logic program 'prog'")
    prog = pddl_to_prolog.translate(task)
#     prog.dump()    
    if DEBUG: print("DEBUG: Exploring Task Step [2]: build model 'model'")    
    model = build_model.compute_model(prog)   
#     print("instantiate.explore task dumps task")
#     task.dump()
    if DEBUG: print("DEBUG: Exploring Task Step [3]: instantiate model")    
    with timers.timing("Completing instantiation"):
        return instantiate(task, model)

if __name__ == "__main__":
    import pddl_parser
    task = pddl_parser.open()
    relaxed_reachable, atoms, actions, axioms, _ = explore(task)
    print("goal relaxed reachable: %s" % relaxed_reachable)
    print("%d atoms:" % len(atoms))
    for atom in atoms:
        print(" ", atom)
    print()
    print("%d actions:" % len(actions))
    for action in actions:
        action.dump()
        print()
    print()
    print("%d axioms:" % len(axioms))
    for axiom in axioms:
        axiom.dump()
        print()
