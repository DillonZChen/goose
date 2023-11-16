import planning
from typing import FrozenSet, TypeVar
from planning.translate.pddl import Task

Proposition = TypeVar("Proposition", bound=str)
State = FrozenSet[Proposition]


def get_planning_problem(domain_pddl: str, problem_pddl: str):
    problem: Task = planning.translate.pddl_parser.open(
        domain_filename=domain_pddl, task_filename=problem_pddl
    )

    return problem
