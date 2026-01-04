import os

from goose.util.paths import POWERLIFTED_BIN, POWERLIFTED_SCRIPT
from goose.util.shell import execute_cmd


def run_powerlifted(domain_path: str, problem_path: str, config: list[str], opts) -> None:
    if not os.path.exists(POWERLIFTED_BIN):
        raise FileNotFoundError(f"{POWERLIFTED_BIN} not found. Please build Powerlifted")

    cmd = [
        "python3",
        POWERLIFTED_SCRIPT,
        "-d",
        domain_path,
        "-i",
        problem_path,
        "-g",
        "clique_kckp",  # supports negative preconditions
        "--time-limit",
        opts.timeout,
        "--translator-output-file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
    ]
    cmd += config
    execute_cmd(cmd)
