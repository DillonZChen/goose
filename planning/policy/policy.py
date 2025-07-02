import logging
import sys
import time
from abc import ABC, abstractmethod
from typing import Optional

import pddl
from pddl.logic.base import And, Not
from pddl.logic.functions import Assign, Decrease, FunctionExpression, Increase, NumericFunction
from pddl.logic.predicates import Predicate
from succgen.planning import Plan
from succgen.planning.action import SGAction
from succgen.planning.effects import AssignEffect, DecreaseEffect, Effects, IncreaseEffect
from succgen.planning.lifted_expressions import to_lifted_expr
from succgen.planning.state import SGState
from succgen.planning.task import SGTask
from succgen.sgutil.logging import mat_to_str
from succgen.sgutil.managers import TimerContextManager
from succgen.sqlite_applicable_action_generator import SQLiteApplicableActionGenerator


class PolicyExecutor(ABC):
    def __init__(self, domain_file: str, problem_file: str, debug: bool = False, bound: int = -1):

        self._debug = debug
        self._bound = bound

        self._t_total = 0
        self._t_eval_p = 0
        self._t_aag = 0
        self._t_sg = 0

        self._start_time = time.perf_counter()

        with TimerContextManager("parsing PDDL inputs") as timer:
            domain = pddl.parse_domain(domain_file)
            problem = pddl.parse_problem(problem_file)
        self._t_parsing = timer.get_time()

        # TODO check if can support PDDL fragment

        self._task = SGTask(domain, problem)
        self._aag = SQLiteApplicableActionGenerator(self._task, debug=debug)
        self._schema_to_effects: list[Effects] = []
        self._init_schemata()

    def _init_schemata(self) -> None:
        for schema in sorted(self._task.domain.actions, key=lambda s: s.name):
            adds = []
            dels = []
            numeric_effects = []
            param_to_index = {p.name: i for i, p in enumerate(schema.parameters)}

            def get_indices(terms):
                return [param_to_index[t.name] for t in terms]

            effects = schema.effect
            effects = list(effects.operands) if isinstance(effects, And) else [effects]
            for effect in effects:
                if isinstance(effect, (Predicate, Not)):
                    is_add = isinstance(effect, Predicate)
                    if not is_add:
                        effect = effect.argument
                    table = self._task.pred_to_i[effect.name]
                    row = get_indices(effect.terms)
                    effect_cpp = (table, row)
                    if is_add:
                        adds.append(effect_cpp)
                    else:
                        dels.append(effect_cpp)
                elif isinstance(effect, (Increase, Decrease, Assign)):
                    function = effect.operands[0]
                    expr = effect.operands[1]
                    assert isinstance(function, NumericFunction), effect
                    assert isinstance(expr, FunctionExpression), effect

                    if isinstance(effect, Increase):
                        Cls = IncreaseEffect
                    elif isinstance(effect, Decrease):
                        Cls = DecreaseEffect
                    elif isinstance(effect, Assign):
                        Cls = AssignEffect
                    else:
                        raise ValueError(f"Unknown effect {effect} of type {type(effect)}")

                    table = self._task.func_to_i[function.name]
                    row = get_indices(function.terms)
                    lifted_expression = to_lifted_expr(
                        expr,
                        param_to_index,
                        self._task,
                    )
                    numeric_effects.append(Cls(table=table, row=row, expression=lifted_expression))
                else:
                    raise NotImplementedError(f"{effect} of type {type(effect)} is not supported yet")

            self._schema_to_effects.append(Effects(adds=adds, dels=dels, numeric_effects=numeric_effects))

    def execute(self) -> Optional[Plan]:
        plan = []
        state = self._task.get_init_state()
        logging.info("Starting policy execution...")
        while True:
            if self._is_goal(state):
                logging.info("Solution found!")
                break
            if self._bound > 0 and len(plan) >= self._bound:
                logging.info(f"{self._bound=} reached. Terminating policy execution.")
                plan = None
                break
            applicable_actions = self._get_applicable_actions(state)
            if len(applicable_actions) == 0:
                logging.info("Deadend reached. Terminating policy execution.")
                plan = None
                break

            t = time.perf_counter()
            action = self.select_action(state, applicable_actions)
            self._t_eval_p += time.perf_counter() - t

            state = self._get_successor_state(action, state)

            plan.append(self._task.action_to_string(action))

            n_steps = len(plan)
            if n_steps > 0 and (n_steps & (n_steps - 1)) == 0:
                logging.info(f"Reached {n_steps=}")

        self._t_total = time.perf_counter() - self._start_time
        self._plan_length = len(plan) if plan is not None else "na"

    @abstractmethod
    def select_action(self, state: SGState, actions: list[SGAction]) -> SGAction:
        raise NotImplementedError

    def _is_goal(self, state: SGState) -> bool:
        return self._task.goal.satisfied_by(state)

    def _get_applicable_actions(self, state: SGState) -> list[SGAction]:
        return self._aag.get_applicable_actions(state)

    def _get_successor_state(self, action: SGAction, state: SGState) -> SGState:
        t = time.perf_counter()
        succ_state = state.apply_action(
            action=self._schema_to_effects[action[0]],
            instantiation=action[1],
            atom_packer=self._task.atom_packer,
            nvars_map=self._task.fluent_index_map_cpp,
        )
        self._t_sg += time.perf_counter() - t
        return succ_state

    def _debug_plan(self, plan: Plan) -> None:
        plan_i = 0
        state = self._task.get_init_state()
        while True:
            applicable_actions = self._get_applicable_actions(state)
            applicable_actions = {self._task.action_to_string(a): a for a in applicable_actions}
            action = plan[plan_i]
            logging.info(f"{plan_i}")
            logging.info(f"{state=}")
            logging.info(f"{action=}")
            assert action in applicable_actions, f"Expected {action} in applicable actions"
            action = applicable_actions[action]
            state = self._get_successor_state(action, state)
            plan_i += 1
            if plan_i == len(plan):
                assert self._is_goal(state), f"Expected goal state, got {state}"
                logging.info("Plan found")
                sys.exit()

    def dump_stats(self) -> None:

        def float_format(x: float) -> str:
            return f"{x:.4f}"

        def percentage(x: float, y: float) -> str:
            return f"{100 * x / y:.1f}%"

        dividers = ("*", "*", "*")

        t_aag_total = self._aag.t_aag_processing + self._aag.t_aag_execution
        t_rem_search = self._t_total - (self._t_eval_p + t_aag_total + self._t_sg)

        stats = [
            dividers,
            ("plan length", self._plan_length),
            dividers,
            ("time parsing", float_format(self._t_parsing)),
            dividers,
            ("time policy evaluations", float_format(self._t_eval_p), percentage(self._t_eval_p, self._t_total)),
            # ("time applicable action generation", float_format(self._t_aag), percentage(self._t_aag, self._t_search)),
            ("time action generation", float_format(t_aag_total), percentage(t_aag_total, self._t_total)),
            ("time successor generation", float_format(self._t_sg), percentage(self._t_sg, self._t_total)),
            ("time other planning routines", float_format(t_rem_search), percentage(t_rem_search, self._t_total)),
            ("total planning time", float_format(self._t_total), percentage(self._t_total, self._t_total)),
            dividers,
        ]

        logging.info(f"Planner statistics:\n" + mat_to_str(stats, rjust=[False, True, True]))
        if self._debug:
            self._aag.dump_profiling()
