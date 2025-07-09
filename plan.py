#!/usr/bin/env python

import argparse
import json
import logging
import os
import random
import sys
import zipfile

from enums.mode import Mode
from enums.planner import Planner
from enums.policy_type import PolicyType
from enums.serialisation import namespace_from_serialisable
from enums.state_representation import StateRepresentation
from planning.policy.wlf_policy import WlfPolicyExecutor
from planning.search.downward import run_downward_fdr, run_downward_pddl, run_downward_standalone
from planning.search.numeric_downward import run_numeric_downward
from planning.search.powerlifted import run_powerlifted, run_powerlifted_standalone
from util.filesystem import file_exists
from util.logging import init_logger, log_opts


_DESCRIPTION = """GOOSE planner script.
  WLF models represent value functions for heuristic search.
  GNN models represent action policies as reactive controllers.
"""

_EPILOG = """example usages:

# Plan with PDDL input and trained Blocksworld model
./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl blocksworld.model

# Plan with FDR input and trained Blocksworld model
./plan.py benchmarks/fdr-ipc23lt/blocksworld/testing/p0_01.sas blocksworld.model

# Plan with PDDL input without a trained model
./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner ff-wl

# Plan with FDR input without a trained model
./plan.py benchmarks/fdr-ipc23lt/blocksworld/testing/p0_01.sas --planner ff-wl
"""


# fmt: off
def get_planning_parser():
    parser = argparse.ArgumentParser(
        description=_DESCRIPTION,
        epilog=_EPILOG,
        formatter_class=argparse.RawTextHelpFormatter,
    )

    parser.add_argument("input1", type=str,
                        help="May be path to PDDL domain file or FDR file.")
    parser.add_argument("input2", type=str, nargs="?",
                        help="May be path to PDDL problem file or model file.")
    parser.add_argument("input3", type=str, nargs="?",
                        help="May be path to model file.")

    # Planning options
    parser.add_argument("--planner", type=Planner.parse,
                        default=Planner.NONE,
                        choices=Planner.choices(),
                        help="Underlying planner. If not specified, it is automatically detected from the model.")
    parser.add_argument("--timeout", type=int,
                        default=3600,
                        help="Timeout for search in seconds. Ignores all other preprocessing times.")
    parser.add_argument("--plan-file", type=str,
                        default="sas_plan",
                        help="Location for output solution files.")
    parser.add_argument("--intermediate-file", type=str,
                        default="intermediate.tmp",
                        help="Location for intermediate files.")

    # Policy options
    policy_group = parser.add_argument_group("policy options")
    policy_group.add_argument("--bound", type=int,
                        default=-1,
                        help="Bound for policy rollouts. If not specified or --bound=-1, then do not use bound.")
    policy_group.add_argument("--random-seed", type=int,
                        default=0,
                        help="Random seed for policy algorithms.")

    parser.add_argument("--debug", action="store_true",
                        help="Debug mode.")

    # Unsupervised WL options
    wl_group = parser.add_argument_group("WL feature options")
    wl_group.add_argument("--iterations", type=int,
                        default=4,
                        help=f"Number of iterations for WL features. " + \
                             f"(default: 4)")
    wl_group.add_argument("--graph-representation", type=str,
                        default="ilg",
                        help=f"Graph representation for WL features. " + \
                             f"(default: ilg)")

    return parser
# fmt: on


def main():
    init_logger()
    parser = get_planning_parser()
    opts = parser.parse_args()

    # Check input validity
    input1 = opts.input1
    input2 = opts.input2
    input3 = opts.input3

    for f in [input1, input2, input3]:
        if f is not None and not file_exists(f):
            raise FileNotFoundError(f)

    is_fdr_input = input1.endswith(".sas")
    is_pddl_input = input1.endswith(".pddl") and input2.endswith(".pddl")
    model_path = None
    if is_fdr_input:
        # FDR checks
        logging.info("Detected FDR input.")
        if input2 is not None and not input2.endswith(".model"):
            raise ValueError(f"Expected no 2nd argument or a .model file with FDR input. Got {input2=}")
        if input3 is not None:
            raise ValueError(f"Expected no 3rd argument with FDR input. Got {input3=}")
        model_path = input2
    elif is_pddl_input:
        # PDDL checks
        logging.info("Detected PDDL input.")
        if input3 is not None and not input3.endswith(".model"):
            raise ValueError(f"Expected no 3rd argument or a .model file with PDDL input. Got {input3=}")
        model_path = input3
    else:
        raise ValueError(f"Got unexpected combination of input arguments\n{input1=}\n{input2=}\n{input3=}")

    # Load from model path if model specified
    if model_path:
        with zipfile.ZipFile(model_path, "r") as zf:
            zf.extractall()
        params_path = f"{model_path}.params"
        opts_path = f"{model_path}.opts"
        train_opts = json.load(open(opts_path, "r"))
        train_opts = argparse.Namespace(**train_opts)
        train_opts = namespace_from_serialisable(train_opts)
    else:
        train_opts = None

    # Check planner validity and automatically detect planner if not specified but model is specified
    def set_planner(planner: Planner):
        opts.__dict__["planner"] = planner
        logging.info(f"Automatically set planner to {planner.value}")

    if opts.planner == Planner.NONE and train_opts is not None:
        if PolicyType.is_not_search(train_opts.policy_type):
            set_planner(Planner.POLICY)
        else:
            if is_fdr_input:
                set_planner(Planner.DOWNWARD_FDR)
            elif is_pddl_input:
                match train_opts.state_representation:
                    case StateRepresentation.DOWNWARD:
                        set_planner(Planner.DOWNWARD)
                    case StateRepresentation.NUMERIC_DOWNWARD:
                        set_planner(Planner.NUMERIC_DOWNWARD)
                    case StateRepresentation.ALL | StateRepresentation.NO_STATICS:
                        set_planner(Planner.POWERLIFTED)
                    case _:
                        raise ValueError(f"Unknown value {train_opts.state_representation=}")

    if is_fdr_input and not Planner.supports_fdr(opts.planner):
        raise ValueError(f"{opts.planner=} does not support FDR input.")
    if is_pddl_input and not Planner.supports_pddl(opts.planner):
        raise ValueError(f"{opts.planner=} does not support PDDL input.")

    # Log plan options
    log_opts(desc="plan", opts=opts)

    # Log train options
    if train_opts is not None:
        log_opts(desc="train", opts=train_opts)

    if Planner.standalone_downward_planner(opts.planner):
        run_downward_standalone(domain_path=input1, problem_path=input2, opts=opts)
    elif Planner.standalone_powerlifted_planner(opts.planner):
        run_powerlifted_standalone(domain_path=input1, problem_path=input2, opts=opts)
    else:
        match opts.planner:
            case Planner.DOWNWARD:
                run_downward_pddl(domain_path=input1, problem_path=input2, wlf_params_path=params_path, opts=opts)
            case Planner.DOWNWARD_FDR:
                run_downward_fdr(sas_path=input1, wlf_params_path=params_path, opts=opts)
            case Planner.NUMERIC_DOWNWARD:
                run_numeric_downward(domain_path=input1, problem_path=input2, wlf_params_path=params_path, opts=opts)
            case Planner.POWERLIFTED:
                run_powerlifted(domain_path=input1, problem_path=input2, wlf_params_path=params_path, opts=opts)
            case Planner.POLICY:
                kwargs = {
                    "domain_path": input1,
                    "problem_path": input2,
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
