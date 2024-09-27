import os
import re
from dataclasses import dataclass

MAX_VAL = 1e9


def sorted_nicely(l):
    """Sort the given list in the way that humans expect."""
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [convert(c) for c in re.split("([0-9]+)", key)]
    l.sort(key=alphanum_key)
    return l


@dataclass
class RunInfo:
    domain: str
    problem: str

    config: str
    seed: int

    tried: bool
    solved: bool
    oom: bool

    initial_heuristic: float
    time: float
    expanded: int
    evaluated: int
    plan_length: int

    error: int


def add_to_data(data: dict, job_info: RunInfo):
    for k, v in job_info.__dict__.items():
        data[k].append(v)


def read_output(
    output: str,
    domain: str,
    problem: str,
    config: str,
    seed: int,
    timeout: int,
):
    tried = False
    solved = False
    oom = False
    initial_heuristic = -1
    time = MAX_VAL
    expanded = MAX_VAL
    evaluated = MAX_VAL
    plan_length = MAX_VAL
    error = False
    if "caught signal" in output:
        error = True

    i = output.find("Initial heuristic value")
    output = output[i:]
    lines = output.split("\n")
    for line in lines:
        if line.startswith("Solution found!"):
            solved = True
        elif line.startswith("Initial heuristic value"):
            tried = True
            initial_heuristic = float(line.split()[-1])
        elif line.startswith("Actual search time"):
            time = float(line.split()[-2].replace("s", ""))
        elif line.startswith("Expanded"):
            expanded = int(line.split()[-2])
        elif line.startswith("Evaluated"):
            evaluated = int(line.split()[-2])
        elif line.startswith("Plan length"):
            plan_length = int(line.split()[-2])
        elif line.startswith("slurmstepd: error: Exceeded job memory limit"):
            oom = True

    if time >= timeout or oom:
        solved = False
        time = MAX_VAL
        expanded = MAX_VAL
        evaluated = MAX_VAL
        plan_length = MAX_VAL

    return RunInfo(
        domain=domain,
        problem=problem,
        config=config,
        seed=seed,
        tried=tried,
        solved=solved,
        oom=oom,
        initial_heuristic=initial_heuristic,
        time=time,
        expanded=expanded,
        evaluated=evaluated,
        plan_length=plan_length,
        error=error,
    )


def read_log(
    path: str,
    domain: str,
    problem: str,
    config: str,
    seed: int,
    timeout: int,
) -> RunInfo:
    if os.path.exists(path):
        with open(path, "r") as f:
            output = f.read()
            return read_output(output, domain, problem, config, seed, timeout)

    tried = False
    solved = False
    oom = False
    initial_heuristic = -1
    time = MAX_VAL
    expanded = MAX_VAL
    evaluated = MAX_VAL
    plan_length = MAX_VAL

    return RunInfo(
        domain=domain,
        problem=problem,
        config=config,
        seed=seed,
        tried=tried,
        solved=solved,
        oom=oom,
        initial_heuristic=initial_heuristic,
        time=time,
        expanded=expanded,
        evaluated=evaluated,
        plan_length=plan_length,
    )
