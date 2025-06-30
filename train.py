#!/usr/bin/env python

import argparse
import logging
import os

import termcolor as tc

from learning.dataset import get_domain_from_opts
from learning.dataset.creator.classic_labelled_dataset_creator import DatasetLabeller
from learning.dataset.dataset_factory import get_dataset
from learning.dataset.state_to_vec import embed_data
from learning.options import parse_opts
from learning.predictor.linear_model.predictor_factory import get_predictor
from util.distinguish_test import distinguish
from util.logging import init_logger
from util.pca_visualise import visualise
from util.timer import TimerContextManager
from wlplan.feature_generator import init_feature_generator
from wlplan.graph_generator import init_graph_generator


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
        domain=get_domain_from_opts(opts),
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
    with TimerContextManager("training model"):
        predictor.fit(X, y, sample_weight)
    with TimerContextManager("evaluating model"):
        predictor.evaluate()

    # Save model
    save_file = opts.save_file
    if save_file:
        with TimerContextManager("saving model"):
            wlf_generator.set_weights(predictor.get_weights())
            wlf_generator.save(save_file)
        if os.path.exists(save_file):
            logging.info(f"Saved WLF model successfully to {tc.colored(save_file, 'blue')}")


def train_gnn(opts: argparse.Namespace) -> None:
    """Trains a GNN model for learning policies.

    Args:
        opts (argparse.Namespace): parsed arguments
    """

    # Torch and Pytorch Geometric imports are done here to avoid unnecessary imports when not using GNN
    import torch

    from learning.dataset.pyg import get_data_loaders
    from learning.predictor.neural_network.gnn import RGNN
    from learning.predictor.neural_network.optimise import optimise_weights
    from learning.predictor.neural_network.serialise import save_gnn_weights

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    logging.info(f"Detected {device}")

    # Parse dataset
    with TimerContextManager("collecting training data"):
        list_labelled_problem_data = DatasetLabeller(opts).compute_labelled_problems_dataset()

    # Initialise PyG dataset
    with TimerContextManager("converting to PyG dataset"):
        graph_generator = init_graph_generator(
            graph_representation=opts.graph_representation,
            domain=get_domain_from_opts(opts),
            differentiate_constant_objects=True,
        )
        train_loader, val_loader = get_data_loaders(
            dataset=dataset,
            graph_generator=graph_generator,
            batch_size=opts.batch_size,
        )

    # Initialise GNN
    model = RGNN(
        n_relations=graph_generator.get_n_relations(),
        in_feat=graph_generator.get_n_features(),
        out_feat=1,
        n_hid=opts.num_hidden,
        n_layers=opts.iterations,
        aggr="max",
        pool="sum",
    )
    model = model.to(device)

    # Optimisation
    with TimerContextManager("optimising model"):
        model_dict = optimise_weights(model, device, train_loader, val_loader, opts)

    # Save model
    save_file = opts.save_file
    if save_file:
        with TimerContextManager("saving_model"):
            save_gnn_weights(model_dict)
        if os.path.exists(save_file):
            logging.info(f"Saved GNN model successfully to {tc.colored(save_file, 'blue')}")


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
            raise ValueError(f"Unknown value {opts.mode=}")
