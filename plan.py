#!/usr/bin/env python

import argparse
import os
import subprocess

from planning.util import PLANNERS_DIR, is_numeric
from util.logging import init_logger


def parse_opts():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_pddl", type=str)
    parser.add_argument("problem_pddl", type=str)
    parser.add_argument("model_path", type=str)
    parser.add_argument("-t", "--timeout", type=int, default=1800)
    parser.add_argument("-p", "--planner", type=str, default="fd", choices=["pwl", "fd", "nfd", "policy"])
    parser.add_argument("-o", "--plan_file", type=str, default="plan.plan")
    parser.add_argument("--intermediate-file", type=str, default="intermediate.tmp")
    opts = parser.parse_args()
    return opts


def main():
    init_logger()
    opts = parse_opts()

    domain_pddl = opts.domain_pddl
    if domain_pddl == "fdr":
        assert opts.planner == "fd", "FDR inputs are only supported with Fast Downward"
    else:
        assert os.path.exists(domain_pddl), domain_pddl
        if is_numeric(domain_pddl):
            if opts.planner != "nfd":
                print("Domain is numeric so switching planner to nfd.")
            opts.planner = "nfd"

    problem_pddl = opts.problem_pddl
    assert os.path.exists(problem_pddl), problem_pddl

    model_path = opts.model_path
    assert os.path.exists(model_path), model_path

    planner = opts.planner
    timeout = str(opts.timeout)

    match planner:
        case "pwl":
            cmd = [
                "python3",
                f"{PLANNERS_DIR}/powerlifted/powerlifted.py",
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
                opts.intermediate_file,
                "--plan-file",
                opts.plan_file,
            ]
        case "fd":
            h_goose = f'wlgoose(model_file="{model_path}")'

            if domain_pddl == "fdr":
                cmd = [
                    "python3",
                    f"{PLANNERS_DIR}/downward/fast-downward.py",
                    problem_pddl,
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
                    timeout,
                    domain_pddl,
                    problem_pddl,
                    "--search",
                    f"eager_greedy([{h_goose}])",
                ]
        case "nfd":
            h_goose = f"wlgoose(model_path={model_path},domain_path={domain_pddl},problem_path={problem_pddl})"

            cmd = [
                "python2",  # nfd defines a pddl module which clashes with the pddl package
                f"{PLANNERS_DIR}/numeric-downward/fast-downward.py",
                "--build",
                "release64",
                "--sas_file",
                opts.intermediate_file,
                "--plan-file",
                opts.plan_file,
                "--search-time-limit",
                timeout,
                domain_pddl,
                problem_pddl,
                "--search",
                f"eager_greedy({h_goose})",
            ]
        case _:
            raise NotImplementedError

    subprocess.check_call(cmd)

    if os.path.exists(opts.intermediate_file):
        os.remove(opts.intermediate_file)


if __name__ == "__main__":
    main()
