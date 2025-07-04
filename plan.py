#!/usr/bin/env python

import argparse
import json
import logging
import os
import random
import subprocess
import sys
import zipfile

import numpy as np
import termcolor as tc

from planning.util import PLANNERS_DIR
from util.filesystem import file_exists
from util.logging import init_logger, mat_to_str


_DESCRIPTION = """GOOSE planner script.
  WLF models represent value functions for heuristic search.
  GNN models represent action policies as reactive controllers.
"""


# fmt: off
def get_planning_parser():
    parser = argparse.ArgumentParser(description=_DESCRIPTION, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("domain_pddl", type=str,
                        help="Path to PDDL domain file or `fdr` if using an FDR input.")
    parser.add_argument("problem_pddl", type=str,
                        help="Path to PDDL problem file or FDR file.")
    parser.add_argument("model_path", type=str,
                        help="Path to the model file.")
    parser.add_argument("-p", "--planner", type=str, default=None,
                        choices=["pwl", "fd", "nfd", "policy"],
                        help="Underlying planner. If not specified, it is automatically detected from the model.")
    parser.add_argument("-t", "--timeout", type=int, default=3600,
                        help="Timeout for search in seconds. Ignores all other preprocessing times.")
    parser.add_argument("-o", "--plan-file", type=str, default="sas_plan",
                        help="Location for output solution files.")
    parser.add_argument("-b", "--bound", type=int, default=-1,
                        help="Bound for policy rollouts. If not specified or --bound=-1, then do not use bound.")
    parser.add_argument("-r", "--random-seed", type=int, default=0,
                        help="Random seed for policy algorithms.")
    parser.add_argument("-d", "--debug", action="store_true",
                        help="Debug mode.")
    parser.add_argument("--intermediate-file", type=str, default="intermediate.tmp",
                        help="Location for trash files.")
    return parser
# fmt: on


def main():
    init_logger()
    parser = get_planning_parser()
    opts = parser.parse_args()

    domain_pddl = opts.domain_pddl
    problem_pddl = opts.problem_pddl
    model_path = opts.model_path

    assert file_exists(domain_pddl) or domain_pddl == "fdr", domain_pddl
    assert file_exists(problem_pddl), problem_pddl
    assert file_exists(model_path), model_path

    # Check model type
    with zipfile.ZipFile(model_path, "r") as zf:
        zf.extractall()
    params_file = f"{model_path}.params"
    opts_file = f"{model_path}.opts"
    train_opts = json.load(open(opts_file, "r"))
    mode = train_opts["mode"]

    # Automatically detect planner if not specified
    if opts.planner is None:
        if mode == "wlf":
            state_repr = train_opts["facts"]
            if state_repr == "fd":
                opts.planner == "fd"
            elif state_repr in {"all", "nostatic"}:
                opts.planner = "pwl"
            elif state_repr == "nfd":
                opts.planner = "nfd"
            else:
                raise ValueError(f"Unknown value {state_repr=}")
        if mode["policy_type"] is not None:
            opts.planner = "policy"
    if domain_pddl == "fdr":
        assert opts.planner == "fd", "FDR input is only supported with Fast Downward `--planner=fd`"

    # Log parsed options
    logging.info(f"Processed options:\n{mat_to_str([[k, tc.colored(v, 'cyan')] for k, v in vars(opts).items()])}")

    timeout = str(opts.timeout)

    match opts.planner:
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
                params_file,
                "--translator-output-file",
                opts.intermediate_file,
                "--plan-file",
                opts.plan_file,
            ]
            subprocess.check_call(cmd)
        case "fd":
            h_goose = f'wlgoose(model_file="{params_file}")'

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
            h_goose = f"wlgoose(model_path={params_file},domain_path={domain_pddl},problem_path={problem_pddl})"

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
            try:
                import torch
                import torch_geometric
            except ModuleNotFoundError:
                logging.info(
                    "The current environment does not have PyTorch and PyTorch Geometric installed. "
                    + "Please install them to use GNN architectures. Exiting."
                )
                sys.exit(1)

            from learning.predictor.neural_network.serialise import load_gnn
            from planning.policy.gnn_policy import GnnPolicyExecutor

            model, train_opts = load_gnn(model_path)

            random.seed(opts.random_seed)
            np.random.seed(opts.random_seed)
            torch.manual_seed(opts.random_seed)

            policy = GnnPolicyExecutor(
                domain_file=domain_pddl,
                problem_file=problem_pddl,
                gnn=model,
                train_opts=train_opts,
                debug=opts.debug,
                bound=opts.bound,
            )
            plan = policy.execute()
            policy.dump_stats()

        case _:
            raise NotImplementedError

    if os.path.exists(opts.intermediate_file):
        os.remove(opts.intermediate_file)


if __name__ == "__main__":
    main()
