import logging
import subprocess
from typing import Optional

import pddl
from pddl.core import Domain, Problem
from pddl.logic import Predicate
from pddl.logic.base import Formula

from util.paths import PLANNERS_DIR


Plan = list[str]
State = frozenset[Predicate]


def get_domain(domain: str | Domain) -> Domain:
    if isinstance(domain, str):
        domain = pddl.parse_domain(domain)
    assert isinstance(domain, Domain)
    return domain


def get_problem(problem: str | Problem) -> Problem:
    if isinstance(problem, str):
        problem = pddl.parse_problem(problem)
    assert isinstance(problem, Problem)
    return problem


def is_numeric_domain(domain: Domain) -> bool:
    return len(domain.functions) > 0


def new_problem(domain: Domain, problem: Problem, state: State, goal: Optional[Formula] = None) -> Problem:
    if goal is None:
        goal = problem.goal
    return Problem(
        name=f"p{hash(state)}{hash(goal)}",
        domain=domain,
        objects=problem.objects,
        init=state,
        goal=goal,
    )


def collect_plan(plan_file: str) -> Plan:
    plan = []
    for line in open(plan_file, "r").readlines():
        if line.startswith(";"):
            continue
        plan.append(line.strip())
    return plan


def get_plan(domain_path: str, problem_path: str, timeout: int = 1) -> Optional[list[Plan]]:
    sas_file = "output.sas"
    plan_file = "plan.plan"
    timeout = int(timeout)
    cmd = [
        f"{PLANNERS_DIR}/downward/fast-downward.py",
        "--sas-file",
        sas_file,
        "--plan-file",
        plan_file,
        "--translate-time-limit",
        str(timeout),
        "--search-time-limit",
        str(timeout),
        domain_path,
        problem_path,
        "--search",
        "astar(lmcut())",
    ]

    run = subprocess.run(cmd, capture_output=True, text=True, check=False)
    stdout = run.stdout
    stderr = run.stderr
    logging.debug(stdout)
    logging.debug(stderr)
    solved = any(substr in stdout for substr in ["Solution found", "Found Plan"])

    if solved:
        return collect_plan(plan_file)
    else:
        return None
