import logging
import subprocess
from typing import Any

import termcolor as tc


def execute_cmd(cmd: list[Any]):
    cmd = list(map(str, cmd))
    cmd_str = " ".join(cmd)
    cmd_str = tc.colored(cmd_str, "magenta")
    logging.info(f"Executing command\n\n\t{cmd_str}\n")
    subprocess.check_call(cmd)
