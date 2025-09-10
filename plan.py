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
from goose.planning.search.downward import run_downward
from goose.planning.search.numeric_downward import run_numeric_downward
from goose.planning.search.powerlifted import run_powerlifted
from goose.util.logging import fmt_cmd, init_logger, log_opts


_DESCRIPTION = """GOOSE planner script"""

_EPILOG = f"""example usages:

plan with trained Blocksworld model
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --model blocksworld.model")}

plan with FDR input and trained Blocksworld model
{fmt_cmd("./plan.py benchmarks/fdr-ipc23lt/blocksworld/testing/p0_01.sas --model blocksworld.model")}

plan with QB(at;wl) heuristic in Downward
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner downward --config '--search eager_greedy([qbatwl(eval=ff())])'")}

plan with PN(at;wl) heuristic in Downward
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner downward --config '--search eager_greedy([pnatwl(width=2,evals=[ff()])])'")}

plan with QB(at;wl) heuristic in Powerlifted
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner powerlifted --config '-s gbfs -e qbatwlff'")}

plan with LAMA
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner lama-first")}

plan with NOLAN
{fmt_cmd("./plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p0_01.pddl --planner nolan")}
"""


# fmt: off
def get_planning_parser():
    parser = argparse.ArgumentParser(
        description=_DESCRIPTION,
        epilog=_EPILOG,
        formatter_class=argparse.RawTextHelpFormatter,
    )

    parser.add_argument("input1", type=str,
                        help=f"Path to PDDL domain file or SAS problem file.")
    parser.add_argument("input2", type=str, nargs="?",
                        help=f"Path to PDDL problem file or nothing if using SAS input.")

    cgroup = parser.add_mutually_exclusive_group()
    cgroup.add_argument("-m", "--model", type=str,
                        help="Path to model file.")
    cgroup.add_argument("-c", "--config", type=str,
                        help="Standalone planner config args specified in quotes.")

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
    model_path = opts.model
    config = opts.config

    is_pddl_input = input2 is not None
    if is_pddl_input:
        logging.info("Detected PDDL input.")
    else:
        logging.info("Detected FDR input.")

    # Load from model path if model specified
    train_opts = None
    if model_path is not None:
        if not os.path.exists(model_path):
            raise ValueError(f"Model file does not exist at {model_path}")
        with zipfile.ZipFile(model_path, "r") as zf:
            zf.extractall()
        params_path = f"{model_path}.params"
        opts_path = f"{model_path}.opts"
        train_opts = json.load(open(opts_path, "r"))
        train_opts = argparse.Namespace(**train_opts)
        train_opts = namespace_from_serialisable(train_opts)
    elif config is not None:
        if not os.path.exists(config) and opts.planner is None:
            raise ValueError(f"--config specified but no value specified for --planner")

    # Check planner validity and automatically detect planner if not specified but model is specified
    def set_planner(planner: Planner):
        opts.__dict__["planner"] = planner
        logging.info(f"Automatically set planner to {planner.value}")

    if opts.planner == Planner.NONE and train_opts is not None:
        if PolicyType.is_not_search(train_opts.policy_type):
            set_planner(Planner.POLICY)
        else:
            match train_opts.state_representation:
                case StateRepresentation.DOWNWARD:
                    set_planner(Planner.DOWNWARD)
                case StateRepresentation.NUMERIC_DOWNWARD:
                    set_planner(Planner.NUMERIC_DOWNWARD)
                case StateRepresentation.ALL | StateRepresentation.NO_STATICS:
                    set_planner(Planner.POWERLIFTED)
                case _:
                    raise ValueError(f"Unknown value {train_opts.state_representation=}")

    if not is_pddl_input and not Planner.supports_fdr(opts.planner):
        raise ValueError(f"{opts.planner=} does not support FDR input.")
    if is_pddl_input and not Planner.supports_pddl(opts.planner):
        raise ValueError(f"{opts.planner=} does not support PDDL input.")

    # Log plan options
    log_opts(desc="plan", opts=opts)

    # Log train options
    if train_opts is not None:
        log_opts(desc="train", opts=train_opts)

    # Parse additional planning configs
    if config is not None:
        config = config.split()
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
        config = []

    if Planner.is_downward_alias(opts.planner):
        run_downward(input1=input1, input2=input2, config=config, alias=opts.planner, opts=opts)
    else:
        match opts.planner:
            case Planner.DOWNWARD:
                run_downward(input1=input1, input2=input2, config=config, opts=opts)
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
