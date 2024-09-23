#!/usr/bin/env python

import argparse
import os
import subprocess

from util.logging import init_logger

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))


def parse_opts():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_pddl", type=str)
    parser.add_argument("problem_pddl", type=str)
    parser.add_argument("model_path", type=str)
    parser.add_argument("-t", "--timeout", type=int, default=1800)
    parser.add_argument("-p", "--planner", type=str, default="fd", choices=["pwl", "fd", "policy"])
    parser.add_argument("-f", "--plan_file", type=str, default="plan.plan", help="Output plan file. Default: plan.plan")
    opts = parser.parse_args()
    return opts


def main():
    init_logger()
    opts = parse_opts()

    domain_pddl = opts.domain_pddl
    assert os.path.exists(domain_pddl)
    domain_pddl = os.path.abspath(domain_pddl)

    problem_pddl = opts.problem_pddl
    assert os.path.exists(problem_pddl)
    problem_pddl = os.path.abspath(problem_pddl)

    model_path = opts.model_path
    assert os.path.exists(model_path)
    model_path = os.path.abspath(model_path)

    planner = opts.planner
    timeout = str(opts.timeout)

    trash_file = ""
    for s in [domain_pddl, problem_pddl, model_path, planner, timeout]:
        trash_file += str(hash(s))
    trash_file = f"output_{trash_file}.out"
    trash_file = os.path.abspath(trash_file)

    if planner == "pwl":
        cwd = f"{_CUR_DIR}/planning/powerlifted"
        cmd = [
            "python3",
            "powerlifted.py",
            "-s",
            "gbfs",
            "-d",
            domain_pddl,
            "-i",
            problem_pddl,
            "-g",
            "clique_kckp",
            "--time-limit",
            timeout,
            "-e",
            "wlgoose",
            "-m",
            model_path,
            "--translator-output-file",
            trash_file,
            "--plan-file",
            opts.plan_file,
        ]
    elif planner == "fd":
        h_goose = f'wlgoose(model_file="{model_path}")'

        cwd = f"{_CUR_DIR}/planning/downward"
        cmd = [
            "python3",
            "fast-downward.py",
            "--sas-file",
            trash_file,
            "--plan-file",
            opts.plan_file,
            "--search-time-limit",
            timeout,
            domain_pddl,
            problem_pddl,
            "--search",
            f"eager_greedy([{h_goose}])",
        ]
    else:
        raise NotImplementedError

    subprocess.check_call(cmd, cwd=cwd)

    if os.path.exists(trash_file):
        os.remove(trash_file)


if __name__ == "__main__":
    main()
