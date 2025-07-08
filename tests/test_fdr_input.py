import pytest

from util import train_plan


BENCHMARK_GROUP = "ipc23lt"
CONFIG_NAME = "classic"


@pytest.mark.parametrize(
    "domain_name,problem_name",
    [
        ("blocksworld", "0_01"),
    ],
)
def test_ipc23lt(request: pytest.FixtureRequest, domain_name: str, problem_name: str) -> None:
    train_plan(
        request=request,
        domain_name=domain_name,
        benchmark_group=BENCHMARK_GROUP,
        problem_name=problem_name,
        config_name=CONFIG_NAME,
        fdr_input=True,
    )
