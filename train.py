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
from util.distinguish_test import distinguish
from util.error_message import get_path_error_msg
from util.logging import init_logger
from util.pca_visualise import visualise
from util.statistics import log_quartiles
from util.timer import TimerContextManager
from wlplan.feature_generation import get_feature_generator
from wlplan.planning import parse_domain


def parse_opts():
    # fmt: off
    parser = argparse.ArgumentParser()
    parser.add_argument("data_config", type=str, 
                        help="Path to .toml data configuration file")
    parser.add_argument("model_config", type=str, 
                        help="Path to .toml model configuration file")
    parser.add_argument("-f", "--facts", type=str, default="fd", choices=["fd", "nfd", "all", "nostatic"],
                        help="Intended facts to keep e.g. Fast Downward `fd` grounds the task and prunes away some facts.")
    parser.add_argument("-r", "--random_seed", type=int, default=2024,
                        help="Random seed for nondeterministic training algorithms.")
    parser.add_argument("-s", "--save_file", type=str, default=None,
                        help="Path to save the model to.")
    parser.add_argument("--visualise_pca", type=str, default=None,
                        help="Path to save visualisation of PCA on WL features.")
    parser.add_argument("--distinguish_test", action="store_true",
                        help="Run distinguishability test.")
    opts = parser.parse_args()
    # fmt: on

    assert os.path.exists(opts.data_config), get_path_error_msg(opts.data_config)
    assert os.path.exists(opts.model_config), get_path_error_msg(opts.model_config)

    model_config = toml.load(opts.model_config)
    opts.features = model_config["features"]
    opts.graph_representation = model_config["graph_representation"]
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
        features = opts.features
        graph_representation = opts.graph_representation
        logging.info(f"{features=}")
        logging.info(f"{graph_representation=}")
        feature_generator = get_feature_generator(
            feature_algorithm=features,
            graph_representation=graph_representation,
            domain=domain,
            iterations=opts.iterations,
        )
        feature_generator.print_init_colours()
        dataset = get_dataset(opts, feature_generator)
        logging.info(f"{len(dataset)=}")

    # Collect colours
    with TimerContextManager("collecting colours"):
        feature_generator.collect(dataset.wlplan_dataset)
    # logging.info(f"n_seen_graphs={feature_generator.get_n_seen_graphs()}")
    # logging.info(f"n_seen_nodes={feature_generator.get_n_seen_nodes()}")
    # logging.info(f"n_seen_edges={feature_generator.get_n_seen_edges()}")
    # logging.info(f"n_seen_initial_colours={feature_generator.get_n_seen_initial_colours()}")
    # logging.info(f"n_seen_refined_colours={feature_generator.get_n_seen_refined_colours()}")

    # Construct features
    with TimerContextManager("constructing features"):
        X = feature_generator.embed(dataset.wlplan_dataset)
        X = np.array(X).astype(float)
        y = dataset.y
        if not opts.rank:
            log_quartiles(y)
    logging.info(f"{X.shape=}")

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
