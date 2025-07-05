import logging
from typing import Union

from _succgen.planning import AtomPacker, FluentIndexMap
from pddl.core import Domain, Problem
from pddl.logic.base import And, Not
from pddl.logic.functions import Assign, Decrease, EqualTo, Increase, NumericFunction, NumericValue
from pddl.logic.predicates import Predicate
from succgen.planning import (
    get_func_index_mappings,
    get_literals_list,
    get_obj_index_mappings,
    get_pred_index_mappings,
    get_schema_index_mappings,
)
from succgen.planning.action import SGAction
from succgen.planning.goal import SGGoal
from succgen.planning.ground_expressions import to_ground_expr
from succgen.planning.state import SGState, pddl_terms_to_row
from succgen.planning.strings import to_value


class SGTask:
    def __init__(self, domain: Domain, problem: Problem):
        self.domain = domain
        self.problem = problem

        """ First-Order Indexes"""
        self.pred_to_i, self.i_to_pred = get_pred_index_mappings(domain)
        self.func_to_i, self.i_to_func = get_func_index_mappings(domain)
        self.obj_to_i, self.i_to_obj = get_obj_index_mappings(domain, problem)
        self.schema_to_i, self.i_to_schema = get_schema_index_mappings(domain)

        """ Statics """
        self.static_predicates: list[int]
        self.static_functions: list[int]
        self.fluent_predicates: list[int]
        self.fluent_functions: list[int]
        self.statics: dict[Union[Predicate, NumericFunction], Union[bool, float]] = {}
        self._handle_statics()

        """ Numeric Indices"""

        self.num_fluent_to_i = {}
        self.i_to_num_fluent = []
        for f in sorted(self.problem.init, key=lambda f: str(f)):
            if not isinstance(f, EqualTo):
                continue
            function = f.operands[0]
            value = f.operands[1]
            assert isinstance(function, NumericFunction)
            assert isinstance(value, NumericValue)
            if self.func_to_i[function.name] in self.static_functions:
                continue
            i = len(self.num_fluent_to_i)
            self.num_fluent_to_i[function] = i
            self.i_to_num_fluent.append(function)
        self.fluent_index_map_pyt: dict[tuple[int, tuple[int]], int] = {}
        for fluent, i in self.num_fluent_to_i.items():
            func = self.func_to_i[fluent.name]
            objs = tuple(self.obj_to_i[o.name] for o in fluent.terms)
            self.fluent_index_map_pyt[(func, objs)] = i
        self.fluent_index_map_cpp = FluentIndexMap(
            [((f, list(o)), i) for (f, o), i in self.fluent_index_map_pyt.items()]
        )

        """ Atom packer """
        self.atom_packer = AtomPacker()

        """ Goal"""
        # TODO check static goals
        true_facts = set()
        false_facts = set()
        numeric_goals = []
        for goal in get_literals_list(self.problem.goal):
            if isinstance(goal, Predicate):
                pred = self.pred_to_i[goal.name]

                if pred in self.static_predicates:
                    if goal not in self.problem.init:
                        raise ValueError(
                            f"Problem unsolvable. Static goal {goal} not in initial state."
                        )
                    else:
                        continue

                instantiation = pddl_terms_to_row(self.obj_to_i, goal.terms)
                i = self.atom_packer.pack(pred, instantiation)
                true_facts.add(i)
            elif isinstance(goal, Not) and isinstance(goal.argument, Predicate):
                goal = goal.argument
                pred = self.pred_to_i[goal.name]

                if pred in self.static_predicates:
                    if goal in self.problem.init:
                        raise ValueError(
                            f"Problem unsolvable. Static negative goal {goal} in initial state."
                        )
                    else:
                        continue

                instantiation = pddl_terms_to_row(self.obj_to_i, goal.terms)
                i = self.atom_packer.pack(pred, instantiation)
                false_facts.add(i)
            else:
                numeric_goals.append(
                    to_ground_expr(
                        goal,
                        self.statics,
                        self.fluent_index_map_pyt,
                        self.func_to_i,
                        self.obj_to_i,
                    )
                )
        self.goal = SGGoal(
            pos_goals=true_facts, neg_goals=false_facts, numeric_goals=numeric_goals
        )

    def _handle_statics(self) -> None:
        all_predicates = set(p.name for p in self.domain.predicates)
        all_functions = set(f.name for f in self.domain.functions)
        static_predicates = all_predicates.copy()
        static_functions = all_functions.copy()

        # There may be a safer way to do this than using strings
        for action in self.domain.actions:
            effects = action.effect
            effects = effects.operands if isinstance(effects, And) else [effects]
            for effect in effects:
                if isinstance(effect, Predicate):
                    nonstatic = effect.name
                elif isinstance(effect, Not) and isinstance(effect.argument, Predicate):
                    nonstatic = effect.argument.name
                elif isinstance(effect, (Decrease, Increase, Assign)):
                    function = effect.operands[0]
                    assert isinstance(function, NumericFunction)
                    nonstatic = function.name
                else:
                    raise ValueError(f"Unknown effect {effect} of type {type(effect)}")
                static_predicates.discard(nonstatic)
                static_functions.discard(nonstatic)

        self.static_predicates = [self.pred_to_i[p] for p in sorted(static_predicates)]
        self.static_functions = [self.func_to_i[f] for f in sorted(static_functions)]
        self.fluent_predicates = [
            self.pred_to_i[p] for p in sorted(all_predicates - static_predicates)
        ]
        self.fluent_functions = [
            self.func_to_i[f] for f in sorted(all_functions - static_functions)
        ]

        logging.debug(f"{self.static_predicates=}")
        logging.debug(f"{self.static_functions=}")
        logging.debug(f"{self.fluent_predicates=}")
        logging.debug(f"{self.fluent_functions=}")

        statics = []
        static_pred_is = set(self.static_predicates)
        static_func_is = set(self.static_functions)
        for fact in self.problem.init:
            if isinstance(fact, Predicate):
                if self.pred_to_i[fact.name] not in static_pred_is:
                    continue
                self.statics[fact] = True
                statics.append(fact)
            elif isinstance(fact, EqualTo):
                function = fact.operands[0]
                value = fact.operands[1]
                assert isinstance(function, NumericFunction), function
                assert isinstance(value, NumericValue), value
                if self.func_to_i[function.name] not in static_func_is:
                    continue
                self.statics[function] = to_value(value)
                statics.append(fact)
            else:
                raise ValueError(f"Unknown fact {fact} of type {type(fact)}")
        # self.statics: PDDLState = frozenset(sorted(statics, key=lambda x: str(x)))

    def get_init_state(self) -> SGState:
        state = self.problem.init

        atoms = set()
        values = [None] * len(self.num_fluent_to_i)
        for fact in sorted(state, key=lambda f: str(f)):
            if isinstance(fact, Predicate):
                if fact in self.statics:
                    continue
                pred = self.pred_to_i[fact.name]
                instantiation = pddl_terms_to_row(self.obj_to_i, fact.terms)
                i = self.atom_packer.pack(pred, instantiation)
                atoms.add(i)
            elif isinstance(fact, EqualTo):
                function = fact.operands[0]
                if function in self.statics:
                    continue
                value = fact.operands[1]
                assert isinstance(function, NumericFunction)
                assert isinstance(value, NumericValue)
                i = self.num_fluent_to_i[function]
                values[i] = to_value(value)
            else:
                raise ValueError(f"Unknown fact {fact} of type {type(fact)}")
        return SGState(atoms=atoms, values=values)

    def dump_state(self, state: SGState) -> None:
        print(self.state_to_string(state))

    def state_to_string(self, state: SGState, delimiter: str = "\n") -> str:
        atoms = []
        values = []

        state_atoms = state.atoms
        state_values = state.values

        for fact in state_atoms:
            fact = self.atom_packer.unpack(fact)
            table_name = self.i_to_pred[fact[0]]
            row = [self.i_to_obj[i] for i in fact[1]]
            atoms.append(f"{table_name}({','.join(row)})")
        for func, i in self.fluent_index_map_pyt.items():
            table_name = self.i_to_func[func[0]]
            row = [self.i_to_obj[i] for i in func[1]]
            values.append(f"{table_name}({','.join(row)}) = {state_values[i]}")

        return delimiter.join(sorted(atoms) + sorted(values))

    def dump_action(self, action: SGAction) -> None:
        print(self.action_to_string(action))

    def action_to_string(self, action: SGAction) -> str:
        schema_name = self.i_to_schema[action[0]]
        obj_names = [self.i_to_obj[i] for i in action[1]]

        return "(" + " ".join([schema_name] + obj_names) + ")"

    def action_to_readable(self, action: SGAction) -> tuple[str, list[str]]:
        schema_name = self.i_to_schema[action[0]]
        obj_names = tuple(self.i_to_obj[i] for i in action[1])
        return (schema_name, obj_names)
