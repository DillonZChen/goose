import pytest
from fixtures import CONFIGS, get_data_input_argument

from util import execute_command, get_command_prefix


@pytest.mark.parametrize("domain_name", ["blocksworld", "childsnack", "satellite"])
@pytest.mark.parametrize("config", CONFIGS)
def test_train_ipc23lt(request: pytest.FixtureRequest, domain_name: str, config: dict[str, str]) -> None:
    script = get_command_prefix(request, "train")
    data_config = get_data_input_argument(benchmark_group="ipc23lt", domain_name=domain_name)
    cmd = f"{script} {data_config} --num-data=1"
    for k, v in config.items():
        cmd += f" --{k.replace('_', '-')}={v}"

    execute_command(cmd)
