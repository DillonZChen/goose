#!/usr/bin/env python

import argparse
import logging

import toml

from learning.dataset.dataset_factory import get_dataset
from learning.dataset.pyg import get_data_loaders
from learning.dataset.state_to_vec import embed_data
from learning.options import parse_opts
from learning.predictor.linear_model.predictor_factory import get_predictor
from util.distinguish_test import distinguish
from util.logging import init_logger
from util.pca_visualise import visualise
from util.timer import TimerContextManager
from wlplan.feature_generator import init_feature_generator
from wlplan.graph_generator import init_graph_generator
from wlplan.planning import Domain, parse_domain


def _domain_from_opts(opts: argparse.Namespace) -> Domain:
    return parse_domain(toml.load(opts.data_config)["domain_pddl"])


def train_wlf(opts: argparse.Namespace) -> None:
    """Trains a linear model using WLF features for learning value functions.

    Args:
        opts (argparse.Namespace): parsed arguments
    """

    # Parse dataset
    with TimerContextManager("parsing training data"):
        dataset = get_dataset(opts)
        logging.info(f"{len(dataset)=}")

    # Collect colours
    wlf_generator = init_feature_generator(
        feature_algorithm=opts.features,
        graph_representation=opts.graph_representation,
        domain=_domain_from_opts(opts),
        iterations=opts.iterations,
        pruning=opts.feature_pruning,
        multiset_hash=bool(opts.hash == "mset"),
    )
    wlf_generator.print_init_colours()
    with TimerContextManager("collecting colours"):
        wlf_generator.collect(dataset.wlplan_dataset)
    logging.info(f"n_colours_per_layer:")
    for i, n_colours in enumerate(wlf_generator.get_layer_to_n_colours()):
        logging.info(f"  {i}={n_colours}")
    if opts.collect_only:
        logging.info("Exiting after collecting colours.")
        exit(0)

    # Construct features
    with TimerContextManager("constructing features"):
        X, y, sample_weight = embed_data(dataset=dataset, wlf_generator=wlf_generator, opts=opts)
    logging.info(f"{X.shape=}")

    # PCA visualisation
    pca_save_file = opts.visualise_pca
    if pca_save_file is not None:
        visualise(X, y, save_file=pca_save_file)
        return

    # Distinguishability testing
    if opts.distinguish_test:
        distinguish(X, y)
        return

    # Optimisation
    predictor = get_predictor(opts.optimisation)
    predictor.fit_evaluate(X, y, sample_weight=sample_weight)

    # Save model
    if opts.save_file:
        with TimerContextManager("saving model"):
            wlf_generator.set_weights(predictor.get_weights())
            wlf_generator.save(opts.save_file)


def train_gnn(opts: argparse.Namespace) -> None:
    """Trains a GNN model for learning policies.

    Args:
        opts (argparse.Namespace): parsed arguments
    """

    # Parse dataset
    with TimerContextManager("parsing training data"):
        dataset = get_dataset(opts)
        logging.info(f"{len(dataset)=}")

    graph_generator = init_graph_generator(
        graph_representation=opts.graph_representation,
        domain=_domain_from_opts(opts),
        differentiate_constant_objects=True,
    )

    train_loader, val_loader = get_data_loaders(
        dataset=dataset,
        graph_generator=graph_generator,
        batch_size=opts.batch_size,
    )

    # Optimisation
    raise NotImplementedError


if __name__ == "__main__":
    init_logger()
    opts = parse_opts()
    match opts.mode:
        case "wlf":
            logging.info("Training using WL features")
            train_wlf(opts)
        case "gnn":
            logging.info("Training using GNN features")
            train_gnn(opts)
        case _:
            raise ValueError(f"Unknown value {opts.mode=}")
