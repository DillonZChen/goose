import subprocess

from util.paths import PLANNERS_DIR
from util.shell import execute_cmd


def run_powerlifted(domain_path: str, problem_path: str, wlf_params_path: str, opts) -> None:
    cmd = [
        "python3",
        f"{PLANNERS_DIR}/powerlifted/powerlifted.py",
        "-s",
        "gbfs",
        "-d",
        domain_path,
        "-i",
        problem_path,
        "-g",
        "clique_kckp",  # supports negative preconditions
        "--time-limit",
        opts.timeout,
        "-e",
        "wlgoose",
        "-m",
        wlf_params_path,
        "--translator-output-file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
    ]
    execute_cmd(cmd)
