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
from learning.options import parse_opts
from learning.predictor.predictor_factory import (
    get_available_predictors,
    get_predictor,
    is_rank_predictor
)
from util.distinguish_test import distinguish
from util.error_message import get_path_error_msg
from util.logging import init_logger
from util.pca_visualise import visualise
from util.statistics import log_quartiles
from util.timer import TimerContextManager
from wlplan.feature_generation import get_feature_generator
from wlplan.planning import parse_domain


def train(opts):
    opts.rank = is_rank_predictor(opts.optimisation)

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
    logging.info(f"n_colours_per_layer:")
    for i, n_colours in enumerate(feature_generator.get_layer_to_n_colours()):
        logging.info(f"  {i}={n_colours}")
    if opts.collect_only:
        logging.info("Exiting after collecting colours.")
        exit(0)

    # Construct features
    with TimerContextManager("constructing features"):
        X, y, sample_weight = embed_data(dataset=dataset, feature_generator=feature_generator, opts=opts)
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
