import subprocess

from planning.util import PLANNERS_DIR


def run_downward(domain_path: str, problem_path: str, wlf_params_path: str, opts) -> None:
    h_goose = f'wlgoose(model_file="{wlf_params_path}")'

    if domain_path == "fdr":
        cmd = [
            "python3",
            f"{PLANNERS_DIR}/downward/fast-downward.py",
            problem_path,
            "--search",
            f"eager_greedy([{h_goose}])",
        ]
    else:
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
    subprocess.check_call(list(map(str, cmd)))
