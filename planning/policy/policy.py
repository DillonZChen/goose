import argparse
import logging
import random
import sys
import time
from abc import ABC, abstractmethod
from typing import Optional

import pddl
import termcolor as tc
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

from enums.policy_type import PolicyType
from wlplan.graph_generator import Graph, init_graph_generator
from wlplan.planning import Action, Atom, State, to_wlplan_domain, to_wlplan_problem


class PolicyExecutor(ABC):
    def __init__(
        self,
        domain_path: str,
        problem_path: str,
        train_opts: argparse.Namespace,
        debug: bool = False,
        bound: int = -1,
    ):
        if debug:
            logging.getLogger().setLevel(logging.DEBUG)

        self._debug = debug
        self._bound = bound

        self._t_total = 0
        self._t_eval_p = 0
        self._t_aag = 0
        self._t_sg = 0

        self._start_time = time.perf_counter()

        with TimerContextManager("parsing PDDL inputs") as timer:
            domain = pddl.parse_domain(domain_path)
            problem = pddl.parse_problem(problem_path)
        self._t_parsing = timer.get_time()

        # TODO check if can support PDDL fragment

        # Planning components
        self._task = SGTask(domain, problem)
        self._aag = SQLiteApplicableActionGenerator(self._task, debug=debug)
        self._schema_to_effects: list[Effects] = []
        self._init_schemata()

        # Policy type
        self._policy_type = PolicyType.parse(train_opts.policy_type)
        if self._policy_type in {PolicyType.VALUE_FUNCTION}:
            self._predict_impl = self._select_v
        elif self._policy_type in {
            PolicyType.QUALITY_FUNCTION,
            PolicyType.ADVANTAGE_FUNCTION,
            PolicyType.POLICY_FUNCTION,
        }:
            self._predict_impl = self._select_q
        else:
            raise ValueError(f"Unknown value {self._policy_type=}")

        # WLPlan components
        self._domain = to_wlplan_domain(self._task.domain)
        self._problem = to_wlplan_problem(self._task.domain, self._task.problem)

        self._name_to_predicate = {p.name: p for p in self._domain.predicates}
        self._name_to_schema = {s.name: s for s in self._domain.schemata}

        self._graph_generator = init_graph_generator(
            graph_representation=train_opts.graph_representation,
            domain=self._domain,
            differentiate_constant_objects=True,
        )
        self._graph_generator.set_problem(self._problem)

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
        cycle_detector = {}
        logging.info("Starting policy execution...")
        while True:
            if self._is_goal(state):
                logging.info("Solution found!")
                break
            if self._bound > 0 and len(plan) >= self._bound:
                logging.info(f"{self._bound=} reached. Terminating policy execution.")
                plan = None
                break

            state_str = self._task.state_to_string(state, delimiter=", ")
            if state_str not in cycle_detector:
                cycle_detector[state_str] = set()

            debug_str = ""
            debug_str += f"step {len(plan)}\n"
            debug_str += f"\n{{{{{state_str}}}}}\n\n"

            applicable_actions = self._get_applicable_actions(state)
            # applicable_actions = []
            # for a in self._get_applicable_actions(state):
            #     a_str = self._task.action_to_string(a)
            #     if a_str not in cycle_detector[state_str]:
            #         applicable_actions.append(a)

            if len(applicable_actions) == 0:
                logging.info("Deadend reached. Terminating policy execution.")
                plan = None
                break

            t = time.perf_counter()
            scores = self.compute_scores(state, applicable_actions)

            best_pred = min(scores.values())
            best_action = random.choice([a for a, v in scores.items() if v == best_pred])
            best_action_str = self._task.action_to_string(best_action)
            for action, score in scores.items():
                action_str = self._task.action_to_string(action)
                if best_action_str == action_str:
                    action_str = tc.colored(action_str, "green")
                debug_str += f"{action_str}  {score:.4f}\n"
            self._t_eval_p += time.perf_counter() - t

            cycle_detector[state_str].add(best_action_str)

            state = self._get_successor_state(best_action, state)
            plan.append(best_action_str)

            n_steps = len(plan)
            logging.debug(debug_str)
            if n_steps > 0 and (n_steps & (n_steps - 1)) == 0:
                logging.info(f"Reached {n_steps=}")

            # if self._debug:
            #     breakpoint()

        self._t_total = time.perf_counter() - self._start_time
        self._plan_length = len(plan) if plan is not None else "na"

    @abstractmethod
    def _predict_graph(self, graph: Graph) -> float:
        raise NotImplementedError

    def _select_v(self, state: SGState, action: SGAction) -> float:  # does not use action
        wl_state = self._sgstate_to_wlstate(state)
        graph = self._graph_generator.to_graph(state=wl_state)
        pred = self._predict_graph(graph)
        return pred

    def _select_q(self, state: SGState, action: SGAction) -> float:
        wl_state = self._sgstate_to_wlstate(state)
        wl_action = self._sgaction_to_wlaction(action)
        graph = self._graph_generator.to_graph(state=wl_state, actions=[wl_action])
        pred = self._predict_graph(graph)
        return pred

    def compute_scores(self, state: SGState, actions: list[SGAction]) -> dict[SGAction, float]:
        return {a: self._predict_impl(state=self._get_successor_state(a, state), action=a) for a in actions}

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

    def _sgstate_to_wlstate(self, state: SGState) -> State:
        wl_state = []
        for atom in state.atoms:
            atom = self._task.atom_packer.unpack(atom)
            predicate = self._name_to_predicate[self._task.i_to_pred[atom[0]]]
            objects = [self._task.i_to_obj[i] for i in atom[1]]
            atom = Atom(predicate=predicate, objects=objects)
            wl_state.append(atom)
        wl_state = State(atoms=wl_state)
        return wl_state

    def _sgaction_to_wlaction(self, action: SGAction) -> Action:
        schema = self._name_to_schema[self._task.i_to_schema[action[0]]]
        objects = [self._task.i_to_obj[i] for i in action[1]]
        wl_action = Action(schema=schema, objects=objects)
        return wl_action

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
