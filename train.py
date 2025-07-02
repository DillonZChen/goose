#!/usr/bin/env python

import argparse
import json
import logging
import os
import random
from typing import Any, Dict

import numpy as np
import termcolor as tc
import toml

from learning.dataset import get_domain_file_from_opts, get_domain_from_opts, get_training_dir_from_opts
from learning.dataset.creator.classic_labelled_dataset_creator import DatasetLabeller, log_dataset_statistics
from learning.dataset.dataset_factory import get_dataset
from learning.dataset.state_to_vec import embed_data
from learning.predictor.linear_model.predictor_factory import get_available_predictors, get_predictor
from learning.predictor.neural_network.policy_type import get_policy_type_options
from util.distinguish_test import distinguish
from util.filesystem import get_path_error_msg
from util.logging import init_logger, mat_to_str
from util.paths import DATA_CACHE_DIR
from util.pca_visualise import visualise
from util.timer import TimerContextManager
from wlplan.feature_generator import (
    get_available_feature_algorithms,
    get_available_pruning_methods,
    init_feature_generator,
)
from wlplan.graph_generator import init_graph_generator


_DESCRIPTION = """GOOSE trainer script.
  WLF models are primarily used to learn value functions for heuristic search.
  GNN models are used to learn action policies as reactive controllers.
"""

_EPILOG = """example usages:

# Train and save a classical Blocksworld model
python3 train.py benchmarks/ipc23lt/blocksworld/ configurations/model/classic.toml -s blocksworld.model

# Train and save a numeric Childsnack model
python3 train.py benchmarks/neurips24/childsnack/ configurations/model/numeric.toml -s numeric_childsnack.model

# Run a distinguishability test
python3 train.py benchmarks/ipc23lt/blocksworld/ --distinguish-test

# Save a PCA visualisation of features to file
python3 train.py benchmarks/ipc23lt/blocksworld/ --visualise-pca blocksworld_pca.png
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
def get_learning_parser():
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


def parse_learning_opts():
    parser = get_learning_parser()
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
            # add parsed opts to save_file
            contents = json.loads(open(save_file, "r").read())
            contents["opts"] = vars(opts)
            with open(save_file, "w") as f:
                json.dump(contents, f, indent=4)

        if os.path.exists(save_file):
            logging.info(f"Saved WLF model successfully to {tc.colored(save_file, 'blue')}")


def train_gnn(opts: argparse.Namespace) -> None:
    """Trains a GNN model for learning policies.

    Args:
        opts (argparse.Namespace): parsed arguments
    """

    # Torch and Pytorch Geometric imports done here to avoid unnecessary imports when not using GNN
    import torch

    from learning.predictor.neural_network.gnn import RGNN
    from learning.predictor.neural_network.optimise import optimise_weights
    from learning.predictor.neural_network.serialise import save_gnn_weights
    from learning.pyg import get_data_loaders

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    logging.info(f"Detected {tc.colored(device, 'blue')}")
    torch.manual_seed(opts.random_seed)

    # Parse dataset
    with TimerContextManager("collecting training data"):
        dataset = DatasetLabeller(opts).get_labelled_dataset()
    log_dataset_statistics(dataset)

    # Initialise PyG dataset
    domain = get_domain_from_opts(opts)
    with TimerContextManager("converting to PyG dataset"):
        graph_generator = init_graph_generator(
            graph_representation=opts.graph_representation,
            domain=domain,
            differentiate_constant_objects=True,
        )
        train_loader, val_loader = get_data_loaders(
            domain=domain,
            dataset=dataset,
            graph_generator=graph_generator,
            batch_size=opts.batch_size,
            policy_type=opts.policy_type,
        )

    # Initialise GNN
    opts._n_relations = graph_generator.get_n_relations()
    opts._n_features = graph_generator.get_n_features()
    model = RGNN.init_from_opts(opts)
    model = model.to(device)

    # Optimisation
    with TimerContextManager("optimising model"):
        weights_dict = optimise_weights(model, device, train_loader, val_loader, opts)

    # Save model
    save_file = opts.save_file
    if save_file:
        with TimerContextManager("saving_model"):
            save_gnn_weights(weights_dict)
        if os.path.exists(save_file):
            logging.info(f"Saved GNN model successfully to {tc.colored(save_file, 'blue')}")


if __name__ == "__main__":
    init_logger()
    opts = parse_learning_opts()
    match opts.mode:
        case "wlf":
            logging.info("Training using WL features")
            train_wlf(opts)
        case "gnn":
            logging.info("Training using GNN features")
            train_gnn(opts)
        case _:
            raise ValueError(f"Unknown value {opts.mode=}")
