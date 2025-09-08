#!/usr/bin/env python

import argparse
import json
import logging
import os
import random
import sys
import zipfile

from goose.enums.mode import Mode
from goose.enums.planner import Planner
from goose.enums.policy_type import PolicyType
from goose.enums.serialisation import namespace_from_serialisable
from goose.enums.state_representation import StateRepresentation
from goose.planning.policy.wlf_policy import WlfPolicyExecutor
from goose.planning.search.downward import run_downward_fdr, run_downward_pddl
from goose.planning.search.numeric_downward import run_numeric_downward
from goose.planning.search.powerlifted import run_powerlifted
from goose.util.logging import fmt_cmd, init_logger, log_opts


_DESCRIPTION = """GOOSE planner script"""

_EPILOG = f"""example usages:

plan with PDDL input and trained Blocksworld model
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl blocksworld.model")}

plan with FDR input and trained Blocksworld model
{fmt_cmd("./plan.py sas benchmarks/fdr-ipc23lt/blocksworld/testing/p0_01.sas blocksworld.model")}

plan with PDDL input, Downward, and novelty heuristic
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner downward '--search eager_greedy([qbpnwl(eval=ff(),g=\"ilg\",l=2,w=\"wl\")])'")}

plan with FDR input, Downward, and novelty heuristic
{fmt_cmd("./plan.py sas benchmarks/fdr-ipc23lt/blocksworld/testing/p0_01.sas --planner downward '--search eager_greedy([qbpnwl(eval=ff(),g=\"ilg\",l=2,w=\"wl\")])'")}

plan with PDDL input, Powerlifted, and novelty heuristic
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner powerlifted '-s gbfs -e qbpnwlff'")}
"""


_SAS_MAGIC = "sas"

# fmt: off
def get_planning_parser():
    parser = argparse.ArgumentParser(
        description=_DESCRIPTION,
        epilog=_EPILOG,
        formatter_class=argparse.RawTextHelpFormatter,
    )

    parser.add_argument("input1", type=str,
                        help=f"Path to `.pddl` domain file or `{_SAS_MAGIC}`.")
    parser.add_argument("input2", type=str,
                        help="Path to `.pddl` or `.sas` problem file.")
    parser.add_argument("input3", type=str,
                        help="Path to model file or standalone planner config args in commas.")

    # Planning options
    parser.add_argument("--planner", type=Planner.parse,
                        default=Planner.NONE,
                        choices=Planner.choices(),
                        help="Underlying planner config. If not specified, it is automatically detected from the model.")
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

    # General options
    parser.add_argument("--debug", action="store_true",
                        help="Debug mode.")

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

    is_fdr_input = input1 == _SAS_MAGIC
    is_pddl_input = input1.endswith(".pddl") and input2.endswith(".pddl")
    model_path = None
    if is_fdr_input:
        logging.info("Detected FDR input.")
    elif is_pddl_input:
        logging.info("Detected PDDL input.")
    else:
        raise ValueError(f"Got unexpected combination of input arguments\n{input1=}\n{input2=}\n{input3=}")

    # Load from model path if model specified
    if os.path.exists(input3):
        model_path = input3
        logging.info(f"Model detected at {model_path}.")
        with zipfile.ZipFile(model_path, "r") as zf:
            zf.extractall()
        params_path = f"{model_path}.params"
        opts_path = f"{model_path}.opts"
        train_opts = json.load(open(opts_path, "r"))
        train_opts = argparse.Namespace(**train_opts)
        train_opts = namespace_from_serialisable(train_opts)
    else:
        logging.info(f"No file exists at {input3=}. Assuming standalone planner config.")
        if not os.path.exists(input3) and opts.planner is None:
            raise ValueError(f"Expected a value for --planner")
        train_opts = None

    # Check planner validity and automatically detect planner if not specified but model is specified
    def set_planner(planner: Planner):
        opts.__dict__["planner"] = planner
        logging.info(f"Automatically set planner to {planner.value}")

    if opts.planner == Planner.NONE and train_opts is not None:
        if PolicyType.is_not_search(train_opts.policy_type):
            set_planner(Planner.POLICY)
        elif is_fdr_input:
            set_planner(Planner.DOWNWARD)
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
        else:
            raise ValueError(f"Unknown input type with {input1=} and {input2=}. Cannot automatically set planner.")

    if is_fdr_input and not Planner.supports_fdr(opts.planner):
        raise ValueError(f"{opts.planner=} does not support FDR input.")
    if is_pddl_input and not Planner.supports_pddl(opts.planner):
        raise ValueError(f"{opts.planner=} does not support PDDL input.")

    # Log plan options
    log_opts(desc="plan", opts=opts)

    # Log train options
    if train_opts is not None:
        log_opts(desc="train", opts=train_opts)

    # Parse additional planning configs
    if model_path is None:
        config = input3.split(" ")
    elif opts.planner == Planner.DOWNWARD:
        config = ["--search", f'eager_greedy([wlgoose(model_file="{params_path}")])']
    elif opts.planner == Planner.NUMERIC_DOWNWARD:
        config = [
            "--search",
            f"eager_greedy(wlgoose(model_path={params_path},domain_path={input1},problem_path={input2}))",
        ]
    elif opts.planner == Planner.POWERLIFTED:
        config = ["-s", "gbfs", "-e", "wlgoose", "-m", params_path]
    elif opts.planner == Planner.POLICY:
        pass
    else:
        raise ValueError()

    match opts.planner:
        case Planner.DOWNWARD:
            if is_fdr_input:
                run_downward_fdr(sas_path=input2, config=config, opts=opts)
            else:
                run_downward_pddl(domain_path=input1, problem_path=input2, config=config, opts=opts)
        case Planner.NUMERIC_DOWNWARD:
            run_numeric_downward(domain_path=input1, problem_path=input2, config=config, opts=opts)
        case Planner.POWERLIFTED:
            run_powerlifted(domain_path=input1, problem_path=input2, config=config, opts=opts)
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
                    from goose.planning.policy.gnn_policy import GnnPolicyExecutor

                    policy = GnnPolicyExecutor(**kwargs)
                case _:
                    raise ValueError(f"Unknown value {train_opts.mode=}")

            random.seed(opts.random_seed)
            plan = policy.execute()
            policy.dump_stats()
            if plan is not None:
                with open(opts.plan_file, "w") as f:
                    f.write("\n".join(plan))
        case _:
            raise ValueError(f"Unknown value {opts.planner=}")

    if os.path.exists(opts.intermediate_file):
        os.remove(opts.intermediate_file)


if __name__ == "__main__":
    main()
