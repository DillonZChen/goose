#!/usr/bin/env python

import os
import subprocess

from planning.options import parse_planning_opts
from planning.util import PLANNERS_DIR
from util.logging import init_logger


def main():
    init_logger()
    opts = parse_planning_opts()

    domain_pddl = opts.domain_pddl
    problem_pddl = opts.problem_pddl
    model_path = opts.model_path

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
                "clique_kckp",  # supports negative preconditions
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
            subprocess.check_call(cmd)
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
            subprocess.check_call(cmd)
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
            subprocess.check_call(cmd)
        case "policy":

            # Torch and Pytorch Geometric imports done here to avoid unnecessary imports when not using GNN
            from planning.policy.gnn_policy import GnnPolicyExecutor

            policy = GnnPolicyExecutor(
                domain_file=domain_pddl,
                problem_file=problem_pddl,
                model_file=model_path,
                debug=False,
            )
            plan = policy.execute()

        case _:
            raise NotImplementedError

    if os.path.exists(opts.intermediate_file):
        os.remove(opts.intermediate_file)


if __name__ == "__main__":
    main()
