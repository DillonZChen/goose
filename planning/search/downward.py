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
    blind = "blind()"
    gc = "goalcount()"
    ff = "ff()"
    wl = f'wlnov(iterations={opts.iterations},graph="{opts.graph_representation}")'
    match opts.planner:
        case Planner.BLIND:
            heuristics = [blind]
        case Planner.WL:
            heuristics = [wl]
        case Planner.GC:
            heuristics = [gc]
        case Planner.FF:
            heuristics = [ff]
        case Planner.GC_WL:
            heuristics = [gc, wl]
        case Planner.FF_WL:
            heuristics = [ff, wl]
        case Planner.FF_GC:
            heuristics = [ff, gc]
        case Planner.FF_GC_WL:
            heuristics = [ff, gc, wl]
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
