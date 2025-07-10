import pytest
from test_utils import train_plan


BENCHMARK_GROUP = "ipc23lt"
CONFIG_NAME = "classic"


@pytest.mark.parametrize(
    "domain_name,problem_name",
    [
        ("blocksworld", "1_01"),
    ],
)
def test_pwl(request: pytest.FixtureRequest, domain_name: str, problem_name: str) -> None:
    train_plan(
        request=request,
        domain_name=domain_name,
        benchmark_group=BENCHMARK_GROUP,
        problem_name=problem_name,
        config_name=CONFIG_NAME,
        planner_name="powerlifted",
    )
