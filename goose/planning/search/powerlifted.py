from goose.util.paths import PLANNERS_DIR
from goose.util.shell import execute_cmd


def run_powerlifted(domain_path: str, problem_path: str, config: list[str], opts) -> None:
    cmd = [
        "python3",
        f"{PLANNERS_DIR}/powerlifted/powerlifted.py",
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
