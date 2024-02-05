""" Main training pipeline script. """

import argparse
import numpy as np
import warnings
from dataset.factory import get_states_from_state_spaces
from models.save_load import print_arguments
from models.wlf.model import BAYESIAN_MODELS, FREQUENTIST_MODELS

warnings.filterwarnings("ignore")


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("domain_pddl")
    parser.add_argument("tasks_dir")
    parser.add_argument("plans_dir")

    # ml model arguments
    parser.add_argument(
        "-m",
        "--model",
        type=str,
        default="linear-svr",
        choices=FREQUENTIST_MODELS + BAYESIAN_MODELS,
        help="ML model",
    )
    parser.add_argument("--seed", type=int, default=0, help="random seed")

    args = parser.parse_args()

    return args


def main():
    args = parse_args()
    print_arguments(args)
    np.random.seed(args.seed)

    # load dataset
    states = get_states_from_state_spaces(args.domain_pddl, args.tasks_dir, args.plans_dir)

    breakpoint()
    



if __name__ == "__main__":
    main()
