import argparse
import logging
import os
import random
import sys
from typing import Any, Dict

import numpy as np
import termcolor as tc
import toml

from learning.dataset import get_domain_file_from_opts, get_training_dir_from_opts
from learning.predictor.linear_model.predictor_factory import get_available_predictors
from learning.predictor.neural_network.policy_type import get_policy_type_options
from util.error_message import get_path_error_msg
from util.logging import mat_to_str
from util.paths import DATA_CACHE_DIR
from wlplan.feature_generator import get_available_feature_algorithms, get_available_pruning_methods


_DESCRIPTION = """GOOSE trainer script.
  WLF models are primarily used to learn value functions for heuristic search.
  GNN models are used to learn action policies as reactive controllers.
"""

_EPILOG = """example usages:

# Train and save a classical Blocksworld model
python3 train.py configurations/data/ipc23lt/blocksworld.toml configurations/model/classic.toml -s blocksworld.model

# Train and save a numeric Childsnack model
python3 train.py configurations/data/neurips24/childsnack.toml configurations/model/numeric.toml -s numeric_childsnack.model

# Run a distinguishability test
python3 train.py configurations/data/ipc23lt/blocksworld.toml --distinguish-test

# Save a PCA visualisation of features to file
python3 train.py configurations/data/ipc23lt/blocksworld.toml --visualise-pca blocksworld_pca.png
"""

_DEFAULT_WLF_VALS = {
    "features": "wl",
    "optimisation": "rank-svm",
    "feature_pruning": "i-mf",
    "hash": "set",
}

_DEFAULT_GNN_VALS = {
    "policy_type": "v",
    "num_hidden": 64,
    "learning_rate": 0.001,
    "patience": 10,
    "reduction": 0.1,
    "batch_size": 16,
    "epochs": 1000,
}


# fmt: off
def get_parser():
    parser = argparse.ArgumentParser(
        description=_DESCRIPTION,
        epilog=_EPILOG,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    # Config options
    parser.add_argument("domain_directory", type=str,
                        help=f"Path to domain directory. The directory must contain a `domain.pddl` domain file and a `training/` directory with `*.pddl` problem files. Optionally, the directory may also contain `training_plans/` directory with `*.plan` plan files corresponding to the problem files.")
    parser.add_argument("model_config", type=str, nargs='?', default=None,
                        help=f"Path to .toml model configuration file.\n" + \
                             f"If not provided, default model values are used. " + \
                             f"Model configuration file overrides command line and default values. ")

    # General model options
    gen_group = parser.add_argument_group("general model options")
    gen_group.add_argument("-g", "--graph-representation", type=str, default="ilg",
                        help=f"Feature generator to use. " + \
                             f"(default: ilg).")
    gen_group.add_argument("-l", "--iterations", type=int, default=2,
                        help=f"Number of WL iterations or GNN message passing layers. " + \
                             f"(default: 2)")
    gen_group.add_argument("-m", "--mode", type=str, default="wlf",
                        choices=["wlf", "gnn"],
                        help=f"Mode to use. " + \
                             f"(default: wlf).")

    # WLF options
    wlf_group = parser.add_argument_group("wlf options")
    wlf_group.add_argument("-f", "--features", type=str, default=None,
                        choices=get_available_feature_algorithms(),
                        help=f"Feature generator to use. " + \
                            f"(default: {_DEFAULT_WLF_VALS['features']}).")
    wlf_group.add_argument("-fp", "--feature-pruning", type=str, default=None,
                        choices=get_available_pruning_methods(),
                        help=f"Pruning method to use for feature generation. " + \
                             f"(default: {_DEFAULT_WLF_VALS['feature_pruning']}).")
    wlf_group.add_argument("--hash", type=str, default=None,
                        choices=["mset", "set"],
                        help=f"Whether to use multisets or sets for neighbour colours. " + \
                             f"(default: {_DEFAULT_WLF_VALS['hash']})")
    wlf_group.add_argument("-o", "--optimisation", type=str, default=None,
                        choices=get_available_predictors(),
                        help=f"Optimisation algorithm to use for prediction. " + \
                             f"(default: {_DEFAULT_WLF_VALS['optimisation']}).")

    # GNN options
    gnn_group = parser.add_argument_group("gnn options")
    gnn_group.add_argument("--policy-type", type=str,
                        choices=get_policy_type_options(),
                        help=f"If specified, GNN policy representation: *v*-function, *q*-function, or distributional *p*. " + \
                             f"(default: {_DEFAULT_GNN_VALS['policy_type']})")
    gnn_group.add_argument("--num-hidden", type=int, default=None,
                        help=f"Hidden GNN dimension. " + \
                             f"(default: {_DEFAULT_GNN_VALS['num_hidden']})")
    gnn_group.add_argument("--learning-rate", type=float, default=None,
                        help=f"Learning rate for Adam. " + \
                             f"(default: {_DEFAULT_GNN_VALS['learning_rate']})")
    gnn_group.add_argument("--patience", type=int, default=None,
                        help=f"Patience for learning rate scheduler. " + \
                             f"(default: {_DEFAULT_GNN_VALS['patience']})")
    gnn_group.add_argument("--reduction", type=float, default=None,
                        help=f"Reduction factor for learning rate scheduler. " + \
                            f"(default: {_DEFAULT_GNN_VALS['reduction']})")
    gnn_group.add_argument("--batch-size", type=int, default=None,
                        help=f"Batch size for training. " + \
                             f"(default: {_DEFAULT_GNN_VALS['batch_size']})")
    gnn_group.add_argument("--epochs", type=int, default=None,
                        help=f"Maximum number of epochs to train for. " + \
                             f"(default: {_DEFAULT_GNN_VALS['epochs']})")

    # Data options
    data_group = parser.add_argument_group("data options")
    data_group.add_argument("--no-cache", dest="cache", action="store_false",
                        help=f"Do not cache processed data no use cached data.")
    data_group.add_argument("--clear-cache", action="store_true",
                        help=f"Clear cache directory.")
    data_group.add_argument("-nd", "--num-data", type=int, default=None,
                        help=f"Number of training data to use. " + \
                             f"(default: None = all available data)")
    data_group.add_argument("-dg", "--data-generation", type=str, default="plan",
                        choices=["plan", "state-space"],
                        help=f"Method for collecting data from training problems. " + \
                             f"(default: plan)")
    data_group.add_argument("-dp", "--data-pruning", type=str, default="equivalent-weighted",
                        choices=["none", "equivalent", "equivalent-weighted"],
                        help=f"Method for pruning data. " + \
                             f"(default: equivalent-weighted)")
    data_group.add_argument("--facts", type=str, default="fd",
                        choices=["fd", "nfd", "all", "nostatic"],
                        help=f"Intended facts to keep e.g. Fast Downward *fd* grounds the task and prunes away statics as well as some unreachable facts. " + \
                             f"(default: fd)")

    # Experiment options
    parser.add_argument("-r", "--random-seed", type=int, default=2024,
                        help=f"Random seed for nondeterministic training algorithms.")
    parser.add_argument("-s", "--save-file", type=str, default=None,
                        help=f"Path to save the model to.")
    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument("--visualise-pca", type=str, default=None,
                        help=f"Path to save visualisation of PCA on WL features, for example pca.png.")
    mutex_group.add_argument("--distinguish-test", action="store_true",
                        help=f"Run distinguishability test.")
    mutex_group.add_argument("--collect-only", action="store_true",
                        help=f"Only collect features and exit.")
    return parser
# fmt: on


def parse_opts():
    parser = get_parser()
    opts = parser.parse_args()

    # Perform trivial tasks
    if opts.clear_cache:
        if os.path.exists(DATA_CACHE_DIR):
            os.system(f"rm -r {DATA_CACHE_DIR}")
        logging.info(f"Cleared cache directory {DATA_CACHE_DIR}.")

    # Check domain directory is valid
    domain_directory = opts.domain_directory
    if not os.path.exists(domain_directory):
        raise ValueError(f"{domain_directory=} does not exist.")
    if not os.path.isdir(domain_directory):
        raise ValueError(f"{domain_directory=} is not a directory.")
    domain_path = get_domain_file_from_opts(opts)
    if not os.path.exists(domain_path):
        raise ValueError(f"A `domain.pddl` file is not found in {domain_directory=}.")
    training_dir = get_training_dir_from_opts(opts)
    if not os.path.exists(training_dir):
        raise ValueError(f"A `training/` directory is not found in {domain_directory=}.")

    # Modify options based on parsed configuration
    if opts.model_config is not None:
        assert os.path.exists(opts.model_config), get_path_error_msg(opts.model_config)
        model_config = toml.load(opts.model_config)
    else:
        model_config = {}

    if "mode" in model_config:
        opts.__dict__["mode"] = model_config["mode"]

    def handle_config_vals(relevant_args: Dict[str, Any], irrelevant_args: Dict[str, Any]) -> None:
        for key, default_value in relevant_args.items():
            if opts.__dict__[key] is None:
                opts.__dict__[key] = default_value
        for key in irrelevant_args:
            opts.__dict__[key] = None
            if key in model_config:
                raise ValueError(f"Contradictory argument *{key}* in config file for {opts.mode=}")

    match opts.mode:
        case "wlf":
            logging.info(f"WLF mode detected. Ignoring all GNN options.")
            handle_config_vals(relevant_args=_DEFAULT_WLF_VALS, irrelevant_args=_DEFAULT_GNN_VALS)
        case "gnn":
            logging.info(f"GNN mode detected. Ignoring all WLF options.")
            handle_config_vals(relevant_args=_DEFAULT_GNN_VALS, irrelevant_args=_DEFAULT_WLF_VALS)
        case _:
            raise ValueError(f"Unknown value {opts.mode=}")

    for key, val in model_config.items():
        if key not in opts.__dict__:
            raise ValueError(f"Unknown argument *{key}* in config file")
        opts.__dict__[key] = model_config[key]
        logging.info(f"Setting option specified in config file {key}: {val}")

    # Modify options based on distinguish or visualisation routine
    if opts.distinguish_test or opts.visualise_pca:
        if opts.mode == "gnn":
            raise ValueError("Distinguishability testing and PCA visualisation are only supported for WLF mode.")
        logging.info("Overriding options to use WLF and regression labels for non-training routine.")
        opts.__dict__["optimisation"] = "svr"

    # Log parsed options
    logging.info(f"Processed options:\n{mat_to_str([[k, tc.colored(v, 'cyan')] for k, v in vars(opts).items()])}")

    # Set seeds
    random.seed(opts.random_seed)
    np.random.seed(opts.random_seed)

    return opts
