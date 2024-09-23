import logging

import pytest
from ipc23lt import DOMAINS, get_domain_pddl, get_problem_pddl

from wlplan.planning import parse_problem

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", sorted(DOMAINS))
def test_profile(domain_name):
    problem_pddl = get_problem_pddl(domain_name, "0_01")
    domain_pddl = get_domain_pddl(domain_name)
    problem = parse_problem(domain_pddl, problem_pddl)
    LOGGER.info(problem)
