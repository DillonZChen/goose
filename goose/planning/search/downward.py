from argparse import Namespace
from typing import Optional

from goose.util.paths import DOWNWARD_SCRIPT
from goose.util.shell import execute_cmd


def run_downward(
    input1: str,
    input2: Optional[str],
    config: Optional[list[str]],
    opts: Namespace,
    alias: Optional[str] = None,
) -> None:
    cmd = ["python3", DOWNWARD_SCRIPT, "--plan-file", opts.plan_file, "--search-time-limit", opts.timeout]

    if alias is not None:
        cmd += ["--alias", alias]

    if input2 is not None:
        cmd += ["--sas-file", opts.intermediate_file, input1, input2]
    else:
        cmd += [input1]

    if config is None:
        config = []
    cmd += config

    execute_cmd(cmd)
