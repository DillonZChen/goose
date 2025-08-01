import logging
import os
import subprocess
from subprocess import PIPE
from typing import Optional

import pytest
import termcolor as tc
from fixtures import get_data_input_argument


def get_command_prefix(request: pytest.FixtureRequest, script: str) -> str:
    """Use apptainer or local script as specified by the command line option
    e.g. ./goose.sif plan.py or python3 plan.py
    """
    if request.config.getoption("--apptainer"):
        assert os.path.exists("./goose.sif"), "Apptainer image 'goose.sif' not found. Please build it first."
        return f"./goose.sif {script}"
    else:
        return f"python3 {script}.py"


def execute_command(cmd: str) -> None:
    """Executes a shell command and asserts that it returns 0."""
    cmd = tc.colored(cmd, "magenta")
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
    planner_name: Optional[str] = None,
    timeout: int = 60,
    fdr_input: bool = False,
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
        fdr_input=fdr_input,
    )


def train(
    request: pytest.FixtureRequest,
    domain_name: str,
    benchmark_group: str,
    config_name: str,
    model_path: str,
) -> None:
    script = get_command_prefix(request, script="train")
    data_config = get_data_input_argument(benchmark_group=benchmark_group, domain_name=domain_name)
    model_config = f"configurations/{config_name}.toml"

    cmd = f"{script} {data_config} {model_config} -s {model_path}"

    execute_command(cmd)


def get_domain_pddl(benchmark_group: str, domain_name: str) -> str:
    return f"benchmarks/{benchmark_group}/{domain_name}/domain.pddl"


def get_problem_pddl(benchmark_group: str, domain_name: str, problem_name: str) -> str:
    return f"benchmarks/{benchmark_group}/{domain_name}/testing/p{problem_name}.pddl"


def plan(
    request: pytest.FixtureRequest,
    domain_name: str,
    benchmark_group: str,
    problem_name: str,
    model_path: str,
    planner: Optional[str] = None,
    timeout: int = 60,
    fdr_input: bool = False,
) -> None:
    script = get_command_prefix(request, script="plan")

    if not fdr_input:
        input1 = get_domain_pddl(benchmark_group, domain_name)
        input2 = get_problem_pddl(benchmark_group, domain_name, problem_name)
    else:
        input1 = "sas"
        input2 = f"benchmarks/fdr-{benchmark_group}/{domain_name}/testing/p{problem_name}.sas"

    cmd = f"{script} {input1} {input2} {model_path} --timeout {timeout}"
    if planner is not None:
        cmd += f" --planner {planner}"

    output, err, rc = popen_command(cmd)
    sol_found = "Solution found!" in output or "Goal found at" in output
    if rc != 0:
        logging.info(f"OUTPUT:\n{output}\n")
        logging.info(f"ERROR:\n{err}\n")
    assert rc == 0, cmd
    assert sol_found, cmd
