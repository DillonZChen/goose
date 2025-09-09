import pytest
from test_utils import execute_command, get_command_prefix, get_domain_pddl, get_problem_pddl


BENCHMARK_GROUP = "ipc23lt"
CONFIG_NAME = "classic"
DOMAIN_PROBLEMS = [
    ("blocksworld", "0_01"),
    ("childsnack", "0_01"),
    ("ferry", "0_01"),
]
PLANNER_CONFIGS = [
    ("downward", '--search eager_greedy([qbatwl(eval=ff(),g="ilg",l=2,w="wl")])'),
    ("downward", '--search eager_greedy([qbatwl(eval=ff(),g="ploig",l=2,w="wl")])'),
    ("downward", '--search eager_greedy([qbatwl(eval=ff(),g="ilg",l=2,w="lwl2")])'),
    ("downward", '--search eager_greedy([qbatwl(eval=ff(),g="ploig",l=2,w="lwl2")])'),
    ("downward", '--search eager_greedy([qbatwl(eval=ff(),g="ilg",l=2,w="iwl")])'),
    ("downward", '--search eager_greedy([qbatwl(eval=ff(),g="ploig",l=2,w="iwl")])'),
    ("powerlifted", "-s gbfs -e qbatwlff"),
]


@pytest.mark.parametrize("domain_name,problem_name", DOMAIN_PROBLEMS)
@pytest.mark.parametrize("planner,config", PLANNER_CONFIGS)
def test_standalone_downward(
    request: pytest.FixtureRequest, domain_name: str, problem_name: str, planner: str, config: str
) -> None:
    script = get_command_prefix(request, script="plan")
    domain_pddl = get_domain_pddl(BENCHMARK_GROUP, domain_name)
    problem_pddl = get_problem_pddl(BENCHMARK_GROUP, domain_name, problem_name)
    cmd = f"{script} {domain_pddl} {problem_pddl} --config '{config}' --planner '{planner}'"

    execute_command(cmd)
