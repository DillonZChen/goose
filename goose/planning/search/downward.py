from goose.util.paths import DOWNWARD_SCRIPT
from goose.util.shell import execute_cmd


def run_downward_pddl(domain_path: str, problem_path: str, config: list[str], opts) -> None:
    cmd = [
        "python3",
        DOWNWARD_SCRIPT,
        "--sas-file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        domain_path,
        problem_path,
    ]
    cmd += config
    execute_cmd(cmd)


def run_downward_fdr(sas_path: str, config: list[str], opts) -> None:
    cmd = [
        "python3",
        DOWNWARD_SCRIPT,
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        sas_path,
    ]
    cmd += config
    execute_cmd(cmd)
