#! /usr/bin/env python

from __future__ import print_function

import itertools

import normalize
import pddl
import timers

class PrologProgram:
    def __init__(self):        
        self.facts = []
        self.rules = []
        self.objects = set()
        def predicate_name_generator():
            for count in itertools.count():
                yield "p$%d" % count
        self.new_name = predicate_name_generator()
    def add_fact(self, atom):
        self.facts.append(Fact(atom))
        self.objects |= set(atom.args)
    def add_rule(self, rule):
        self.rules.append(rule)
    def dump(self, file=None):        
        print("Facts in PrologProgram:")
        for fact in self.facts:
            print(fact, file=file)
        print("Rules in PrologProgram:")
        for rule in self.rules:
            print(getattr(rule, "type", "none"), rule, file=file)
    def normalize(self):
        # Normalized prolog programs have the following properties:
        # 1. Each variable that occurs in the effect of a rule also occurs in its
        #    condition.
        # 2. The variables that appear in each effect or condition are distinct.
        # 3. There are no rules with empty condition.
        self.remove_free_effect_variables()
        self.split_duplicate_arguments()
        self.convert_trivial_rules()
    def split_rules(self):
        import split_rules
        # Splits rules whose conditions can be partitioned in such a way that
        # the parts have disjoint variable sets, then split n-ary joins into
        # a number of binary joins, introducing new pseudo-predicates for the
        # intermediate values.
        new_rules = []
        for rule in self.rules:
            new_rules += split_rules.split_rule(rule, self.new_name)
        self.rules = new_rules
    def remove_free_effect_variables(self):
        """Remove free effect variables like the variable Y in the rule
        p(X, Y) :- q(X). This is done by introducing a new predicate
        @object, setting it true for all objects, and translating the above
        rule to p(X, Y) :- q(X), @object(Y).
        After calling this, no new objects should be introduced!"""

        # Note: This should never be necessary for typed domains.
        # Leaving it in at the moment regardless.
        must_add_predicate = False
        for rule in self.rules:                        
#            print("rule: %s" % rule)
            eff_vars = get_variables([rule.effect])
#            print("this rule has the following conditions: %s" % rule.conditions)            
            cond_vars = get_variables(rule.conditions)            
#            print("condition vars %s // effect vars %s" % (cond_vars, eff_vars))
            if not eff_vars.issubset(cond_vars):
                must_add_predicate = True
                eff_vars -= cond_vars
                for var in sorted(eff_vars):
                    rule.add_condition(pddl.Atom("@object", [var]))
        if must_add_predicate:
            print("Unbound effect variables: Adding @object predicate.")
            self.facts += [Fact(pddl.Atom("@object", [obj])) for obj in self.objects]
    def split_duplicate_arguments(self):
        """Make sure that no variable occurs twice within the same symbolic fact,
        like the variable X does in p(X, Y, X). This is done by renaming the second
        and following occurrences of the variable and adding equality conditions.
        For example p(X, Y, X) is translated to p(X, Y, X@0) with the additional
        condition =(X, X@0); the equality predicate must be appropriately instantiated
        somewhere else."""
        printed_message = False
        for rule in self.rules:
            if rule.rename_duplicate_variables() and not printed_message:                
                print("Duplicate arguments: Adding equality conditions.") 
                printed_message = True

    def convert_trivial_rules(self):
        """Convert rules with an empty condition into facts.
        This must be called after bounding rule effects, so that rules with an
        empty condition must necessarily have a variable-free effect.
        Variable-free effects are the only ones for which a distinction between
        ground and symbolic atoms is not necessary."""
        must_delete_rules = []
        for i, rule in enumerate(self.rules):
            if not rule.conditions:
                assert not get_variables([rule.effect])
                self.add_fact(pddl.Atom(rule.effect.predicate, rule.effect.args))
                must_delete_rules.append(i)
        if must_delete_rules:
            print("Trivial rules: Converted to facts.")
            for rule_no in must_delete_rules[::-1]:
                del self.rules[rule_no]

def get_variables(symbolic_atoms):
    variables = set()
    for sym_atom in symbolic_atoms:
#        print("Trying to get variables. Symbolic Atom = %s" % sym_atom)
#        print("my arguments are %s" % [sym_atom.args])
        variables |= set([arg for arg in sym_atom.args if arg[0] == "?"])
    return variables

class Fact:
    def __init__(self, atom):
        self.atom = atom
    def __str__(self):
        return "%s." % self.atom

class Rule:
    def __init__(self, conditions, effect):
        self.conditions = conditions
        self.effect = effect
    def add_condition(self, condition):
        self.conditions.append(condition)
    def get_variables(self):
        return get_variables(self.conditions + [self.effect])
    def _rename_duplicate_variables(self, atom, new_conditions):
        used_variables = set()
        for i, var_name in enumerate(atom.args):
            if var_name[0] == "?":
                if var_name in used_variables:
                    new_var_name = "%s@%d" % (var_name, len(new_conditions))
                    atom = atom.replace_argument(i, new_var_name)
                    new_conditions.append(pddl.Atom("=", [var_name, new_var_name]))
                else:
                    used_variables.add(var_name)
        return atom
    def rename_duplicate_variables(self):
        extra_conditions = []
        self.effect = self._rename_duplicate_variables(
            self.effect, extra_conditions)
        old_conditions = self.conditions
        self.conditions = []
        for condition in old_conditions:
            self.conditions.append(self._rename_duplicate_variables(
                    condition, extra_conditions))
        self.conditions += extra_conditions
        return bool(extra_conditions)
    def __str__(self):
        cond_str = ", ".join(map(str, self.conditions))
        return "%s :- %s." % (self.effect, cond_str)

def translate_typed_object(prog, obj, type_dict):
    supertypes = type_dict[obj.type_name].supertype_names
    for type_name in [obj.type_name] + supertypes:
        prog.add_fact(pddl.TypedObject(obj.name, type_name).get_atom())

def translate_facts(prog, task):
    type_dict = dict((type.name, type) for type in task.types)
    for obj in task.objects:
        translate_typed_object(prog, obj, type_dict)
    for fact in task.init:
        assert isinstance(fact,pddl.Atom)
        prog.add_fact(fact)
    for fact in task.num_init:
#        print("pddl_to_prolog translating fact ")
#        fact.dump()
        assert isinstance(fact,pddl.FunctionAssignment)
        atom = normalize.get_function_predicate(fact.fluent)
        prog.add_fact(atom)

def translate(task):
    # Note: The function requires that the task has been normalized.
    with timers.timing("Generating Datalog program"):
        prog = PrologProgram()
        translate_facts(prog, task)
        for conditions, effect in normalize.build_exploration_rules(task):            
#            rule = Rule(conditions, effect)
#            print("\nadding rule %s"% rule)        
#            prog.add_rule(rule)
            prog.add_rule(Rule(conditions, effect))

#    print("############################## dumping translated logic program ##############")
#    prog.dump()
#    print("############################## done dumping translated logic program ##############")
        
    with timers.timing("Normalizing Datalog program", block=True):
        # Using block=True because normalization can output some messages
        # in rare cases.
        prog.normalize()
        prog.split_rules()
#    print("############################## dumping normalized logic program ##############")
#    prog.dump()
#    print("############################## done dumping normalized logic program ##############")                
    return prog


if __name__ == "__main__":
    # test_normalization()
    import pddl_parser
    task = pddl_parser.open()
    normalize.normalize(task)
    prog = translate(task)
    prog.dump()
