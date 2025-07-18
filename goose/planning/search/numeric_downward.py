import subprocess

from goose.util.paths import PLANNERS_DIR
from goose.util.shell import execute_cmd


def run_numeric_downward(domain_path: str, problem_path: str, wlf_params_path: str, opts) -> None:
    h_goose = f"wlgoose(model_path={wlf_params_path},domain_path={domain_path},problem_path={problem_path})"

    cmd = [
        "python2",  # nfd defines a pddl module which clashes with the pddl package
        f"{PLANNERS_DIR}/numeric-downward/fast-downward.py",
        "--build",
        "release64",
        "--sas_file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        domain_path,
        problem_path,
        "--search",
        f"eager_greedy({h_goose})",
    ]
    execute_cmd(cmd)
