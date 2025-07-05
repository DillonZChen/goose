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

from enums.mode import Mode
from enums.planner import Planner
from enums.serialisation import namespace_from_serialisable
from enums.state_representation import StateRepresentation
from planning.policy.wlf_policy import WlfPolicyExecutor
from planning.search.downward import run_downward
from planning.search.numeric_downward import run_numeric_downward
from planning.search.powerlifted import run_powerlifted
from planning.util import PLANNERS_DIR
from util.filesystem import file_exists
from util.logging import init_logger, log_opts


_DESCRIPTION = """GOOSE planner script.
  WLF models represent value functions for heuristic search.
  GNN models represent action policies as reactive controllers.
"""


# fmt: off
def get_planning_parser():
    parser = argparse.ArgumentParser(description=_DESCRIPTION, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("domain_path", type=str,
                        help="Path to PDDL domain file or `fdr` if using an FDR input.")
    parser.add_argument("problem_path", type=str,
                        help="Path to PDDL problem file or FDR file.")
    parser.add_argument("model_path", type=str,
                        help="Path to the model file.")
    parser.add_argument("-p", "--planner", type=Planner.parse, default=None,
                        choices=Planner.choices(),
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

    domain_path = opts.domain_path
    problem_path = opts.problem_path
    model_path = opts.model_path

    assert file_exists(domain_path) or domain_path == "fdr", domain_path
    assert file_exists(problem_path), problem_path
    assert file_exists(model_path), model_path

    # Load from model path
    with zipfile.ZipFile(model_path, "r") as zf:
        zf.extractall()
    params_path = f"{model_path}.params"
    opts_path = f"{model_path}.opts"
    train_opts = json.load(open(opts_path, "r"))
    train_opts = argparse.Namespace(**train_opts)
    train_opts = namespace_from_serialisable(train_opts)

    # Automatically detect planner if not specified
    def set_planner(planner: Planner):
        opts.__dict__["planner"] = planner
        logging.info(f"Automatically set planner to {planner.value}")

    if opts.planner is None:
        if train_opts.policy_type.is_not_search():
            set_planner(Planner.POLICY)
        else:
            match train_opts.facts:
                case StateRepresentation.FD:
                    set_planner(Planner.FD)
                case StateRepresentation.NFD:
                    set_planner(Planner.NFD)
                case StateRepresentation.ALL | StateRepresentation.NO_STATIC:
                    set_planner(Planner.PWL)
                case _:
                    raise ValueError(f"Unknown value {train_opts.facts=}")
    if domain_path == "fdr":
        assert opts.planner == Planner.FD, "FDR input is only supported with Fast Downward `--planner=fd`"

    # Log plan options
    log_opts(desc="plan", opts=opts)

    # Log train options
    log_opts(desc="train", opts=train_opts)

    match opts.planner:
        case Planner.PWL:
            run_powerlifted(domain_path=domain_path, problem_path=problem_path, wlf_params_path=params_path, opts=opts)
        case Planner.FD:
            run_downward(domain_path=domain_path, problem_path=problem_path, wlf_params_path=params_path, opts=opts)
        case Planner.NFD:
            run_numeric_downward(
                domain_path=domain_path, problem_path=problem_path, wlf_params_path=params_path, opts=opts
            )
        case Planner.POLICY:
            kwargs = {
                "domain_path": domain_path,
                "problem_path": problem_path,
                "params_path": params_path,
                "train_opts": train_opts,
                "debug": opts.debug,
                "bound": opts.bound,
            }

            match train_opts.mode:
                case Mode.WLF:
                    policy = WlfPolicyExecutor(**kwargs)
                case Mode.GNN:
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
                    from planning.policy.gnn_policy import GnnPolicyExecutor

                    policy = GnnPolicyExecutor(**kwargs)
                case _:
                    raise ValueError(f"Unknown value {train_opts.mode=}")

            random.seed(opts.random_seed)
            plan = policy.execute()
            policy.dump_stats()

        case _:
            raise ValueError(f"Unknown value {opts.planner=}")

    if os.path.exists(opts.intermediate_file):
        os.remove(opts.intermediate_file)


if __name__ == "__main__":
    main()
