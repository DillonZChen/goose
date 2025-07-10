from enums.planner import Planner
from util.paths import PLANNERS_DIR
from util.shell import execute_cmd


def run_downward_pddl(domain_path: str, problem_path: str, wlf_params_path: str, opts) -> None:
    h_goose = f'wlgoose(model_file="{wlf_params_path}")'
    cmd = [
        "python3",
        f"{PLANNERS_DIR}/downward/fast-downward.py",
        "--sas-file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        domain_path,
        problem_path,
        "--search",
        f"eager_greedy([{h_goose}])",
    ]
    execute_cmd(cmd)


def run_downward_fdr(sas_path: str, wlf_params_path: str, opts) -> None:
    h_goose = f'wlgoose(model_file="{wlf_params_path}")'
    cmd = [
        "python3",
        f"{PLANNERS_DIR}/downward/fast-downward.py",
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        sas_path,
        "--search",
        f"eager_greedy([{h_goose}])",
    ]
    execute_cmd(cmd)


def run_downward_standalone(domain_path: str, problem_path: str, opts) -> None:
    BLIND = "blind()"
    GC = "goalcount()"
    ADD = "add()"
    FF = "ff()"

    QBWLGC = 'qbwl(h="gc")'
    QBWLADD = 'qbwl(h="add")'
    QBWLFF = 'qbwl(h="ff")'

    QBPNGC = 'qbpn(h="gc")'
    QBPNADD = 'qbpn(h="add")'
    QBPNFF = 'qbpn(h="ff")'

    match opts.planner:
        case Planner.DOWNWARD_BLIND:
            heuristics = [BLIND]
        case Planner.DOWNWARD_GC:
            heuristics = [GC]
        case Planner.DOWNWARD_ADD:
            heuristics = [ADD]
        case Planner.DOWNWARD_FF:
            heuristics = [FF]
        case Planner.DOWNWARD_QBPNGC:
            heuristics = [QBPNGC]
        case Planner.DOWNWARD_QBPNADD:
            heuristics = [QBPNADD]
        case Planner.DOWNWARD_QBPNFF:
            heuristics = [QBPNFF]
        case Planner.DOWNWARD_QBWLGC:
            heuristics = [QBWLGC]
        case Planner.DOWNWARD_QBWLADD:
            heuristics = [QBWLADD]
        case Planner.DOWNWARD_QBWLFF:
            heuristics = [QBWLFF]
        case _:
            raise NotImplementedError(f"{opts.planner=}")
    heuristics = ", ".join(heuristics)
    cmd = [
        "python3",
        f"{PLANNERS_DIR}/downward/fast-downward.py",
        "--sas-file",
        opts.intermediate_file,
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        domain_path,
        problem_path,
        "--search",
        f"eager_greedy([{heuristics}])",
    ]
    execute_cmd(cmd)
