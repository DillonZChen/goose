import pytest
from fixtures import get_data_input_argument

from util import execute_command, get_command_prefix


def test_distinguishability_test(request: pytest.FixtureRequest):
    """Tests whether code does not crash when running the distinguishability test."""
    script = get_command_prefix(request, script="train")
    data_config = get_data_input_argument(benchmark_group="ipc23lt", domain_name="blocksworld")
    cmd = f"{script} {data_config} --distinguish-test"

    execute_command(cmd)
