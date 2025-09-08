import logging
import subprocess
from typing import Any

from goose.util.logging import fmt_cmd


def execute_cmd(cmd: list[Any]):
    cmd = list(map(str, cmd))
    cmd_str = " ".join(cmd)
    cmd_str = fmt_cmd(cmd_str)
    logging.info(f"Executing command\n\n{cmd_str}\n")
    subprocess.check_call(cmd)
