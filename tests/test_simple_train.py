import pytest
from fixtures import CONFIGS

from util import execute_command, get_command_prefix


@pytest.mark.parametrize("domain_name", ["blocksworld", "childsnack", "satellite"])
@pytest.mark.parametrize("config", CONFIGS)
def test_train_ipc23lt(request: pytest.FixtureRequest, domain_name: str, config: dict[str, str]) -> None:
    script = get_command_prefix(request, "train")
    data_config = f"configurations/data/ipc23lt/{domain_name}.toml"
    cmd = f"{script} {data_config} --num-data=1"
    for k, v in config.items():
        cmd += f" --{k.replace('_', '-')}={v}"

    execute_command(cmd)
