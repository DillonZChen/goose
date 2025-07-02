import logging
import queue
import time
from enum import Enum
from typing import Any, Optional

import pddl
from _succgen.planning import AssignEffect, DecreaseEffect, Effects, IncreaseEffect
from pddl.logic.base import And, Not
from pddl.logic.functions import (
    Assign,
    Decrease,
    EqualTo,
    FunctionExpression,
    Increase,
    NumericFunction,
    NumericValue,
)
from pddl.logic.predicates import Predicate
from succgen.guidance_factory import get_heuristic, get_policy
from succgen.planning import Plan
from succgen.planning.effects import Effects
from succgen.planning.lifted_expressions import to_lifted_expr
from succgen.planning.node import SearchNode
from succgen.planning.state import State
from succgen.planning.task import Task
from succgen.planning.visited_storage import VisitedStorage
from succgen.priority_queue import PriorityQueue
from succgen.sqlite_applicable_action_generator import SGAction, SQLiteApplicableActionGenerator
from succgen.util.logging import mat_to_str
from succgen.util.managers import TimerContextManager


class Mode(Enum):
    HEURISTIC = "heuristic"
    POLICY = "policy"
    BOTH = "both"


class LiftedNumericPlanner:
    def __init__(
        self,
        domain_file: str,
        problem_file: str,
        heuristic_name: Optional[str],
        policy_name: Optional[str],
        debug: bool = False,
        bound: int = -1,
    ):
        if heuristic_name is None and policy_name is None:
            raise ValueError("A heuristic or policy must be specified with --heuristic or --policy")
        if heuristic_name is not None and policy_name is not None:
            self._mode = Mode.BOTH
        elif heuristic_name is not None:
            self._mode = Mode.HEURISTIC
        else:
            self._mode = Mode.POLICY

        self._debug = debug
        self._bound = bound

        # Statistics
        self._n_eval_h = 0
        self._n_eval_p = 0
        self._n_exp = 0
        self._n_dead = 0
        self._t_eval_h = 0
        self._t_eval_p = 0
        self._t_aag = 0
        self._t_sg = 0

        self._best_h = float("inf")
        self._start_time = time.perf_counter()

        with TimerContextManager("parsing PDDL inputs") as timer:
            domain = pddl.parse_domain(domain_file)
            problem = pddl.parse_problem(problem_file)
        self._t_parsing = timer.get_time()

        # TODO check if can support PDDL fragment

        self._task = Task(domain, problem)
        self._heuristic = get_heuristic(self._task, heuristic_name)
        self._policy = get_policy(self._task, policy_name)

        # Planning modules
        self._vis = VisitedStorage()
        self._aag = SQLiteApplicableActionGenerator(self._task, debug=debug)
        self._s_id_counter = 0

        # PDDL information
        self._schema_to_effects: list[Effects] = []
        self._initialise_schema()

    def _initialise_schema(self) -> None:
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

    def _get_new_counter(self) -> int:
        ret = self._s_id_counter
        self._s_id_counter += 1
        return ret

    def _evaluate_heuristic(self, state: State) -> float:
        if self._heuristic is None:
            return None

        t = time.perf_counter()
        h = self._heuristic.evaluate(state)
        self._t_eval_h += time.perf_counter() - t
        self._n_eval_h += 1

        if h < self._best_h:
            self._best_h = h
            evaluations = self._n_eval_h
            expansions = self._n_exp
            logging.info(f"New best {h=}, {evaluations=}, {expansions=}")

        return h

    def _evaluate_policy(self, state: State, actions: list[SGAction]) -> SGAction:
        if self._policy is None:
            return None

        t = time.perf_counter()
        action = self._policy.generate(state, actions)
        self._t_eval_p += time.perf_counter() - t
        self._n_eval_p += 1

        # if self._heuristic is None:  # log each action if purely policy mode
        #     action_str = self._task.action_to_string(action)
        #     logging.info(f"Step {self._n_eval_p}: {action_str}")

        return action

    def _is_goal(self, state: State) -> bool:
        ret = self._task.goal.satisfied_by(state)
        if ret:
            logging.info(f"Solution found!")
        return ret

    def _get_successor_state(self, action: SGAction, state: State) -> State:
        t = time.perf_counter()
        succ_state = state.apply_action(
            action=self._schema_to_effects[action[0]],
            instantiation=action[1],
            atom_packer=self._task.atom_packer,
            nvars_map=self._task.fluent_index_map_cpp,
        )
        self._t_sg += time.perf_counter() - t
        return succ_state

    def _get_applicable_actions(self, node: SearchNode) -> list[SGAction]:
        applicable_actions = self._aag.get_applicable_actions(node.state)
        self._n_exp += 1
        return applicable_actions

    def _extract_plan(self, node: Optional[SearchNode]) -> Plan:
        plan = []
        while True:
            if node is None or node.parent_s_id == -1:
                break
            plan.append(node.achieving_action)
            node = self._vis.get(node.parent_s_id)

        t = time.perf_counter()
        self._t_search = t - self._search_start_time
        self._t_plan = t - self._start_time

        self._plan_length = len(plan)
        plan = list(reversed(plan))
        plan = [self._task.action_to_string(action) for action in plan]

        return plan

    def _get_init_node(self, init_state: State) -> SearchNode:
        return SearchNode(
            state=init_state, achieving_action=(0, ()), s_id=self._get_new_counter(), parent_s_id=-1, g=0
        )

    def execute_plan(self, plan: Plan) -> None:
        plan_i = 0
        init_state = self._task.get_init_state()
        node = self._get_init_node(init_state)
        while True:
            applicable_actions = self._get_applicable_actions(node)
            actions_str = {self._task.action_to_string(a) for a in applicable_actions}
            # for a in actions_str:
            #     print(a)
            assert plan[plan_i] in actions_str, f"Expected {plan[plan_i]} in {actions_str}"
            plan_i += 1
            applicable_actions = [a for a in applicable_actions if self._task.action_to_string(a) == plan[plan_i - 1]]
            assert len(applicable_actions) == 1, applicable_actions
            action = applicable_actions[0]
            succ_state = self._get_successor_state(action, node.state)
            node = SearchNode(
                state=succ_state,
                achieving_action=action,
                s_id=self._get_new_counter(),
                parent_s_id=node.s_id,
                g=node.g + 1,
            )
            h = self._evaluate_heuristic(succ_state)
            # print("*" * 80)
            # print(self._task.action_to_string(action))
            # print()
            # self._task.dump_state(succ_state)
            print(f"{h=}")
            if plan_i == len(plan):
                assert self._is_goal(succ_state), f"Expected goal state, got {succ_state}"
                logging.info("Plan found")
                exit(0)
            continue

    def gbfs(self) -> Optional[Plan]:
        self._search_start_time = time.perf_counter()

        # Initial state
        init_state = self._task.get_init_state()
        node = self._get_init_node(init_state)
        if self._is_goal(init_state):
            logging.info("Initial state is already a goal state!")
            return self._extract_plan(node)

        if self._debug:
            logging.info("Initial state:")
            self._task.dump_state(node.state)

        h = self._evaluate_heuristic(node.state)
        self._vis.add(node)
        expanded: set[int] = set()  # for multiqueue
        queue_h = PriorityQueue()  # heuristic
        queue_p = PriorityQueue()  # preferred operators

        if h is not None:
            queue_h.push(node, h)
            to_pop_from_h = True
        else:
            queue_p.push(node, 0)

        while True:
            # get node
            if self._bound != -1 and self._n_exp >= self._bound:
                self._extract_plan(None)
                logging.info("Bound reached, terminating planning.")
                return

            match self._mode:
                case Mode.POLICY:
                    if len(queue_p) == 0:
                        break
                    # if self._n_eval_p >= 10000:
                    #     self._extract_plan(None)
                    #     logging.error("Policy ran for 10000 steps. Terminating as this is likely a cycle.")
                    #     return None
                    node = queue_p.pop()
                case Mode.HEURISTIC:
                    if len(queue_h) == 0:
                        break
                    node = queue_h.pop()
                case _:
                    to_pop_from_h = to_pop_from_h and (len(queue_h) > 0) or (len(queue_p) == 0)
                    node = None
                    while node is None and (queue_h or queue_p):
                        if to_pop_from_h:
                            node = queue_h.pop()
                            to_pop_from_h = False
                        else:
                            node = queue_p.pop()
                            to_pop_from_h = True
                        if node.s_id in expanded:
                            node = None
                    if node is None and not (queue_h or queue_p):
                        break

            # get successors
            expanded.add(node.s_id)
            applicable_actions = self._get_applicable_actions(node)
            if len(applicable_actions) == 0:
                continue

            if self._n_exp % 10000 == 0:
                logging.info(f"{self._n_exp=}, {self._n_eval_h=}, {self._n_eval_p=}")
                # for a in applicable_actions:
                #     print(self._task.action_to_string(a))
                # breakpoint()

            def handle_action(node: SearchNode, action: SGAction, queue: PriorityQueue) -> None:
                succ_state = self._get_successor_state(action, node.state)
                succ_node = SearchNode(
                    state=succ_state,
                    achieving_action=action,
                    s_id=self._get_new_counter(),
                    parent_s_id=node.s_id,
                    g=node.g + 1,
                )

                if self._is_goal(succ_state):
                    return self._extract_plan(succ_node)

                if self._heuristic is None:  # policy mode
                    queue.push(succ_node, 0)
                elif not self._vis.contains(succ_state):  # heuristic or heuristic + policy mode
                    h = self._evaluate_heuristic(succ_state)
                    if h == float("inf"):
                        self._n_dead += 1
                        return None
                    queue.push(succ_node, h)

                self._vis.add(succ_node)

                return None

            policy_action = None
            if self._policy:
                policy_action = self._evaluate_policy(node.state, applicable_actions)
                plan = handle_action(node, policy_action, queue_p)
                if plan is not None:
                    return plan

            if self._heuristic:
                for action in applicable_actions:
                    if action == policy_action:
                        continue
                    plan = handle_action(node, action, queue_h)
                    if plan is not None:
                        return plan

        self._extract_plan(None)
        logging.info("Search space exhausted, no solution found")
        return None

    def dump_stats(self) -> None:

        def float_format(x: float) -> str:
            return f"{x:.4f}"

        def percentage(x: float, y: float) -> str:
            return f"{100 * x / y:.1f}%"

        dividers = ("*", "*", "*")

        t_aag_setup = self._aag.t_aag_processing
        t_aag_exec = self._aag.t_aag_execution
        t_aag_total = t_aag_setup + t_aag_exec
        t_rem_search = self._t_search - (self._t_eval_h + self._t_eval_p + t_aag_total + self._t_sg)

        stats = [
            dividers,
            ("plan length", self._plan_length),
            ("num expansions", self._n_exp),
            ("num heuristic evaluations", self._n_eval_h),
            ("num policy evaluations", self._n_eval_p),
            ("num deadends", self._n_dead),
            dividers,
            ("time parsing", float_format(self._t_parsing)),
            dividers,
            ("time heuristic evaluations", float_format(self._t_eval_h), percentage(self._t_eval_h, self._t_search)),
            ("time policy evaluations", float_format(self._t_eval_p), percentage(self._t_eval_p, self._t_search)),
            # ("time applicable action generation", float_format(self._t_aag), percentage(self._t_aag, self._t_search)),
            ("time action generation setup", float_format(t_aag_setup), percentage(t_aag_setup, self._t_search)),
            ("time action generation execution", float_format(t_aag_exec), percentage(t_aag_exec, self._t_search)),
            ("time successor generation", float_format(self._t_sg), percentage(self._t_sg, self._t_search)),
            ("time other search routines", float_format(t_rem_search), percentage(t_rem_search, self._t_search)),
            ("total search time", float_format(self._t_search), percentage(self._t_search, self._t_search)),
            dividers,
            ("total planning time", float_format(self._t_plan)),
            dividers,
        ]

        logging.info(f"Planner statistics:\n" + mat_to_str(stats, rjust=[False, True, True]))
        if self._debug:
            self._aag.dump_profiling()
        if self._debug:
            self._aag.dump_profiling()
