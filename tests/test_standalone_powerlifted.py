import pytest

from util import execute_command, get_command_prefix, get_domain_pddl, get_problem_pddl


BENCHMARK_GROUP = "ipc23lt"
CONFIG_NAME = "classic"


@pytest.mark.parametrize("domain_name,problem_name", [("blocksworld", "0_01")])
@pytest.mark.parametrize("config", ["powerlifted(gc)", "powerlifted(ff)"])
def test_standalone_downward(request: pytest.FixtureRequest, domain_name: str, problem_name: str, config: str) -> None:
    script = get_command_prefix(request, script="plan")
    domain_pddl = get_domain_pddl(BENCHMARK_GROUP, domain_name)
    problem_pddl = get_problem_pddl(BENCHMARK_GROUP, domain_name, problem_name)
    cmd = f"{script} {domain_pddl} {problem_pddl} --planner '{config}'"

    execute_command(cmd)
