#! /usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import sys


def python_version_supported():
    major, minor = sys.version_info[:2]
    return (major == 2 and minor >= 7) or (major, minor) >= (3, 2)

if not python_version_supported():
    sys.exit("Error: Translator only supports Python >= 2.7 and Python >= 3.2.")


from collections import defaultdict
from copy import deepcopy
from itertools import product

import axiom_rules
import fact_groups
import instantiate
import normalize
import numeric_axiom_rules
import options
import pddl
import pddl_parser
import sas_tasks
import simplify
import timers
import tools

# TODO: The translator may generate trivial derived variables which are always
# true, for example if there is a derived predicate in the input that only
# depends on (non-derived) variables which are detected as always true.
# Such a situation was encountered in the PSR-STRIPS-DerivedPredicates domain.
# Such "always-true" variables should best be compiled away, but it is
# not clear what the best place to do this should be. Similar
# simplifications might be possible elsewhere, for example if a
# derived variable is synonymous with another variable (derived or
# non-derived).

DEBUG = True

simplified_effect_condition_counter = 0
added_implied_precondition_counter = 0


def strips_to_sas_dictionary(groups, num_axioms, num_axiom_map, num_fluents, assert_partial, include_numeric=True):       
    dictionary = {}
    numeric_dictionary = {}
#     print("Creating_strips_to_sas_dictionary. Numeric Axioms:")
#     for nax in num_axioms:
#         nax.dump()
#     print("Numeric Action Map for S2S is")
#     for entry in num_axiom_map:
#         print("%s -> %s" % (entry, num_axiom_map[entry]))
#    print("S2S from groups:")
#    for element in groups:
#        print(element)  
    for var_no, group in enumerate(groups):
        for val_no, atom in enumerate(group):
            dictionary.setdefault(atom, []).append((var_no, val_no))
    if assert_partial:
        assert all(len(sas_pairs) == 1
                   for sas_pairs in dictionary.values())
    ranges = [len(group) + 1 for group in groups]
    
    num_count = 0
    if include_numeric:    
        redundant_axioms = []
        for axiom in num_axioms:
            if axiom.effect in num_axiom_map:
                redundant_axioms.append(axiom.effect)
            else:
#                print("AE adding %s -> %d" % (axiom.effect, num_count))
                numeric_dictionary[axiom.effect]= num_count
                num_count += 1
        for axiom_effect in redundant_axioms:
                numeric_dictionary[axiom_effect] = numeric_dictionary[num_axiom_map[axiom_effect].effect]        
        fluent_list = list(num_fluents)
#         print("List of all numeric fluents:")
#         for element in fluent_list:
#             print("   %s" % element)
        fluent_list.sort(lambda x,y: cmp(str(x), str(y)))
        for fluent in fluent_list: # are partially contained in num_axiom
            if fluent not in numeric_dictionary:
#                print("Num-Variable #%d \t-> %s" % (num_count, fluent))
                numeric_dictionary[fluent]=num_count
                num_count += 1
    # ranges: the range of each FDR variable in groups (including "none of those")
    # dictionary: mapping from grounded facts to the index of the corresponding FDR variable-value pair
    #             e.g. Atom on(A, B) mapsto (5, 2) --> fact #2 of the (selected mutex) group # 5
    # numeric dictionary: maps the name of numeric ground facts to the corresponding real valued variable
    #             no ranges are given for such variables as the range is "infinite" (no finite domain representation)
    #             (in reality the range is obviously a finite number of 2^64 bits (double representation))
    return ranges, dictionary, num_count, numeric_dictionary


def translate_strips_conditions_aux(conditions, dictionary, ranges, numeric_dictionary, comparison_axioms, mutex_check = False):
    condition = {}
    comp_axiom_dict = comparison_axioms[0] 
    sas_comp_axioms = comparison_axioms[1]
    for fact in conditions:
        if (isinstance(fact,pddl.FunctionComparison) or 
            isinstance(fact,pddl.NegatedFunctionComparison)):
#            print("comparison condition in translate strips condition aux:")
            if fact not in dictionary:
#                print("Fact is not in dictionary")
                for part in fact.parts: # TODO: remove this assertion check when not debugging  
                    assert part in numeric_dictionary                    
                parts = [numeric_dictionary[part] for part in fact.parts]
                key = (fact.comparator, tuple(parts))
                negated = fact.negated
                if key in comp_axiom_dict:
#                    print("key %s ist im comp_axiom_dict" % [key])
#                    print(comp_axiom_dict)
                    fact = comp_axiom_dict[key]
                    if negated:
#                       print("(nach dem negieren)")
                        fact = fact.negate()
                else:
                    axiom = sas_tasks.SASCompareAxiom(fact.comparator, 
                                                      parts, len(ranges)) 
                    if negated:
                        negfact = fact
                        posfact = fact.negate()
                    else:
                        posfact = fact
                        negfact = fact.negate()

                    if not mutex_check: # if mutexes are checked the axioms will not be added because the
                            # strips to sas dictionary would not be updated properly otherwise 
                        sas_comp_axioms.append(axiom)
                        comp_axiom_dict[key] = posfact
                    dictionary.setdefault(posfact,[]).append((len(ranges), 0))
                    dictionary.setdefault(negfact,[]).append((len(ranges), 1))
                    ranges.append(3)
#            print (dictionary[fact])
            var, val = dictionary[fact][0]
            if (var in condition and val not in condition[var]):
                # Conflicting conditions on this variable: Operator invalid.
                return None
            condition[var] = set([val])        
        else:
            if fact.negated:
                # we handle negative conditions later, because then we
                # can recognize when the negative condition is already
                # ensured by a positive condition
                continue
            for var, val in dictionary.get(fact, ()):
                # The default () here is a bit of a hack. For goals (but
                # only for goals!), we can get static facts here. They
                # cannot be statically false (that would have been
                # detected earlier), and hence they are statically true
                # and don't need to be translated.
                # TODO: This would not be necessary if we dealt with goals
                # in the same way we deal with operator preconditions etc.,
                # where static facts disappear during grounding. So change
                # this when the goal code is refactored (also below). (**)
                if (condition.get(var) is not None and
                    val not in condition.get(var)):
                    # Conflicting conditions on this variable: Operator invalid.
                    return None
                condition[var] = set([val])

    def number_of_values(var_vals_pair):
        var, vals = var_vals_pair
        return len(vals)

    for fact in conditions:
        if (isinstance(fact,pddl.FunctionComparison) or 
            isinstance(fact,pddl.NegatedFunctionComparison)):
            continue
        if fact.negated:
           ## Note  Here we use a different solution than in Sec. 10.6.4
           ##       of the thesis. Compare the last sentences of the third
           ##       paragraph of the section.
           ##       We could do what is written there. As a test case,
           ##       consider Airport ADL tasks with only one airport, where
           ##       (occupied ?x) variables are encoded in a single variable,
           ##       and conditions like (not (occupied ?x)) do occur in
           ##       preconditions.
           ##       However, here we avoid introducing new derived predicates
           ##       by treat the negative precondition as a disjunctive
           ##       precondition and expanding it by "multiplying out" the
           ##       possibilities.  This can lead to an exponential blow-up so
           ##       it would be nice to choose the behaviour as an option.
            done = False
            new_condition = {}
            atom = pddl.Atom(fact.predicate, fact.args)  # force positive
            for var, val in dictionary.get(atom, ()):
                # see comment (**) above
                poss_vals = set(range(ranges[var]))
                poss_vals.remove(val)

                if condition.get(var) is None:
                    assert new_condition.get(var) is None
                    new_condition[var] = poss_vals
                else:
                    # constrain existing condition on var
                    prev_possible_vals = condition.get(var)
                    done = True
                    prev_possible_vals.intersection_update(poss_vals)
                    if len(prev_possible_vals) == 0:
                        # Conflicting conditions on this variable:
                        # Operator invalid.
                        return None

            if not done and len(new_condition) != 0:
                # we did not enforce the negative condition by constraining
                # an existing condition on one of the variables representing
                # this atom. So we need to introduce a new condition:
                # We can select any from new_condition and currently prefer the
                # smallest one.
                candidates = sorted(new_condition.items(), key=number_of_values)
                var, vals = candidates[0]
                condition[var] = vals

    def multiply_out(condition):  # destroys the input
        sorted_conds = sorted(condition.items(), key=number_of_values)
        flat_conds = [{}]
        for var, vals in sorted_conds:
            if len(vals) == 1:
                for cond in flat_conds:
                    cond[var] = vals.pop()  # destroys the input here
            else:
                new_conds = []
                for cond in flat_conds:
                    for val in vals:
                        new_cond = deepcopy(cond)
                        new_cond[var] = val
                        new_conds.append(new_cond)
                flat_conds = new_conds
        return flat_conds

    return multiply_out(condition)


def translate_strips_conditions(conditions, dictionary, ranges, numeric_dictionary,
                                mutex_dict, mutex_ranges, comp_axioms):
    if not conditions:
        return [{}]  # Quick exit for common case.

    # Check if the condition violates any mutexes.
    if translate_strips_conditions_aux(conditions, mutex_dict, mutex_ranges, 
									numeric_dictionary, comp_axioms, True) is None:
        return None

    return translate_strips_conditions_aux(conditions, dictionary, ranges, numeric_dictionary, comp_axioms)


def translate_strips_operator(operator, dictionary, ranges, numeric_dictionary, mutex_dict,
                              mutex_ranges, implied_facts, comp_axioms, num_vals, relevant_numeric_vars):
    conditions = translate_strips_conditions(operator.precondition, dictionary,
                                             ranges, numeric_dictionary, mutex_dict, mutex_ranges, comp_axioms)
    
    if conditions is None:
        return []
    sas_operators = []
    for condition in conditions:
        op = translate_strips_operator_aux(operator, dictionary, ranges, numeric_dictionary,
                                           mutex_dict, mutex_ranges,
                                           implied_facts, condition, comp_axioms, num_vals, relevant_numeric_vars)
        if op is not None:
            sas_operators.append(op)
    return sas_operators


def negate_and_translate_condition(condition, dictionary, ranges, numeric_dict, mutex_dict,
                                   mutex_ranges, comp_axioms):
    # condition is a list of lists of literals (DNF)
    # the result is the negation of the condition in DNF in
    # finite-domain representation (a list of dictionaries that map
    # variables to values)
    negation = []
    if [] in condition:  # condition always satisfied
        return None  # negation unsatisfiable
    for combination in product(*condition):
        cond = [l.negate() for l in combination]
        cond = translate_strips_conditions(cond, dictionary, ranges, numeric_dict,
                                           mutex_dict, mutex_ranges, comp_axioms)
        if cond is not None:
            negation.extend(cond)
    return negation if negation else None


def translate_strips_operator_aux(operator, dictionary, ranges, numeric_dictionary, mutex_dict,
                                  mutex_ranges, implied_facts, condition, comp_axioms, num_vals, relevant_numeric):
#    if DEBUG: print("Translating strips operator >%s< to SAS" % operator.name)
#     for entry in dictionary:
#         print("%s \t-> %s" % (entry, dictionary[entry]))
#     print("implied facts = %s" % implied_facts)
#     print("Numeric Dictionary = ")
#     for entry in numeric_dictionary:
#         print("%s \t-> %s" % (entry, numeric_dictionary[entry]))

    # collect all add effects
    effects_by_variable = defaultdict(lambda: defaultdict(list))
    # effects_by_variables: var -> val -> list(FDR conditions)
    add_conds_by_variable = defaultdict(list)
    for conditions, fact in operator.add_effects:
        eff_condition_list = translate_strips_conditions(conditions, dictionary,
                                                         ranges, numeric_dictionary, mutex_dict,
                                                         mutex_ranges, comp_axioms)
        if eff_condition_list is None:  # Impossible condition for this effect.
            continue
        for var, val in dictionary[fact]:
            effects_by_variable[var][val].extend(eff_condition_list)
            add_conds_by_variable[var].append(conditions)
#    print("the following defaultdict was created and contains the add effects")
#    print(effects_by_variable)
#    for key in effects_by_variable:
#        print("%s \t-> %s" % (key,effects_by_variable[key]))

    # collect all del effects
    del_effects_by_variable = defaultdict(lambda: defaultdict(list))
    for conditions, fact in operator.del_effects:
        eff_condition_list = translate_strips_conditions(conditions, dictionary,
                                                         ranges, numeric_dictionary, mutex_dict,
                                                         mutex_ranges, comp_axioms)
        if eff_condition_list is None:  # Impossible condition for this effect.
            continue
        for var, val in dictionary[fact]:
            del_effects_by_variable[var][val].extend(eff_condition_list)
#    print("the following dictionary contains (conditional) delete effects")
#    for key in del_effects_by_variable:
#        print("%s \t-> %s" % (key,del_effects_by_variable[key]))

    # collect all (numeric) assignment effects
    ass_effects_by_variable = defaultdict(lambda: defaultdict(list))
    # effects_by_variables: num_var -> dict( (assexpr, assvar) -> [conditions])
    # num_var: affected variable
    # assexpr: "= + - * /
      
    for conditions, assignment in operator.assign_effects: 
        eff_condition_list = translate_strips_conditions(conditions, dictionary,
                                                         ranges, numeric_dictionary, mutex_dict,
                                                         mutex_ranges, comp_axioms)
        if eff_condition_list is None:  # Impossible condition for this effect.
            continue
        #assert(assignment.expression in numeric_dictionary), "%s not in numeric dictionary" % assignment.expression
        if assignment.expression in numeric_dictionary:
            ass_effects_by_variable[numeric_dictionary[assignment.fluent]][(assignment.symbol, numeric_dictionary[assignment.expression])].extend(eff_condition_list)
    # add effect var=none_of_those for all del effects with the additional
    # condition that the deleted value has been true and no add effect triggers
    for var in del_effects_by_variable:
        no_add_effect_condition = negate_and_translate_condition(
            add_conds_by_variable[var], dictionary, ranges, numeric_dictionary, mutex_dict,
            mutex_ranges, comp_axioms)
        if no_add_effect_condition is None:  # there is always an add effect
            continue
        none_of_those = ranges[var] - 1
        for val, conds in del_effects_by_variable[var].items():
            for cond in conds:
                # add guard
                if var in cond and cond[var] != val:
                    continue  # condition inconsistent with deleted atom
                cond[var] = val
                # add condition that no add effect triggers
                for no_add_cond in no_add_effect_condition:
                    new_cond = dict(cond)
                    # This is a rather expensive step. We try every no_add_cond
                    # with every condition of the delete effect and discard the
                    # overal combination if it is unsatisfiable. Since
                    # no_add_effect_condition is precomputed it can contain many
                    # no_add_conds in which a certain literal occurs. So if cond
                    # plus the literal is already unsatisfiable, we still try
                    # all these combinations. A possible optimization would be
                    # to re-compute no_add_effect_condition for every delete
                    # effect and to unfold the product(*condition) in
                    # negate_and_translate_condition to allow an early break.
                    for cvar, cval in no_add_cond.items():
                        if cvar in new_cond and new_cond[cvar] != cval:
                            # the del effect condition plus the deleted atom
                            # imply that some add effect on the variable
                            # triggers
                            break
                        new_cond[cvar] = cval
                    else:
                        effects_by_variable[var][none_of_those].append(new_cond)

    return build_sas_operator(operator.name, condition, effects_by_variable, ass_effects_by_variable,
                              operator.cost, ranges, implied_facts, relevant_numeric) 

def build_sas_operator(name, condition, effects_by_variable, ass_effects_by_variable, deprecated_cost, ranges,
                       implied_facts, relevant_numeric_variables):
#    print("build_sas_operator with relevant vars:", relevant_numeric_variables)
#    if DEBUG: print("Building SAS Operator %s with %d logic and %d numeric effects" % (name, len(effects_by_variable),len(ass_effects_by_variable)))  
    if options.add_implied_preconditions:
        implied_precondition = set()
        for fact in condition.items():
            implied_precondition.update(implied_facts[fact])
    prevail_and_pre = dict(condition)
    pre_post = []
    num_pre_post = []
    for var in effects_by_variable:
        orig_pre = condition.get(var, -1)
        added_effect = False
        for post, eff_conditions in effects_by_variable[var].items():
            pre = orig_pre
            # if the effect does not change the variable value, we ignore it
            if pre == post:
                continue
            eff_condition_lists = [sorted(eff_cond.items())
                                   for eff_cond in eff_conditions]
            if ranges[var] == 2:
                # Apply simplifications for binary variables.
                if prune_stupid_effect_conditions(var, post,
                                                  eff_condition_lists):
                    global simplified_effect_condition_counter
                    simplified_effect_condition_counter += 1
                if (options.add_implied_preconditions and pre == -1 and
                        (var, 1 - post) in implied_precondition):
                    global added_implied_precondition_counter
                    added_implied_precondition_counter += 1
                    pre = 1 - post
            for eff_condition in eff_condition_lists:
                # we do not need to represent a precondition as effect condition
                # and we do not want to keep an effect whose condition contradicts
                # a pre- or prevail condition
                filtered_eff_condition = []
                eff_condition_contradicts_precondition = False
                for variable, value in eff_condition:
                    if variable in prevail_and_pre:
                        if prevail_and_pre[variable] != value:
                            eff_condition_contradicts_precondition = True
                            break
                    else:
                        filtered_eff_condition.append((variable, value))
                if eff_condition_contradicts_precondition:
                    continue
                pre_post.append((var, pre, post, filtered_eff_condition))
                added_effect = True
        if added_effect:
            # the condition on var is not a prevail condition but a
            # precondition, so we remove it from the prevail condition
            condition.pop(var, -1)

    for numvar in ass_effects_by_variable:
#        if DEBUG: print("Numeric effect on numvar = %s" % numvar)
#        orig_pre = condition.get(numvar, -1)
#        assert orig_pre == -1 # numeric variables cannot occur in preconditions (instead propositional variables are derived by axioms)
#    numeric variables and logic variables are not stored in the same data structures, but they *do* can have the same index within their
#    respective arrays
        for (ass_op, post_var), eff_conditions in ass_effects_by_variable[numvar].items():
#            if DEBUG: print("assignment operator : >%s<" % ass_op)
#            if DEBUG: print("post condition variable : %d" % post_var)
            # otherwise the condition on numvar is not a prevail condition but a
            # precondition, so we remove it from the prevail condition
            eff_condition_lists = [sorted(eff_cond.items())
                                   for eff_cond in eff_conditions]
            for eff_condition in eff_condition_lists:
                # we do not need to represent a precondition as effect condition
                num_pre_post.append((numvar, ass_op, post_var, eff_condition))
#    if DEBUG: print("pre_post = %s " % pre_post)
#    if DEBUG: print("num_pre_post = %s " % num_pre_post)
    if not pre_post: # build only operators with at least one regular effect (not instrumentation)
        irrelevant = True 
        for effect in  num_pre_post:
#            print ("numeric effect ", effect) 
            if effect[0] in relevant_numeric_variables:
#                print("RELEVANT! because ", effect[0], " is in ", relevant_numeric_variables)
                irrelevant = False
                break            
        if irrelevant:    
#            if DEBUG: print("operator is noop - return None")
            return None
    prevail = list(condition.items())
    return sas_tasks.SASOperator(name, prevail, pre_post, num_pre_post, deprecated_cost)


def prune_stupid_effect_conditions(var, val, conditions):
    ## (IF <conditions> THEN <var> := <val>) is a conditional effect.
    ## <var> is guaranteed to be a binary variable.
    ## <conditions> is in DNF representation (list of lists).
    ##
    ## We simplify <conditions> by applying two rules:
    ## 1. Conditions of the form "var = dualval" where var is the
    ##    effect variable and dualval != val can be omitted.
    ##    (If var != dualval, then var == val because it is binary,
    ##    which means that in such situations the effect is a no-op.)
    ## 2. If conditions contains any empty list, it is equivalent
    ##    to True and we can remove all other disjuncts.
    ##
    ## returns True when anything was changed
    if conditions == [[]]:
        return False  # Quick exit for common case.
    assert val in [0, 1]
    dual_fact = (var, 1 - val)
    simplified = False
    for condition in conditions:
        # Apply rule 1.
        while dual_fact in condition:
            # print "*** Removing dual condition"
            simplified = True
            condition.remove(dual_fact)
        # Apply rule 2.
        if not condition:
            conditions[:] = [[]]
            simplified = True
            break
    return simplified


def translate_strips_axiom(axiom, dictionary, ranges, num_dict, mutex_dict, mutex_ranges, comp_ax):
    conditions = translate_strips_conditions(axiom.condition, dictionary,
                                             ranges, num_dict, mutex_dict, mutex_ranges, comp_ax)
    if conditions is None:
        return []
    if axiom.effect.negated:
        [(var, _)] = dictionary[axiom.effect.positive()]
        effect = (var, ranges[var] - 1)
    else:
        [effect] = dictionary[axiom.effect]
    axioms = []
    for condition in conditions:
        axioms.append(sas_tasks.SASAxiom(condition.items(), effect))
    return axioms

def translate_numeric_axiom(axiom, prop_dictionary, num_dictionary):
    effect = num_dictionary.get(axiom.effect)
    op = axiom.op
    parts = []
    for part in axiom.parts:
        if isinstance(part, pddl.PrimitiveNumericExpression):
            parts.append(num_dictionary.get(part))
        else: # part is PropositionalNumericAxiom
            parts.append(prop_dictionary.get(part.effect)[0])
    return sas_tasks.SASNumericAxiom(op, parts, effect)



def translate_strips_operators(actions, strips_to_sas, ranges, numeric_strips_to_sas, mutex_dict,
                               mutex_ranges, implied_facts, comp_axioms, num_vals, relevant_numeric_vars):
    result = []
    for action in actions:
        sas_ops = translate_strips_operator(action, strips_to_sas, ranges, numeric_strips_to_sas, mutex_dict, mutex_ranges, implied_facts, comp_axioms, num_vals, relevant_numeric_vars)                                                                      
        result.extend(sas_ops)
    return result


def translate_strips_axioms(axioms, strips_to_sas, ranges, num_dict, mutex_dict,
                            mutex_ranges, comp_ax):
    result = []
    for axiom in axioms:
        sas_axioms = translate_strips_axiom(axiom, strips_to_sas, ranges, num_dict,
                                            mutex_dict, mutex_ranges, comp_ax)
        result.extend(sas_axioms)
    return result

def add_key_to_comp_axioms(comparison_axioms, translation_key): # Adds a "human" readable name to each propositional variable    
    for axiom in comparison_axioms[1]:
        value_list = []
        assert axiom.effect == len(translation_key), "current effect %s != next variable %s" % (axiom.effect, len(translation_key))
        value_list.append(str(axiom))
        value_list.append(str(axiom.invert_comparator()))
        value_list.append('<none of those>')
        translation_key.append(value_list)

def dump_task(init, goals, actions, axioms, axiom_layer_dict):
    old_stdout = sys.stdout
    with open("output.dump", "w") as dump_file:
        sys.stdout = dump_file
        print("Initial state")
        for atom in init:
            print(atom)
        print()
        print("Goals")
        for goal in goals:
            print(goal)
        for action in actions:
            print()
            print("Action")
            action.dump()
        for axiom in axioms:
            print()
            print("Axiom")
            axiom.dump()
        print()
        print("Axiom layers")
        for atom, layer in axiom_layer_dict.items():
            print("%s: layer %d" % (atom, layer))
    sys.stdout = old_stdout


def translate_task(strips_to_sas, ranges, translation_key, numeric_strips_to_sas, num_count,
                   mutex_dict, mutex_ranges, mutex_key,
                   init, num_init, goal_list, global_constraint,
                   actions, axioms, num_axioms,                    
                   num_axioms_by_layer, 
                   num_axiom_map, const_num_axioms,
                   metric, implied_facts, init_constant_predicates, init_constant_numerics):    
    with timers.timing("Processing axioms", block=True):
        axioms, axiom_init, axiom_layer_dict = axiom_rules.handle_axioms(
            actions, axioms, goal_list, global_constraint)
    init = init + axiom_init    
    if options.dump_task:
        # Remove init facts that don't occur in strips_to_sas: they're constant.
        nonconstant_init = filter(strips_to_sas.get, init)
        dump_task(nonconstant_init, goal_list, actions, axioms, axiom_layer_dict)

    init_values = [rang - 1 for rang in ranges]
    # Closed World Assumption: Initialize to "range - 1" == Nothing.
    for fact in init:
        pairs = strips_to_sas.get(fact, [])  # empty for static init facts
        for var, val in pairs:
            curr_val = init_values[var]
            if curr_val != ranges[var] - 1 and curr_val != val:
                assert False, "Inconsistent init facts! [fact = %s]" % fact
            init_values[var] = val
  
    comparison_axioms = [{},[]]
    goal_dict_list = translate_strips_conditions(goal_list, strips_to_sas, ranges, numeric_strips_to_sas,
                                                 mutex_dict, mutex_ranges, comparison_axioms)
    global_constraint_dict_list = translate_strips_conditions([global_constraint], strips_to_sas, ranges, numeric_strips_to_sas,
                                                 mutex_dict, mutex_ranges, comparison_axioms)
#    print("goal_dict_list = %s" % goal_dict_list)
#    print("comparison_axioms = %s" %comparison_axioms)
    if goal_dict_list is None:
        # "None" is a signal that the goal_list is unreachable because it
        # violates a mutex.
        return unsolvable_sas_task("Goal violates a mutex")

    assert len(goal_dict_list) == 1, "Negative goal not supported"
    ## we could substitute the negative goal literal in
    ## normalize.substitute_complicated_goal, using an axiom. We currently
    ## don't do this, because we don't run into this assertion, if the
    ## negative goal is part of finite domain variable with only two
    ## values, which is most of the time the case, and hence refrain from
    ## introducing axioms (that are not supported by all heuristics)
    goal_pairs = list(goal_dict_list[0].items())
    if not goal_pairs:
        return solvable_sas_task("Empty goal")
    sas_goal = sas_tasks.SASGoal(goal_pairs)

    assert len(global_constraint_dict_list) == 1 # the key is the axiom fluent, the value (0) the evaluation to true

    num_init_values = [0.0]*num_count # initialize numeric varialbes with 0.0
#     if DEBUG:
#         print("Strips-to-sas dict is")     
#         for entry in strips_to_sas:
#             print("%s -> %s"%(entry, strips_to_sas[entry]))
#         print("Numeric Strips-to-sas dict is")               
#         for entry in numeric_strips_to_sas:
#             print("%s -> %s"%(entry,numeric_strips_to_sas[entry]))

    relevant_numeric = []        
    for fact in num_init:        
        var = numeric_strips_to_sas.get(fact.fluent,-1)
        if var > -1:
            val = fact.expression.value        
            num_init_values[var]=val
            if fact.fluent.ntype == 'R':
                # the corresponding numeric variable is "regular" and therefore relevant
                relevant_numeric.append(var)

    operators = translate_strips_operators(actions, strips_to_sas, ranges, numeric_strips_to_sas,
                                           mutex_dict, mutex_ranges,
                                           implied_facts, comparison_axioms, num_init_values, relevant_numeric)

    axioms = translate_strips_axioms(axioms, strips_to_sas, ranges, numeric_strips_to_sas, mutex_dict,
                                     mutex_ranges, comparison_axioms)

    sas_num_axioms = [translate_numeric_axiom(axiom,strips_to_sas, numeric_strips_to_sas) for axiom in num_axioms 
                      if axiom not in const_num_axioms and
                      axiom.effect not in num_axiom_map]


    axiom_layers = [-1] * len(ranges) # default axiom layer is -1           
    ## each numeric axiom gets its own layer (a wish of a colleague for 
    ## knowledge compilation or search. If you use only the translator,
    ## you can change this)
    num_axiom_layers = [-1] * num_count
    num_axiom_layer = 0
    for layer in num_axioms_by_layer:
        num_axioms_by_layer[layer].sort(lambda x,y: cmp(x.name,y.name))
        for axiom in num_axioms_by_layer[layer]:
            if axiom.effect not in num_axiom_map:
                var = numeric_strips_to_sas[axiom.effect]
                if layer == -1:
                    num_axiom_layers[var] = -1
                else:
                    num_axiom_layers[var] = num_axiom_layer
                    num_axiom_layer += 1
#    print("comparison_axioms = %s" %comparison_axioms)
    comp_axiom_init = [2] * len(comparison_axioms[1]) # initializing comparison axioms with value 3 (none of those)
    init_values.extend(comp_axiom_init) # 

    for axiom in comparison_axioms[1]:
        axiom_layers[axiom.effect] = num_axiom_layer
    for atom, layer in axiom_layer_dict.iteritems():
        assert layer >= 0
        [(var, val)] = strips_to_sas[atom]
        axiom_layers[var] = layer + num_axiom_layer + 1
    add_key_to_comp_axioms(comparison_axioms, translation_key)    
    variables = sas_tasks.SASVariables(ranges, axiom_layers, translation_key, num_axiom_layer)
    
    num_variables = [-1] * num_count
    num_var_types = ['U'] * num_count # variable type is unknown
    for entry in numeric_strips_to_sas:
        num_variables[numeric_strips_to_sas[entry]] = entry             
        num_var_types[numeric_strips_to_sas[entry]] = entry.ntype       
    assert num_count == len(num_variables), "%d nc <-> variables %d"%(num_count,len(num_variables))
    
    numeric_variables = sas_tasks.SASNumericVariables(num_variables, num_axiom_layers, num_var_types)
    mutexes = [sas_tasks.SASMutexGroup(group) for group in mutex_key]
            
    for axiom in const_num_axioms:
#         print("Axiom = %s" % axiom)
#         print("Axiom.effect = %s" % axiom.effect)
#         print("corresponding variable = %s" % numeric_strips_to_sas.get(axiom.effect))
        var = numeric_strips_to_sas.get(axiom.effect)
        val = axiom.parts[0].value
        num_init_values[var]=val           
        
    sas_init = sas_tasks.SASInit(init_values, num_init_values)
#    print("SASInit is")
#    sas_init.dump()      
    
    # look up metric fluent 
    if metric[1] == -1:
        # minimize unit cost, no metric fluent specified
        assert metric[0] == '<' 
        sas_metric = metric
    else:
        assert metric[1] in numeric_strips_to_sas, "Metric fluent %s missing in strips_to_sas_dict" % metric[1]
        # look up (possibly derived) metric fluent to be optimized
        sas_metric = (metric[0], numeric_strips_to_sas[metric[1]])
        
#    print ("debug check metric: metric=")
#    print (metric)
#    print ("sas_metric fluent %d" % sas_metric[1])
#    print ("Returning task with global constraint = ",global_constraint_dict_list[0].items()[0])

    return sas_tasks.SASTask(variables, numeric_variables, mutexes, sas_init, sas_goal,
                             operators, axioms, comparison_axioms[1], sas_num_axioms,
							 global_constraint_dict_list[0].items()[0], sas_metric,
                             init_constant_predicates, init_constant_numerics)

def trivial_task(solvable):
    variables = sas_tasks.SASVariables(
        [2], [-1], [["Atom dummy(val1)", "Atom dummy(val2)"]], 0)
    # We create no mutexes: the only possible mutex is between
    # dummy(val1) and dummy(val2), but the preprocessor would filter
    # it out anyway since it is trivial (only involves one
    # finite-domain variable).
    num_variables = sas_tasks.SASNumericVariables(['total-cost'], [-1], ['I']) # no numeric variables
    mutexes = []
    init = sas_tasks.SASInit([0],[0])    
    if solvable:
        goal_fact = (0, 0)
    else:
        goal_fact = (0, 1)
    goal = sas_tasks.SASGoal([goal_fact])
    operators = []
    axioms = []
    comp_axioms = []
    numeric_axioms = []
    global_constraint = (0, 0)
    metric = ('<', 0)
    init_constant_predicates = [] 
    init_constant_numerics = []        
    return sas_tasks.SASTask(variables, num_variables, mutexes, init, goal,
                 operators, axioms, comp_axioms, numeric_axioms, global_constraint,
                 metric, init_constant_predicates, init_constant_numerics)


def solvable_sas_task(msg):
    print("%s! Generating solvable task..." % msg)
    return trivial_task(solvable=True)

def unsolvable_sas_task(msg):
    print("%s! Generating unsolvable task..." % msg)
    return trivial_task(solvable=False)

def pddl_to_sas(task):
    with timers.timing("Instantiating", block=True):
        (relaxed_reachable, atoms, num_fluents, actions, axioms, num_axioms,
            init_constant_predicates, init_constant_numerics,
            reachable_action_params) = instantiate.explore(task)
        
    if DEBUG: print("Task converted to SAS.")    
#    task.function_administrator.dump()    
    if DEBUG: print("Relaxed_reachable: %s" % relaxed_reachable)
    if DEBUG: print("List of %d Atoms:" % len(atoms))
#     for atom in atoms:
#         atom.dump()
    if DEBUG: print("List of %d numeric Fluents:" % len(num_fluents))
#     for fluent in num_fluents:
#         fluent.dump()
    if len(num_fluents) == 0:
        assert False
    if DEBUG: print("List of %d Actions:" % len(actions))    
#     for action in actions:
#         print(action.name)
    if DEBUG: print("List of %d propositional Axioms:" % len(axioms))
#    if len(axioms) > 0:
#        axioms[0].dump()
#        print("layer=%s"%axioms[0].__class__ )
#    for axiom in axioms:    
#        axiom.dump()
    if DEBUG: print("List of %d numeric Axioms:" % len(num_axioms))
#     for numax in num_axioms:
#         numax.dump()
    if DEBUG: print("List of %d constant predicates from initial state" % len(init_constant_predicates))
#     for icp in init_constant_predicates:
#         icp.dump()
    if DEBUG: print("List of %d constant numeric predicates from initial state"% len(init_constant_numerics))
#     for icn in init_constant_numerics:
#         icn.dump()        
    if DEBUG: print("List of %d reachable Action Parameters:"% len(reachable_action_params))
#     for rap in reachable_action_params:
#         rap.dump()
             
    if not relaxed_reachable:
        return unsolvable_sas_task("No relaxed solution")

    # HACK! Goals should be treated differently.
    if isinstance(task.goal, pddl.Conjunction):
        goal_list = task.goal.parts
    else:
        goal_list = [task.goal]
    for item in goal_list:
        assert isinstance(item, pddl.Literal)
    
    assert isinstance(task.global_constraint, pddl.Literal)
 #   if DEBUG: print("Global constraint Literal is ", task.global_constraint)

    with timers.timing("Computing fact groups", block=True):
        # groups: the ground facts are grouped in order to create multi valued variables        
        # mutex_groups: includes all mutex groups, so a fact can appear in multiple groups 
        # translation key: the names of the grounded facts of the MVV determined in groups, including "NegatedAtom" for binary and "none of those" for multi valued variables  
        groups, mutex_groups, translation_key = fact_groups.compute_groups(
            task, atoms, reachable_action_params)
#        print ("Fact groups (%d) computed %s" % (len(groups),[len(group) for group in groups]))        
#        print ("Full mutex groups (%d) containing %s:" % (len(mutex_groups),[len(group) for group in mutex_groups])) 

    with timers.timing("Handling numeric axioms"):
        num_axioms_by_layer, max_num_layer, num_axiom_map, const_num_axioms = \
            numeric_axiom_rules.handle_axioms(num_axioms)


    with timers.timing("Building STRIPS to SAS dictionary"):
        ranges, strips_to_sas, num_count, numeric_strips_to_sas = strips_to_sas_dictionary(
            groups, num_axioms, num_axiom_map, num_fluents, assert_partial=options.use_partial_encoding)
#     if DEBUG:
#         print("Strips to sas dictionary (%s entries)" % len(strips_to_sas))     
#         for entry in sorted(strips_to_sas.items(), key=lambda x:x[1]): # sort by value          
#             print("%s -> %s" % entry)
#         print(ranges)
#         print("Numeric Strips to sas dictionary (%s entries)" % len(numeric_strips_to_sas))
#         for entry in numeric_strips_to_sas:
#             print("%s -> %s" % (entry, numeric_strips_to_sas[entry]))        

#     print ("pddl2sas Zwischendebug: metric = ", task.metric)
#     assert task.metric[1] in numeric_strips_to_sas


    with timers.timing("Building dictionary for full mutex groups"):
        mutex_ranges, mutex_dict, _, _ = strips_to_sas_dictionary(
            mutex_groups, num_axioms, num_axiom_map, num_fluents, assert_partial=False, include_numeric=False)

    if options.add_implied_preconditions:
        with timers.timing("Building implied facts dictionary..."):
            implied_facts = build_implied_facts(strips_to_sas, groups,
                                                mutex_groups)
    else:
        implied_facts = {}

    with timers.timing("Building mutex information", block=True):
        mutex_key = build_mutex_key(strips_to_sas, mutex_groups)

    with timers.timing("Translating task", block=True):
        sas_task = translate_task(
            strips_to_sas, ranges, translation_key, numeric_strips_to_sas, num_count, 
            mutex_dict, mutex_ranges, mutex_key, task.init, task.num_init, goal_list,
			task.global_constraint, actions, axioms, num_axioms, num_axioms_by_layer,
			num_axiom_map, const_num_axioms, task.metric, implied_facts,
			init_constant_predicates, init_constant_numerics)        
#    print("len(variables.valuenames) = %s" % len(sas_task.variables.value_names))

    print("%d effect conditions simplified" %
          simplified_effect_condition_counter)
    print("%d implied preconditions added" %
          added_implied_precondition_counter)
    
#    print("created sas_task")
#    sas_task.dump()

    if options.filter_unreachable_facts:
        with timers.timing("Detecting unreachable propositions", block=True):
            try:
                simplify.filter_unreachable_propositions(sas_task)
            except simplify.Impossible:
                return unsolvable_sas_task("Simplified to trivially false goal")
            except simplify.TriviallySolvable:
                return solvable_sas_task("Simplified to empty goal")

#    print("translate pddl to sas returns task")        
    return sas_task


def build_mutex_key(strips_to_sas, groups):
    group_keys = []
    for group in groups:
        group_key = []
        for fact in group:
            if strips_to_sas.get(fact):
                for var, val in strips_to_sas[fact]:
                    group_key.append((var, val))
            else:
                print("not in strips_to_sas, left out:", fact)
        group_keys.append(group_key)
    return group_keys


def build_implied_facts(strips_to_sas, groups, mutex_groups):
    ## Compute a dictionary mapping facts (FDR pairs) to lists of FDR
    ## pairs implied by that fact. In other words, in all states
    ## containing p, all pairs in implied_facts[p] must also be true.
    ##
    ## There are two simple cases where a pair p implies a pair q != p
    ## in our FDR encodings:
    ## 1. p and q encode the same fact
    ## 2. p encodes a STRIPS proposition X, q encodes a STRIPS literal
    ##    "not Y", and X and Y are mutex.
    ##
    ## The first case cannot arise when we use partial encodings, and
    ## when we use full encodings, I don't think it would give us any
    ## additional information to exploit in the operator translation,
    ## so we only use the second case.
    ##
    ## Note that for a pair q to encode a fact "not Y", Y must form a
    ## fact group of size 1. We call such propositions Y "lonely".

    ## In the first step, we compute a dictionary mapping each lonely
    ## proposition to its variable number.
    lonely_propositions = {}
    for var_no, group in enumerate(groups):
        if len(group) == 1:
            lonely_prop = group[0]
            assert strips_to_sas[lonely_prop] == [(var_no, 0)]
            lonely_propositions[lonely_prop] = var_no

    ## Then we compute implied facts as follows: for each mutex group,
    ## check if prop is lonely (then and only then "not prop" has a
    ## representation as an FDR pair). In that case, all other facts
    ## in this mutex group imply "not prop".
    implied_facts = defaultdict(list)
    for mutex_group in mutex_groups:
        for prop in mutex_group:
            prop_var = lonely_propositions.get(prop)
            if prop_var is not None:
                prop_is_false = (prop_var, 1)
                for other_prop in mutex_group:
                    if other_prop is not prop:
                        for other_fact in strips_to_sas[other_prop]:
                            implied_facts[other_fact].append(prop_is_false)

    return implied_facts


def dump_statistics(sas_task):
    print("Translator variables: %d" % len(sas_task.variables.ranges))
    print(("Translator derived variables: %d" %
           len([layer for layer in sas_task.variables.axiom_layers
                if layer >= 0])))
    print("Translator facts: %d" % sum(sas_task.variables.ranges))
    print("Translator goal facts: %d" % len(sas_task.goal.pairs))
    print("Translator mutex groups: %d" % len(sas_task.mutexes))
    print(("Translator total mutex groups size: %d" %
           sum(mutex.get_encoding_size() for mutex in sas_task.mutexes)))
    print("Translator operators: %d" % len(sas_task.operators))
    print("Translator axioms: %d" % len(sas_task.axioms))
    print("Translator task size: %d" % sas_task.get_encoding_size())
    try:
        peak_memory = tools.get_peak_memory_in_kb()
    except Warning as warning:
        print(warning)
    else:
        print("Translator peak memory: %d KB" % peak_memory)


def main():
    timer = timers.Timer()
    with timers.timing("Parsing", True):
        task = pddl_parser.open(
            domain_filename=options.domain, task_filename=options.task)
#     print("translate.main(): task parsed\n Function symbols:")
#     for fs in task.FUNCTION_SYMBOLS:
#         print (fs)
#     print("task is:")
#     task.dump()    
    with timers.timing("Handling Global Constraints"):
        task.add_global_constraints()

    with timers.timing("Normalizing task"):
        normalize.normalize(task)
#     print("translate.main(): Function symbols after normalization:")
#     for fs in task.FUNCTION_SYMBOLS:
#         print (fs)
#     print("task is:")
#     task.dump()    
#     assert False

    if options.generate_relaxed_task:
        # Remove delete effects.
        for action in task.actions:
            for index, effect in reversed(list(enumerate(action.effects))):
                if effect.literal.negated:
                    del action.effects[index]

    sas_task = pddl_to_sas(task)
    dump_statistics(sas_task)
#    for operator in sas_task.operators:
#        print("Operatorname = %s" % operator.name)
#     print("Dumping axioms in task before writing output ")
#     for axiom in sas_task.axioms:
#         axiom.dump()
#         avar, aval = axiom.effect        
#         print("Effect state value init: %s axiomresult: %s" % (avar,aval))
#         print("Init = %s" % [sas_task.init.values[avar]])

    with timers.timing("Writing output"):
        with open(options.sas_file, "w") as output_file:
            sas_task.output(output_file)
    print("Done! %s" % timer)


if __name__ == "__main__":
    main()
