import logging
from collections import Counter
from typing import List

from planning import STRIPSProblem, get_strips_problem
from utils.metrics import CountMetric, metrics_logger
from utils.timer import timed

_log = logging.getLogger(__name__)


@timed("GenerateSTRIPSProblemsTime")
def generate_strips_problems(
    domain_pddl: str, domain_pddls: List[str], problem_pddls: List[str]
) -> List[STRIPSProblem]:
    """
    Generate STRIPS problems given paths to domain and problem PDDLs.
    Only one of `domain_pddl` and `domain_pddls` may be specified.

    If `domain_pddl` is specified, then it will be assumed as the
    domain PDDL file for all `problem_pddls`.

    If `domain_pddls` is specified, then each element of the list is assumed
    to be the domain PDDL file for the corresponding element in
    `problem_pddls`.

    Parameters
    ----------
    domain_pddl: str
    domain_pddls: List[str]
    problem_pddls: List[str]

    Returns
    -------
    List[STRIPSProblem]
    """
    if domain_pddl and domain_pddls:
        raise ValueError(
            "Only one of domain_pddl or domain_pddls may be specified"
        )
    else:
        # Sanity check
        if not (domain_pddl or domain_pddls):
            raise ValueError("At least one domain must be specified")

    # Generate STRIPSProblem objects
    if domain_pddl:
        problems = [
            get_strips_problem(domain_pddl, problem_pddl)
            for problem_pddl in problem_pddls
        ]
    else:
        if len(domain_pddls) != len(problem_pddls):
            raise ValueError(
                "Length of domain PDDLs must be equal to length of problem "
                "PDDLs"
            )

        problems = [
            get_strips_problem(domain_pddl, problem_pddl)
            for domain_pddl, problem_pddl in zip(domain_pddls, problem_pddls)
        ]

    # Warn if there are any non-unique problems (determined by name)
    prob_names_counter = Counter([problem.name for problem in problems])
    for prob_name, count in prob_names_counter.items():
        if count > 1:
            _log.warning(
                f"There are {count} problems with the identical problem name "
                f"{prob_name}. This may lead to unexpected behaviour."
            )

    # Metrics
    num_problems = len(problems)
    _log.info(f"Generated {num_problems} STRIPS Problems")
    metrics_logger.add_metric(
        CountMetric("NumberOfSTRIPSProblems", num_problems)
    )

    return problems


def max_number_of_preconditions(problems: List[STRIPSProblem]) -> int:
    """
    Compute the maximum number of preconditions in the actions of a list of
    STRIPS problems

    Parameters
    ----------
    problems: List[STRIPSProblem]

    Returns
    -------
    int
    """
    return max(
        len(action.preconditions)
        for problem in problems
        for action in problem.actions
    )


def max_number_of_add_effects(problems: List[STRIPSProblem]) -> int:
    """
    Compute the maximum number of additive effects in the actions of a list of
    STRIPS problems

    Parameters
    ----------
    problems: List[STRIPSProblem]

    Returns
    -------
    int
    """
    return max(
        len(action.add_effects)
        for problem in problems
        for action in problem.actions
    )
