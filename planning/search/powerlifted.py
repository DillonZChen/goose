from enums.planner import Planner
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


def run_powerlifted_standalone(domain_path: str, problem_path: str, opts) -> None:
    match opts.planner:
        case Planner.POWERLIFTED_BLIND:
            heuristic = "blind"
            search = "gbfs"
        case Planner.POWERLIFTED_GC:
            heuristic = "goalcount"
            search = "gbfs"
        case Planner.POWERLIFTED_FF:
            heuristic = "ff"
            search = "gbfs"
        case Planner.POWERLIFTED_WLNS_FF:
            heuristic = "ff"
            search = "wlns"
        case _:
            raise NotImplementedError(f"{opts.planner=}")

    cmd = [
        "python3",
        f"{PLANNERS_DIR}/powerlifted/powerlifted.py",
        "-s",
        search,
        "-d",
        domain_path,
        "-i",
        problem_path,
        "-g",
        "clique_kckp",  # supports negative preconditions
        "--time-limit",
        opts.timeout,
        "-e",
        heuristic,
        "--translator-output-file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
    ]
    execute_cmd(cmd)
