import logging
import re
import subprocess
from subprocess import PIPE, STDOUT


def popen(cmd):
    logging.info("This make take some time...")
    p = subprocess.Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate()
    output = output.decode("utf-8")
    err = err.decode("utf-8")
    rc = p.returncode
    return output, err, rc


def _log_subprocess_output(pipe):
    for line in iter(pipe.readline, b""):  # b'\n'-separated lines
        line = line.decode("utf-8").strip()
        logging.info(line)


def train(domain, save_path, predictor="gpr_2"):
    data_config = f"configurations/data/ipc23lt/{domain}.toml"
    model_config = f"configurations/model/{predictor}.toml"
    cmd = ["python3", "train.py", data_config, model_config, "-s", save_path]
    cmd_str = " ".join(cmd)
    logging.critical(cmd_str)
    process = subprocess.Popen(cmd, stdout=PIPE, stderr=STDOUT)
    with process.stdout:
        _log_subprocess_output(process.stdout)
    rc = process.wait()  # 0 means success
    assert rc == 0, cmd_str


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
    elif planner == "fd":
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


def plan(domain, problem, evaluator, planner, **kwargs):
    domain_pddl = f"benchmarks/ipc23lt/{domain}/domain.pddl"
    problem_pddl = f"benchmarks/ipc23lt/{domain}/testing/p{problem}.pddl"
    cmd = ["python3", "plan.py", domain_pddl, problem_pddl, evaluator, "-t", "60", "-p", planner]
    cmd_str = " ".join(cmd)
    logging.critical(cmd_str)
    output, err, rc = popen(cmd)
    stats = parse_output(output, planner)
    assert rc == 0, cmd_str
    if expected_expanded_ub := kwargs.get("expected_expanded_ub"):
        assert stats["expanded"] <= expected_expanded_ub, (stats["expanded"], expected_expanded_ub)
