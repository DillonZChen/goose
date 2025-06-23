import argparse
import logging
import os
import random

import numpy as np
import termcolor as tc
import toml

from learning.predictor.predictor_factory import get_available_predictors
from util.error_message import get_path_error_msg
from wlplan.feature_generation import (
    get_available_feature_generators,
    get_available_pruning_methods,
)

_DEF_VAL = {
    "features": "wl",
    "graph_representation": "ilg",
    "iterations": 2,
    "optimisation": "rank-lp",
    "data_generation": "plan",
    "feature_pruning": "none",
    "data_pruning": "equivalent-weighted",
    "facts": "fd",
    "multiset_hash": 1,
}

_DESCRIPTION = """GOOSE trainer script."""

_EPILOG = """example usages:

// Train and save a classical Blocksworld model
python3 train.py configurations/data/ipc23lt/blocksworld.toml configurations/model/classic.toml -s blocksworld.model

// Train and save a numeric Childsnack model
python3 train.py configurations/data/neurips24/childsnack.toml configurations/model/numeric.toml -s numeric_childsnack.model

// Run a distinguishability test
python3 train.py configurations/data/ipc23lt/blocksworld.toml --distinguish-test

// Save a PCA visualisation of features to file
python3 train.py configurations/data/ipc23lt/blocksworld.toml --visualise-pca blocksworld_pca.png
"""


def get_parser():
    # fmt: off
    parser = argparse.ArgumentParser(description=_DESCRIPTION, epilog=_EPILOG, formatter_class=argparse.RawDescriptionHelpFormatter)

    # shortcut options
    parser.add_argument("data_config", type=str, 
                        help=f"Path to .toml data configuration file")
    parser.add_argument("model_config", type=str, nargs='?', default=None, 
                        help=f"Path to .toml model configuration file. If not provided, default model values are used. " + \
                             f"However, any command line arguments override any values specified in the model configuration file. ")
    
    # model options
    parser.add_argument("-f", "--features", type=str, default=None, 
                        choices=get_available_feature_generators(),
                        help=f"Feature generator to use. " + \
                             f"(default: {_DEF_VAL['features']}).")
    parser.add_argument("-g", "--graph-representation", type=str, default=None, 
                        help=f"Feature generator to use. " + \
                             f"(default: {_DEF_VAL['graph_representation']}).")
    parser.add_argument("-fp", "--feature-pruning", type=str, default=None,
                        choices=get_available_pruning_methods(),
                        help=f"Pruning method to use for feature generation. " + \
                             f"(default: {_DEF_VAL['feature_pruning']}).")
    parser.add_argument("-L", "--iterations", type=int, default=None,
                        help=f"Number of iterations of the WL feature generator. Analogous to number of hidden layers in a neural network. " + \
                             f"(default: {_DEF_VAL['iterations']})")
    parser.add_argument("--multiset_hash", type=int, default=None,
                        help=f"Whether to use multisets or sets for neighbour colours. " + \
                             f"(default: {_DEF_VAL['multiset_hash']})")
    
    # optimisation options
    parser.add_argument("-o", "--optimisation", type=str, default=None,
                        choices=get_available_predictors(),
                        help=f"Optimisation algorithm to use for prediction. " + \
                             f"(default: {_DEF_VAL['optimisation']}).")
    parser.add_argument("-d", "--data-generation", type=str, default=None,
                        choices=["plan", "state-space"],
                        help=f"Method for collecting data from training problems. " + \
                             f"(default: {_DEF_VAL['data_generation']})")
    parser.add_argument("-dp", "--data-pruning", type=str, default=None,
                        choices=["none", "equivalent", "equivalent-weighted"],
                        help=f"Method for pruning data. " + \
                             f"(default: {_DEF_VAL['data_pruning']})")
    parser.add_argument("--facts", type=str, default=None, 
                        choices=["fd", "nfd", "all", "nostatic"],
                        help=f"Intended facts to keep e.g. Fast Downward `fd` grounds the task and prunes away statics as well as some unreachable facts. " + \
                             f"(default: {_DEF_VAL['facts']})")
    
    # script options
    parser.add_argument("-r", "--random-seed", type=int, default=2024,
                        help=f"Random seed for nondeterministic training algorithms.")
    parser.add_argument("-s", "--save-file", type=str, default=None,
                        help=f"Path to save the model to.")
    parser.add_argument("--visualise-pca", type=str, default=None,
                        help=f"Path to save visualisation of PCA on WL features.")
    parser.add_argument("--distinguish-test", action="store_true",
                        help=f"Run distinguishability test.")
    parser.add_argument("--collect-only", action="store_true",
                        help=f"Only collect features and exit.")
    # fmt: on
    return parser


def parse_opts():
    parser = get_parser()
    opts = parser.parse_args()

    assert os.path.exists(opts.data_config), get_path_error_msg(opts.data_config)
    logging.info(f"{opts.data_config=}")

    if opts.model_config is not None:
        assert os.path.exists(opts.model_config), get_path_error_msg(opts.model_config)
        model_config = toml.load(opts.model_config)
    else:
        model_config = {}
            
    if opts.distinguish_test:
        logging.info("Overriding optimisation to `svr` to use regression labels.")
        model_config["optimisation"] = "svr"

    for key, default_value in _DEF_VAL.items():
        config_value = model_config.get(key, None)
        parsed_value = getattr(opts, key)

        def get_msg(desc, value):
            return f"Setting {desc} value {key}: " + tc.colored(value, "cyan")

        if parsed_value is None and config_value is not None:
            logging.info(get_msg("config", config_value))
            opts.__dict__[key] = config_value
        elif parsed_value is None and config_value is None:
            logging.info(get_msg("default", default_value))
            opts.__dict__[key] = default_value
        elif parsed_value is not None:
            msg = get_msg("parsed", parsed_value)
            if config_value is not None:
                msg += f" (overriding config value {config_value})"
            logging.info(msg)

    random.seed(opts.random_seed)
    np.random.seed(opts.random_seed)

    return opts
