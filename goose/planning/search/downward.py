from goose.enums.planner import Planner
from goose.util.paths import PLANNERS_DIR
from goose.util.shell import execute_cmd


def run_downward_pddl(domain_path: str, problem_path: str, config: list[str], opts) -> None:
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
        # "--search",
        # f"eager_greedy([{h_goose}])",
    ]
    cmd += config
    execute_cmd(cmd)


def run_downward_fdr(sas_path: str, config: list[str], opts) -> None:
    cmd = [
        "python3",
        f"{PLANNERS_DIR}/downward/fast-downward.py",
        "--plan-file",
        opts.plan_file,
        "--search-time-limit",
        opts.timeout,
        sas_path,
        # "--search",
        # f"{h_goose}])",
    ]
    cmd += config
    execute_cmd(cmd)


def run_downward_standalone(domain_path: str, problem_path: str, opts) -> None:
    BLIND = "blind()"
    GC = "goalcount()"
    ADD = "add()"
    FF = "ff()"

    QBWLGC = f"qbwl(eval={GC})"
    QBWLADD = f"qbwl(eval={ADD})"
    QBWLFF = f"qbwl(eval={FF})"

    QBPNGC = f"qbpn(eval={GC})"
    QBPNADD = f"qbpn(eval={ADD})"
    QBPNFF = f"qbpn(eval={FF})"

    QBPNWLGC = f"qbpnwl(eval={GC})"
    QBPNWLADD = f"qbpnwl(eval={ADD})"
    QBPNWLFF = f"qbpnwl(eval={FF})"

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
        case Planner.DOWNWARD_QBPNWLGC:
            heuristics = [QBPNWLGC]
        case Planner.DOWNWARD_QBPNWLADD:
            heuristics = [QBPNWLADD]
        case Planner.DOWNWARD_QBPNWLFF:
            heuristics = [QBPNWLFF]
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
