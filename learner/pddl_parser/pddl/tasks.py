from . import axioms
from . import predicates
from . import pddl_types
from . import predicates
from . import conditions
from . import actions
from . import functions
from typing import List


class Requirements:
    def __init__(self, requirements):
        self.requirements = requirements
        for req in requirements:
            assert req in (
              ":strips", ":adl", ":typing", ":negation", ":equality",
              ":negative-preconditions", ":disjunctive-preconditions",
              ":existential-preconditions", ":universal-preconditions",
              ":quantified-preconditions", ":conditional-effects",
              ":derived-predicates", ":action-costs"), req
    def __str__(self):
        return ", ".join(self.requirements)

class Task:
    def __init__(self,
                 domain_name: str,
                 task_name: str,
                 requirements: Requirements,
                 types: List[pddl_types.Type],
                 objects: List[pddl_types.TypedObject],
                 preds: List[predicates.Predicate],
                 funcs: List[functions.Function],
                 init: List[conditions.Literal],
                 goal: conditions.Conjunction,
                 acts: List[actions.Action],
                 axs: List[axioms.Axiom],
                 use_metric: bool):
        self.domain_name = domain_name
        self.task_name = task_name
        self.name = task_name
        self.requirements = requirements
        self.types = types
        self.objects = objects
        self.predicates = preds
        self.functions = funcs
        self.init = init
        self.initial_state = init
        self.goal = goal
        self.goals = set(self.goal.parts)
        self.goal_set = set(self.goal.parts)
        self.actions = acts
        self.axioms = axs
        self.axiom_counter = 0
        self.use_min_cost_metric = use_metric

    def add_axiom(self, parameters, condition):
        name = "new-axiom@%d" % self.axiom_counter
        self.axiom_counter += 1
        axiom = axioms.Axiom(name, parameters, len(parameters), condition)
        self.predicates.append(predicates.Predicate(name, parameters))
        self.axioms.append(axiom)
        return axiom
    
    def is_goal_state(self, state):
        return self.goal_set.issubset(state) 

    def dump(self):
        print("Problem %s: %s [%s]" % (
            self.domain_name, self.task_name, self.requirements))
        print("Types:")
        for typ in self.types:
            print("  %s" % typ)
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
        print("Goal:")
        self.goal.dump()
        print("Actions:")
        for action in self.actions:
            action.dump()
        if self.axioms:
            print("Axioms:")
            for axiom in self.axioms:
                axiom.dump()
