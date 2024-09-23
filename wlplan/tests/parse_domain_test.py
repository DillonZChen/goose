import logging

import pytest
from ipc23lt import DOMAINS, get_domain_pddl

from wlplan.planning import parse_domain

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", sorted(DOMAINS))
def test_profile(domain_name):
    domain_pddl = get_domain_pddl(domain_name)
    domain = parse_domain(domain_pddl)
    LOGGER.info(domain)
