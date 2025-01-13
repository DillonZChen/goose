import logging
import os
import re
import subprocess
from subprocess import PIPE

import termcolor

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
_ROOT_DIR = os.path.normpath(f"{_CUR_DIR}/..")
_PYTHON2_RECIPE = os.path.normpath(f"{_ROOT_DIR}/util/python2.def")
_PYTHON2_CONTAINER = os.path.normpath(f"{_ROOT_DIR}/python2.sif")
_PYTHON2_MSG = f"Please build the Python2 container via\n\n\t" + \
                termcolor.colored(f"apptainer build {_PYTHON2_CONTAINER} {_PYTHON2_RECIPE}\n", "magenta")


def popen(cmd):
    logging.info("This make take some time...")
    p = subprocess.Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate()
    output = output.decode("utf-8")
    err = err.decode("utf-8")
    rc = p.returncode
    return output, err, rc


def train(domain, save_path, predictor, benchmarks="ipc23lt", numeric=False):
    # if numeric and not os.path.exists(_PYTHON2_CONTAINER):
    #     logging.info(_PYTHON2_MSG)
    #     assert False
    data_config = f"configurations/data/{benchmarks}/{domain}.toml"
    model_config = f"configurations/model/{predictor}.toml"
    cmd = ["python3", "train.py", data_config, "-mc", model_config, "-s", save_path]
    # if numeric:
    #     cmd = [_PYTHON2_CONTAINER] + cmd
    cmd_str = " ".join(cmd)
    logging.critical(cmd_str)
    rc = os.system(cmd_str)
    assert rc == 0


def parse_output(output, planner):
    stats = {
        "solved": False,
        "time": float("inf"),
        "plan_length": float("inf"),
        "expanded": float("inf"),
    }

    if planner == "pwl":
        for line in output.split("\n"):
            if line.startswith("Goal found at"):
                stats["solved"] = True
                stats["time"] = float(line.split(" ")[-1])
            elif line.startswith("Plan length"):
                stats["plan_length"] = int(line.split(" ")[-2])
            elif line.startswith("Expanded") and stats["expanded"] == float("inf"):
                stats["expanded"] = int(line.split(" ")[-2])
    elif planner in {"fd", "nfd"}:
        if "Solution found!" in output:
            stats["solved"] = True
            output = output.split("Solution found!")[1]

            match = re.search(r"Plan length: (\d+)", output)
            if match:
                stats["plan_length"] = int(match.group(1))

            match = re.search(r"Total time: (\d+\.\d+)", output)
            if match:
                stats["time"] = float(match.group(1))

            match = re.search(r"Expanded (\d+)", output)
            if match:
                stats["expanded"] = int(match.group(1))
    else:
        raise NotImplementedError

    solved = stats["solved"]
    time = stats["time"]
    plan_length = stats["plan_length"]
    expanded = stats["expanded"]
    logging.info(f"{solved=}")
    logging.info(f"{time=}")
    logging.info(f"{plan_length=}")
    logging.info(f"{expanded=}")
    return stats


def plan(domain, problem, evaluator, planner, benchmarks="ipc23lt", numeric=False, **kwargs):
    # if numeric and not os.path.exists(_PYTHON2_CONTAINER):
    #     logging.info(_PYTHON2_MSG)
    #     assert False
    domain_pddl = f"benchmarks/{benchmarks}/{domain}/domain.pddl"
    problem_pddl = f"benchmarks/{benchmarks}/{domain}/testing/p{problem}.pddl"
    cmd = ["python3", "plan.py", domain_pddl, problem_pddl, evaluator, "-t", "60", "-p", planner]
    # if numeric:
    #     cmd = [_PYTHON2_CONTAINER] + cmd
    cmd_str = " ".join(cmd)
    logging.critical(cmd_str)
    output, err, rc = popen(cmd)
    stats = parse_output(output, planner)
    assert rc == 0, cmd_str
    if expected_expanded_ub := kwargs.get("expected_expanded_ub"):
        assert stats["expanded"] <= expected_expanded_ub, (stats["expanded"], expected_expanded_ub)
