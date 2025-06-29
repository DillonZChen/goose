import pytest

from util import execute_command, get_command_prefix


def test_distinguishability_test(request: pytest.FixtureRequest):
    """Tests whether code does not crash when running the distinguishability test."""
    prefix = get_command_prefix(request, script="train")
    cmd = f"{prefix} configurations/data/ipc23lt/blocksworld.toml --distinguish-test"

    execute_command(cmd)
