#!/usr/bin/env python

import argparse
import json
import logging
import os
import random
import sys
import zipfile
from typing import Any, Callable, Dict

import numpy as np
import termcolor as tc
import toml
from wlplan.feature_generator import (
    get_available_feature_generators,
    get_available_pruning_methods,
    init_feature_generator,
)
from wlplan.graph_generator import init_graph_generator

from goose.enums.mode import Mode
from goose.enums.policy_type import PolicyType
from goose.enums.serialisation import namespace_from_serialisable, namespace_to_serialisable
from goose.enums.state_representation import StateRepresentation
from goose.learning.dataset import get_domain_file_from_opts, get_domain_from_opts, get_training_dir_from_opts
from goose.learning.dataset.dataset_factory import get_dataset
from goose.learning.dataset.state_to_vec import embed_data
from goose.learning.predictor.linear_model.predictor_factory import (
    get_available_predictors,
    get_predictor,
    is_unitary_classifier,
)
from goose.util.distinguish_test import distinguish
from goose.util.filesystem import get_path_error_msg
from goose.util.logging import fmt_cmd, init_logger, log_opts
from goose.util.pca_visualise import visualise
from goose.util.timer import TimerContextManager


_DESCRIPTION = """GOOSE trainer script.
  WLF models are primarily used to learn value functions for heuristic search.
  GNN models are used to learn action policies as reactive controllers.
"""

_EPILOG = f"""example usages:

train and save a classical Blocksworld model
{fmt_cmd('./train.py benchmarks/ipc23lt/blocksworld/ configurations/classic.toml -s blocksworld.model')}

train and save a numeric Childsnack model
{fmt_cmd('./train.py benchmarks/neurips24/childsnack/ configurations/numeric.toml -s numeric_childsnack.model')}

run a distinguishability test
{fmt_cmd('./train.py benchmarks/ipc23lt/blocksworld/ --distinguish-test')}

save a PCA visualisation of features to file
{fmt_cmd('./train.py benchmarks/ipc23lt/blocksworld/ --visualise-pca blocksworld_pca.png')}
"""

_DEFAULT_WLF_VALS = {
    "features": "wl",
    "optimisation": "rank-svm",
    "feature_pruning": "i-mf",
    "hash": "set",
}

_DEFAULT_GNN_VALS = {
    "num_hidden": 64,
    "learning_rate": 0.001,
    "patience": 10,
    "reduction": 0.1,
    "batch_size": 16,
    "epochs": 1024,
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
    parser.add_argument("model_config", type=str, nargs='?',
                        default=None,
                        help=f"Path to .toml model configuration file.\n" + \
                             f"If not provided, default model values are used. " + \
                             f"Model configuration file overrides command line and default values. ")

    # General model options
    gen_group = parser.add_argument_group("general model options")
    gen_group.add_argument("-g", "--graph-representation", type=str,
                        default="ilg",
                        help=f"Feature generator to use. " + \
                             f"(default: ilg)")
    gen_group.add_argument("-l", "--iterations", type=int,
                        default=2,
                        help=f"Number of WL iterations or GNN message passing layers. " + \
                             f"(default: 2)")
    gen_group.add_argument("-m", "--mode", type=Mode.parse,
                        default=Mode.WLF,
                        choices=Mode.choices(),
                        help=f"Model mode to use. " + \
                             f"(default: wlf)")
    gen_group.add_argument("-p", "--policy-type", type=PolicyType.parse, default=PolicyType.SEARCH,
                        choices=PolicyType.choices(),
                        help=f"If specified, policy representation via X-function for X in choices. " + \
                             f"(default: None, i.e. learn search)")

    # WLF options
    wlf_group = parser.add_argument_group("wlf options")
    wlf_group.add_argument("-f", "--features", type=str,
                        default=None,
                        choices=get_available_feature_generators(),
                        help=f"Feature generator to use. " + \
                            f"(default: {_DEFAULT_WLF_VALS['features']})")
    wlf_group.add_argument("-fp", "--feature-pruning", type=str,
                        default=None,
                        choices=get_available_pruning_methods(),
                        help=f"Pruning method to use for feature generation. " + \
                             f"(default: {_DEFAULT_WLF_VALS['feature_pruning']})")
    wlf_group.add_argument("--hash", type=str,
                        default=None,
                        choices=["mset", "set"],
                        help=f"Whether to use multisets or sets for neighbour colours. " + \
                             f"(default: {_DEFAULT_WLF_VALS['hash']})")
    wlf_group.add_argument("-o", "--optimisation", type=str,
                        default=None,
                        choices=get_available_predictors(),
                        help=f"Optimisation algorithm to use for prediction. " + \
                             f"(default: {_DEFAULT_WLF_VALS['optimisation']})")

    # GNN options
    gnn_group = parser.add_argument_group("gnn options")
    gnn_group.add_argument("--num-hidden", type=int,
                        default=None,
                        help=f"Hidden GNN dimension. " + \
                             f"(default: {_DEFAULT_GNN_VALS['num_hidden']})")
    gnn_group.add_argument("--learning-rate", type=float,
                        default=None,
                        help=f"Learning rate for Adam. " + \
                             f"(default: {_DEFAULT_GNN_VALS['learning_rate']})")
    gnn_group.add_argument("--patience", type=int,
                        default=None,
                        help=f"Patience for learning rate scheduler. " + \
                             f"(default: {_DEFAULT_GNN_VALS['patience']})")
    gnn_group.add_argument("--reduction", type=float,
                        default=None,
                        help=f"Reduction factor for learning rate scheduler. " + \
                            f"(default: {_DEFAULT_GNN_VALS['reduction']})")
    gnn_group.add_argument("--batch-size", type=int,
                        default=None,
                        help=f"Batch size for training. " + \
                             f"(default: {_DEFAULT_GNN_VALS['batch_size']})")
    gnn_group.add_argument("--epochs", type=int,
                        default=None,
                        help=f"Maximum number of epochs to train for. " + \
                             f"(default: {_DEFAULT_GNN_VALS['epochs']})")

    # Data options
    data_group = parser.add_argument_group("data options")
    data_group.add_argument("--cache", type=str,
                        default=None,
                        help=f"Path to labelled data or to place labelled data. If not specified, cache is not used.")
    data_group.add_argument("-nd", "--num-data", type=int,
                        default=None,
                        help=f"Number of training data to use. " + \
                             f"(default: None = all available data)")
    data_group.add_argument("-dg", "--data-generation", type=str,
                        default="plan",
                        choices=["plan", "state-space"],
                        help=f"Method for collecting data from training problems. " + \
                             f"(default: plan)")
    data_group.add_argument("-dp", "--data-pruning", type=str,
                        default="equivalent-weighted",
                        choices=["none", "equivalent", "equivalent-weighted"],
                        help=f"Method for pruning data. " + \
                             f"(default: equivalent-weighted)")
    data_group.add_argument("-sr", "--state-representation", type=StateRepresentation.parse,
                        default=StateRepresentation.DOWNWARD,
                        choices=StateRepresentation.choices(),
                        help=f"Intended facts to keep e.g. Fast Downward *downward* grounds the task and prunes away statics as well as some unreachable facts. " + \
                             f"(default: downward)")

    # Experiment options
    parser.add_argument("-r", "--random-seed", type=int,
                        default=2024,
                        help=f"Random seed for nondeterministic training algorithms.")
    parser.add_argument("-s", "--save-file", type=str,
                        default=None,
                        help=f"Path to save the model to.")
    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument("--visualise-pca", type=str,
                        default=False,
                        help=f"Path to save visualisation of PCA on WL features, for example pca.png.")
    mutex_group.add_argument("--distinguish-test", action="store_true",
                        help=f"Run distinguishability test.")
    return parser
# fmt: on


def parse_learning_opts():
    """Parses command line and config file options for training a model.
    Checks validity of options and modifies any if necessary.

    Raises:
        ValueError: invalid option configuration

    Returns:
        argparse.Namespace: training options
    """

    parser = get_learning_parser()
    opts = parser.parse_args()

    # Check parsed domain directory is valid
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

    # Subroutine for assigning namespace values
    def assign_namespace_value(key: str, value: Any) -> None:
        nonlocal opts
        opts.__setattr__(key, value)
        # Ensure strings are converted into proper enum objects
        opts = namespace_from_serialisable(opts)

    # Modify options based on configuration file
    if opts.model_config is not None:
        assert os.path.exists(opts.model_config), get_path_error_msg(opts.model_config)
        model_config = toml.load(opts.model_config)
        # Replace "" strings with None. This is because .toml files have no option to specify Python None
        for key, value in model_config.items():
            if value == "":
                model_config[key] = None
                logging.info(f"Replacing empty string value for {key} with None")
    else:
        model_config = {}

    for key, val in model_config.items():
        if key not in opts.__dict__:
            raise ValueError(f"Unknown argument *{key}* in config file")
        assign_namespace_value(key=key, value=model_config[key])
        logging.info(f"Setting option specified in config file {key}: {val}")

    def handle_config_vals(relevant_args: Dict[str, Any], irrelevant_args: Dict[str, Any]) -> None:
        for key, default_value in relevant_args.items():
            if opts.__dict__[key] is None:
                assign_namespace_value(key=key, value=default_value)
        for key in irrelevant_args:
            assign_namespace_value(key=key, value=None)
            if key in model_config:
                raise ValueError(f"Contradictory argument *{key}* in config file for {opts.mode=}")

    match opts.mode:
        case Mode.WLF:
            logging.info(f"WLF mode detected. Ignoring all GNN options.")
            handle_config_vals(relevant_args=_DEFAULT_WLF_VALS, irrelevant_args=_DEFAULT_GNN_VALS)
        case Mode.GNN:
            if opts.policy_type is None:
                raise ValueError("Heuristic learning not supported for GNNs, please specify --policy-type.")
            logging.info(f"GNN mode detected. Ignoring all WLF options.")
            handle_config_vals(relevant_args=_DEFAULT_GNN_VALS, irrelevant_args=_DEFAULT_WLF_VALS)
        case _:
            raise ValueError(f"Unknown value {opts.mode=}")

    # Modify options based on distinguish or visualisation routine
    if opts.distinguish_test or opts.visualise_pca:
        if opts.mode == Mode.GNN:
            raise ValueError("Distinguishability testing and PCA visualisation are only supported for WLF mode.")
        logging.info("Overriding options to use WLF and regression labels for non-training routine.")
        assign_namespace_value(key="optimisation", value="svr")
        assign_namespace_value(key="policy_type", value=PolicyType.SEARCH)

    # Modify predictor if policy mode is distribution
    if (
        opts.mode == Mode.WLF
        and PolicyType.is_policy_function(opts.policy_type)
        and not is_unitary_classifier(opts.optimisation)
    ):
        logging.info("Overriding optimisation for WLF policy function learner to use SVM optimisation.")
        assign_namespace_value(key="optimisation", value="svm")

    # Ensure strings are converted into proper enum objects
    opts = namespace_from_serialisable(opts)

    # Log train options
    log_opts(desc="train", opts=opts)

    # Set seeds
    random.seed(opts.random_seed)
    np.random.seed(opts.random_seed)

    return opts


def save(opts: argparse.Namespace, save_subroutine: Callable[[str], None]) -> None:
    """Save opts and model to file if specified in opts.save_file.

    Args:
        opts (argparse.Namespace): train options, containing save_file location
        save_subroutine (Callable[[str], None]): subroutine for serialisaing model component
    """

    save_file = opts.save_file
    if not save_file:
        return

    opts_file = save_file + ".opts"
    param_file = save_file + ".params"

    with TimerContextManager("saving model"):
        # serialise opts
        opts_content = namespace_to_serialisable(opts)
        opts_content = vars(opts_content)
        with open(opts_file, "w") as f:
            json.dump(opts_content, f, indent=4)

        # serialise params
        save_subroutine(param_file)

        # zip opts and params
        with zipfile.ZipFile(save_file, "w") as zipf:
            zipf.write(opts_file)
            zipf.write(param_file)

    if os.path.exists(save_file):
        logging.info(f"Saved model successfully to {tc.colored(save_file, 'blue')}")


def train_wlf(opts: argparse.Namespace) -> None:
    """Trains a linear model using WLF features for learning value functions.

    Args:
        opts (argparse.Namespace): parsed arguments
    """

    # Parse dataset
    with TimerContextManager("collecting training data"):
        domain_dataset, labels = get_dataset(opts)

    # Process dataset
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
        wlf_generator.collect(domain_dataset)
    logging.info(f"n_colours_per_layer:")
    for i, n_colours in enumerate(wlf_generator.get_layer_to_n_colours()):
        logging.info(f"  {i}={n_colours}")
    with TimerContextManager("constructing features"):
        X, y, sample_weight = embed_data(
            domain_dataset=domain_dataset,
            labels=labels,
            wlf_generator=wlf_generator,
            opts=opts,
        )
    logging.info(f"{X.shape=}")

    # [Optional] PCA visualisation
    pca_save_file = opts.visualise_pca
    if pca_save_file:
        visualise(X, y, save_file=pca_save_file)
        return

    # [Optional] Distinguishability testing
    if opts.distinguish_test:
        distinguish(X, y)
        return

    # Initialise model
    predictor = get_predictor(opts.optimisation)

    # Optimise model
    with TimerContextManager("training model"):
        predictor.fit(X, y, sample_weight)
    with TimerContextManager("evaluating model"):
        predictor.evaluate()

    # Save model
    save(opts=opts, save_subroutine=lambda x: wlf_generator.save(x, predictor.get_weights()))


def train_gnn(opts: argparse.Namespace) -> None:
    """Trains a GNN model for learning policies.

    Args:
        opts (argparse.Namespace): parsed arguments
    """

    # Torch and Pytorch Geometric imports done here to avoid unnecessary imports when not using GNN
    try:
        import torch
        import torch_geometric
    except ModuleNotFoundError:
        logging.info(
            "The current environment does not have PyTorch and PyTorch Geometric installed. "
            + "Please install them to use GNN architectures. Exiting."
        )
        sys.exit(1)

    from goose.learning.predictor.neural_network.gnn import RGNN
    from goose.learning.predictor.neural_network.optimise import optimise_weights
    from goose.learning.predictor.neural_network.serialise import save_gnn_weights
    from goose.learning.pyg import get_data_loaders

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    logging.info(f"Detected {tc.colored(device, 'blue')}")
    torch.manual_seed(opts.random_seed)

    # Parse dataset
    with TimerContextManager("collecting training data"):
        domain_dataset, labels = get_dataset(opts)

    # Process dataset
    domain = get_domain_from_opts(opts)
    with TimerContextManager("converting to PyG dataset"):
        graph_generator = init_graph_generator(
            graph_representation=opts.graph_representation,
            domain=domain,
            differentiate_constant_objects=True,
        )
        train_loader, val_loader = get_data_loaders(
            domain_dataset=domain_dataset,
            labels=labels,
            graph_generator=graph_generator,
            batch_size=opts.batch_size,
        )

    # Initialise model
    opts._n_relations = graph_generator.get_n_relations()
    opts._n_features = graph_generator.get_n_features()
    model = RGNN.init_from_opts(opts)
    model = model.to(device)
    logging.info(f"{model.num_parameters=}")

    # Optimise model
    with TimerContextManager("optimising model"):
        weights_dict = optimise_weights(model, device, train_loader, val_loader, opts)

    # Save model
    save(opts=opts, save_subroutine=lambda x: save_gnn_weights(x, weights_dict))


if __name__ == "__main__":
    init_logger()
    opts = parse_learning_opts()
    match opts.mode:
        case Mode.WLF:
            logging.info("Training using WL features")
            train_wlf(opts)
        case Mode.GNN:
            logging.info("Training using GNN features")
            train_gnn(opts)
        case _:
            raise ValueError(f"Unknown value {opts.mode=}")
