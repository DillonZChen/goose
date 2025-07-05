import subprocess

from planning.util import PLANNERS_DIR


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
    subprocess.check_call(list(map(str, cmd)))
