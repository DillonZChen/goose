import pytest
from test_utils import train_plan


BENCHMARK_GROUP = "neurips24"
CONFIG_NAME = "numeric"


@pytest.mark.parametrize(
    "domain_name,problem_name",
    [
        ("childsnack", "2_30"),
        ("spanner", "2_30"),
    ],
)
def test_neurips24(request: pytest.FixtureRequest, domain_name: str, problem_name: str) -> None:
    train_plan(
        request=request,
        domain_name=domain_name,
        benchmark_group=BENCHMARK_GROUP,
        problem_name=problem_name,
        config_name=CONFIG_NAME,
    )
