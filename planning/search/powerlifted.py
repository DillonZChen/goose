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
    BLIND = "blind"
    GC = "goalcount"
    FF = "ff"

    QBWLGC = "qbwlgc"
    QBWLFF = "qbwlff"

    GBFS = "gbfs"
    DQS = "dqs"

    match opts.planner:
        case Planner.POWERLIFTED_BLIND:
            heuristic = BLIND
            search = GBFS
        case Planner.POWERLIFTED_GC:
            heuristic = GC
            search = GBFS
        case Planner.POWERLIFTED_FF:
            heuristic = FF
            search = GBFS
        case Planner.POWERLIFTED_ALT_BFWS_GC:
            heuristic = GC
            search = "alt-bfws1"
        case Planner.POWERLIFTED_ALT_BFWS_FF:
            heuristic = FF
            search = "alt-bfws1"
        case Planner.POWERLIFTED_QBWLGC:
            heuristic = QBWLGC
            search = GBFS
        case Planner.POWERLIFTED_QBWLFF:
            heuristic = QBWLFF
            search = GBFS
        case Planner.POWERLIFTED_DQS_QBWLGC:
            heuristic = QBWLGC
            search = DQS
        case Planner.POWERLIFTED_DQS_QBWLFF:
            heuristic = QBWLFF
            search = DQS
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
