#!/usr/bin/env python

import argparse
import logging
import os
import random

import numpy as np
import termcolor as tc
import toml
from sklearn.metrics import f1_score

from learning.dataset.dataset_factory import get_dataset
from learning.dataset.state_to_vec import embed_data
from learning.predictor.predictor_factory import (get_available_predictors, get_predictor,
                                                  is_rank_predictor)
from util.distinguish_test import distinguish
from util.error_message import get_path_error_msg
from util.logging import init_logger
from util.pca_visualise import visualise
from util.statistics import log_quartiles
from util.timer import TimerContextManager
from wlplan.feature_generation import (get_available_feature_generators,
                                       get_available_pruning_methods, get_feature_generator)
from wlplan.planning import parse_domain

_DEF_VAL = {
    "features": "wl",
    "iterations": 4,
    "optimisation": "svr",
    "data_generation": "plan",
    "feature_pruning": "none",
    "data_pruning": "equivalent-weighted",
    "facts": "fd",
    "multiset_hash": 1,
}


def get_parser():
    # fmt: off
    parser = argparse.ArgumentParser()

    # shortcut options
    parser.add_argument("data_config", type=str, 
                        help=f"Path to .toml data configuration file")
    parser.add_argument("-mc", "--model_config", type=str, default=None,
                        help=f"Path to .toml model configuration file. If not provided, default model values are used. " + \
                             f"However, any command line arguments override any values specified in the model configuration file. ")
    
    # model options
    parser.add_argument("-f", "--features", type=str, default=None, 
                        choices=get_available_feature_generators(),
                        help=f"Feature generator to use. " + \
                             f"(default: {_DEF_VAL['features']}).")
    parser.add_argument("-fp", "--feature_pruning", type=str, default=None,
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
    parser.add_argument("-d", "--data_generation", type=str, default=None,
                        choices=["plan", "state-space"],
                        help=f"Method for collecting data from training problems. " + \
                             f"(default: {_DEF_VAL['data_generation']})")
    parser.add_argument("-dp", "--data_pruning", type=str, default=None,
                        choices=["none", "equivalent", "equivalent-weighted"],
                        help=f"Method for pruning data. " + \
                             f"(default: {_DEF_VAL['data_pruning']})")
    parser.add_argument("--facts", type=str, default=None, 
                        choices=["fd", "nfd", "all", "nostatic"],
                        help=f"Intended facts to keep e.g. Fast Downward `fd` grounds the task and prunes away statics as well as some unreachable facts. " + \
                             f"(default: {_DEF_VAL['facts']})")
    
    # script options
    parser.add_argument("-r", "--random_seed", type=int, default=2024,
                        help=f"Random seed for nondeterministic training algorithms.")
    parser.add_argument("-s", "--save_file", type=str, default=None,
                        help=f"Path to save the model to.")
    parser.add_argument("--visualise_pca", type=str, default=None,
                        help=f"Path to save visualisation of PCA on WL features.")
    parser.add_argument("--distinguish_test", action="store_true",
                        help=f"Run distinguishability test.")
    parser.add_argument("--collect_only", action="store_true",
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


def train(opts):
    opts.rank = is_rank_predictor(opts.optimisation)
    
    # Parse dataset
    with TimerContextManager("parsing training data"):
        domain_pddl = toml.load(opts.data_config)["domain_pddl"]
        domain = parse_domain(domain_pddl)
        features = opts.features
        feature_generator = get_feature_generator(
            features,
            domain,
            iterations=opts.iterations,
            pruning=opts.feature_pruning,
            multiset_hash=opts.multiset_hash,
        )
        # feature_generator.print_init_colours()
        dataset = get_dataset(opts, feature_generator)

    # Collect colours
    with TimerContextManager("collecting colours"):
        feature_generator.collect(dataset.wlplan_dataset)
    logging.info(f"n_colours_per_layer:")
    for i, n_colours in enumerate(feature_generator.get_layer_to_n_colours()):
        logging.info(f"  {i}={n_colours}")
    if opts.collect_only:
        logging.info("Exiting after collecting colours.")
        exit(0)

    # Construct features
    with TimerContextManager("constructing features"):
        X, y, sample_weight = embed_data(
            dataset=dataset, feature_generator=feature_generator, opts=opts
        )
    if not opts.rank:
        log_quartiles(y)
    logging.info(f"{X.shape=}")

    # distinct_per_column_counts = {}
    # for column in X.T:
    #     column = set(column)
    #     size = len(column)
    #     if size not in distinct_per_column_counts:
    #         distinct_per_column_counts[size] = 0
    #     distinct_per_column_counts[size] += 1
    # for k in sorted(distinct_per_column_counts.keys()):
    #     print(k, distinct_per_column_counts[k])
    
    # colour_counts = {}
    # for column in X.T:
    #     summ = sum(column)
    #     if summ not in colour_counts:
    #         colour_counts[summ] = 0
    #     colour_counts[summ] += 1
    # for k in sorted(colour_counts.keys()):
    #     print(k, colour_counts[k])
    
    # breakpoint()

    # PCA visualisation
    pca_save_file = opts.visualise_pca
    if pca_save_file is not None:
        visualise(X, y, save_file=pca_save_file)
        return

    # distinguishability testing
    if opts.distinguish_test:
        distinguish(X, y)
        return

    # Train model
    predictor = get_predictor(opts.optimisation)
    predictor.fit_evaluate(X, y, sample_weight=sample_weight)

    # Save model
    if opts.save_file:
        with TimerContextManager("saving model"):
            feature_generator.set_weights(predictor.get_weights())
            feature_generator.save(opts.save_file)


if __name__ == "__main__":
    init_logger()
    opts = parse_opts()
    train(opts)
