import logging
import os
import re
import subprocess
from subprocess import PIPE
from typing import Any, Optional

import pytest


def get_command_prefix(request: pytest.FixtureRequest, script: str) -> str:
    """Use apptainer or local script as specified by the command line option."""
    if request.config.getoption("--apptainer"):
        assert os.path.exists("./goose.sif"), "Apptainer image 'goose.sif' not found. Please build it first."
        return f"./goose.sif {script}"
    else:
        return f"python3 {script}.py"


def execute_command(cmd: str) -> None:
    """Executes a shell command and asserts that it returns 0."""
    logging.info(f"Executing command\n\n\t{cmd}\n")

    rc = os.system(cmd)

    assert rc == 0


def popen_command(cmd: str) -> tuple[str, str, int]:
    """Executes a shell command the output, error, and return code."""
    logging.info(f"Executing command\n\n\t{cmd}\n")

    logging.info("This make take some time...")
    p = subprocess.Popen(cmd.split(), stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate()
    output = output.decode("utf-8")
    err = err.decode("utf-8")
    rc = p.returncode
    return output, err, rc


def train_plan(
    request: pytest.FixtureRequest,
    domain_name: str,
    benchmark_group: str,
    problem_name: str,
    config_name: str,
    planner_name: str,
    timeout: int = 60,
    expanded_ub: Optional[int] = None,
) -> None:
    model_dir = "tests/models"
    os.makedirs(model_dir, exist_ok=True)
    model_path = f"{model_dir}/" + "-".join([domain_name, benchmark_group, config_name]) + ".model"
    train(
        request=request,
        domain_name=domain_name,
        benchmark_group=benchmark_group,
        config_name=config_name,
        model_path=model_path,
    )
    plan(
        request=request,
        domain_name=domain_name,
        benchmark_group=benchmark_group,
        problem_name=problem_name,
        model_path=model_path,
        planner=planner_name,
        timeout=timeout,
        expanded_ub=expanded_ub,
    )


def train(
    request: pytest.FixtureRequest,
    domain_name: str,
    benchmark_group: str,
    config_name: str,
    model_path: str,
) -> None:
    script = get_command_prefix(request, script="train")
    data_config = f"configurations/data/{benchmark_group}/{domain_name}.toml"
    model_config = f"configurations/model/{config_name}.toml"

    cmd = f"{script} {data_config} {model_config} -s {model_path}"

    execute_command(cmd)


def plan(
    request: pytest.FixtureRequest,
    domain_name: str,
    benchmark_group: str,
    problem_name: str,
    model_path: str,
    planner: str,
    timeout: int = 60,
    expanded_ub: Optional[int] = None,
) -> None:
    script = get_command_prefix(request, script="plan")
    domain_pddl = f"benchmarks/{benchmark_group}/{domain_name}/domain.pddl"
    problem_pddl = f"benchmarks/{benchmark_group}/{domain_name}/testing/p{problem_name}.pddl"

    cmd = f"{script} {domain_pddl} {problem_pddl} {model_path} -t {timeout} -p {planner}"

    output, err, rc = popen_command(cmd)
    stats = parse_output(output, planner)
    if rc != 0:
        logging.info(f"OUTPUT:\n{output}\n")
        logging.info(f"ERROR:\n{err}\n")
    assert rc == 0, cmd
    if expanded_ub:
        assert stats["expanded"] <= expanded_ub, (stats["expanded"], expanded_ub)


def parse_output(output: str, planner: str) -> dict[str, Any]:
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
