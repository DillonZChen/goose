#!/usr/bin/env python

import argparse
import logging
import os
import random

import numpy as np
import toml
from sklearn.metrics import f1_score

from learning.dataset.dataset_factory import get_dataset
from learning.predictor.predictor_factory import get_predictor
from util.error_message import get_path_error_msg
from util.logging import init_logger
from util.timer import TimerContextManager
from wlplan.feature_generation import WLFeatures
from wlplan.planning import parse_domain


def parse_opts():
    # fmt: off
    parser = argparse.ArgumentParser()
    parser.add_argument("data_config", type=str, 
                        help="Path to .toml data configuration file")
    parser.add_argument("model_config", type=str, 
                        help="Path to .toml model configuration file")
    parser.add_argument("-f", "--facts", type=str, default="fd", choices=["fd", "all", "nostatic"],
                        help="Intended facts to keep e.g. Fast Downward `fd` grounds the task and prunes away some facts.")
    parser.add_argument("-r", "--random_seed", type=int, default=2024,
                        help="Random seed for nondeterministic training algorithms.")
    parser.add_argument("-d", "--max_data", type=int, default=10000,
                        help="Maximum number of data points to use.")
    parser.add_argument("-s", "--save_file", type=str, default=None,
                        help="Path to save the model to.")
    opts = parser.parse_args()
    # fmt: on

    assert os.path.exists(opts.data_config), get_path_error_msg(opts.data_config)
    assert os.path.exists(opts.model_config), get_path_error_msg(opts.model_config)

    model_config = toml.load(opts.model_config)
    opts.optimisation = model_config["optimisation"]
    opts.iterations = model_config["iterations"]
    opts.rank = model_config["rank"]
    opts.data_generation = model_config["data_generation"]

    random.seed(opts.random_seed)
    np.random.seed(opts.random_seed)

    return opts


def main():
    init_logger()
    opts = parse_opts()

    # Parse dataset
    with TimerContextManager("parsing training data"):
        domain_pddl = toml.load(opts.data_config)["domain_pddl"]
        domain = parse_domain(domain_pddl)
        feature_generator = WLFeatures(
            graph_representation="ilg",
            domain=domain,
            iterations=opts.iterations,
            multiset_hash=True,
        )
        dataset = get_dataset(opts, feature_generator)

    # Collect colours
    with TimerContextManager("collecting colours"):
        feature_generator.collect(dataset.wlplan_dataset)
    logging.info(f"n_seen_graphs={feature_generator.get_n_seen_graphs()}")
    logging.info(f"n_seen_nodes={feature_generator.get_n_seen_nodes()}")
    logging.info(f"n_seen_edges={feature_generator.get_n_seen_edges()}")
    logging.info(f"n_seen_initial_colours={feature_generator.get_n_seen_initial_colours()}")
    logging.info(f"n_seen_refined_colours={feature_generator.get_n_seen_refined_colours()}")

    # Construct features
    with TimerContextManager("constructing features"):
        X = feature_generator.embed(dataset.wlplan_dataset)
        X = np.array(X).astype(float)
        y = dataset.y
    logging.info(f"{X.shape=}")

    # Train model
    with TimerContextManager("training model"):
        predictor = get_predictor(opts.optimisation)
        predictor.fit(X, y)
        feature_generator.set_weights(predictor.get_weights())

    # Evaluate model
    if not opts.rank:
        y_pred = predictor.predict(X)
        mse_loss = np.mean((y - y_pred) ** 2)
        logging.info(f"{mse_loss=}")
        y_pred = np.round(y_pred)
        f1_macro = f1_score(y, y_pred, average="macro")
        logging.info(f"{f1_macro=}")

    # Save model
    if opts.save_file:
        with TimerContextManager("saving model"):
            feature_generator.save(opts.save_file)


if __name__ == "__main__":
    main()
