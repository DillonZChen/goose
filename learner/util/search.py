import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import re
from representation import REPRESENTATIONS

""" Module containing useful methods and configurations for 24-AAAI search experiments. """


REPEATS = 1
VAL_REPEATS = 5
TIMEOUT = 630  # 10 minute timeout + time to load model etc.
FAIL_LIMIT = {
    "gripper": 2,
    "spanner": 10,
    "visitall": 10,
    "visitsome": 10,
    "blocks": 10,
    "ferry": 10,
    "sokoban": 20,
    "n-puzzle": 10,
}

_AUX_DIR = "aux_dir"
_PLAN_DIR = "plans"
os.makedirs(_AUX_DIR, exist_ok=True)
os.makedirs(_PLAN_DIR, exist_ok=True)


def sorted_nicely(l):
    """Sort the given iterable in the way that humans expect."""
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [convert(c) for c in re.split("([0-9]+)", key)]
    return sorted(l, key=alphanum_key)


def search_cmd(args):
    cmd = {
        "fd": fd_cmd,
        "pwl": pwl_cmd,
    }[args.planner]
    return cmd(
        df=args.domain_pddl,
        pf=args.task_pddl,
        m=args.model_path,
        search=args.search,
        timeout=args.timeout,
    )


def pwl_cmd(df, pf, m, search, timeout=TIMEOUT):
    description = f"pwl_{pf.replace('.pddl','').replace('/','-')}_{search}_{os.path.basename(m).replace('.dt', '')}"
    lifted_file = f"{_AUX_DIR}/{description}.lifted"
    plan_file = f"{_PLAN_DIR}/{description}.plan"
    cmd = (
        f"./../planners/powerlifted/powerlifted.py "
        f"-d {df} "
        f"-i {pf} "
        f"-m {m} "
        f"-e gnn "
        f"-s {search} "
        f"--time-limit {timeout} "
        f"--seed 0 "
        f"--translator-output-file {lifted_file} "
        f"--plan-file {plan_file}"
    )
    cmd = f"export GOOSE={os.getcwd()} && {cmd}"
    return cmd, lifted_file


def fd_cmd(df, pf, m, search, timeout=TIMEOUT):
    if search == "gbbfs":
        search = "batch_eager_greedy"
    elif search == "gbfs":
        search = "eager_greedy"
    else:
        raise NotImplementedError

    description = f"fd_{pf.replace('.pddl','').replace('/','-')}_{search}_{os.path.basename(m).replace('.dt', '')}"
    sas_file = f"{_AUX_DIR}/{description}.sas_file"
    plan_file = f"{_PLAN_DIR}/{description}.plan"
    cmd = (
        f"./../planners/downward/fast-downward.py --search-time-limit {timeout} --sas-file {sas_file} --plan-file {plan_file} "
        + f'{df} {pf} --search \'{search}([goose(model_path="{m}", domain_file="{df}", instance_file="{pf}")])\''
    )
    cmd = f"export GOOSE={os.getcwd()} && {cmd}"
    return cmd, sas_file
