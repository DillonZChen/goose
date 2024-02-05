""" Main training pipeline script. """

import time
import argparse
import numpy as np
import representation
import models.wlf
import warnings
import os
import pathlib
from itertools import product
from sklearn.model_selection import train_test_split
from sklearn.metrics import log_loss, mean_squared_error
from models.wlf.model import BAYESIAN_MODELS, FREQUENTIST_MODELS
from dataset.dataset_wl import ALL_KEY, get_dataset_from_args
from models.save_load import print_arguments, save_kernel_model
from util.metrics import f1_macro

warnings.filterwarnings("ignore")

_SCS_ALL = "all"
_SCS_NONE = "none"
_SCS_SCHEMA_EXACT = "schema_exact"
_SCS_SCHEMA_APPROX = "schema_approx"

_F1_KEEP_TOL = 1e-3

F1_KEY = "f1_macro"
MSE_KEY = "mse"

_SCORING_HEURISTIC = {
    MSE_KEY: mean_squared_error,
    F1_KEY: f1_macro,
}


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("domain_pddl")
    parser.add_argument("tasks_dir")
    parser.add_argument("plans_dir")

    # ml model arguments
    parser.add_argument(
        "-m",
        "--model",
        type=str,
        default="linear-svr",
        choices=FREQUENTIST_MODELS + BAYESIAN_MODELS,
        help="ML model",
    )
    parser.add_argument(
        "--save-file",
        type=str,
        default=None,
        help="save file of model weights",
    )

    # data arguments
    parser.add_argument(
        "-s",
        "--schema_count_strategy",
        default=_SCS_NONE,
        choices=[_SCS_NONE, _SCS_ALL, _SCS_SCHEMA_EXACT, _SCS_SCHEMA_APPROX],
        help="Strategy for learning schema counts.\n"
        + f"{_SCS_NONE}: learn h* prediction.\n"
        + f"{_SCS_ALL}: learn schema counts and sum with h* prediction.\n"
        + f"{_SCS_SCHEMA_EXACT}: try to learn schema counts exactly.\n"
        + f"{_SCS_SCHEMA_APPROX}: try to learn schema counts approximately.\n",
    )
    parser.add_argument(
        "-r",
        "--rep",
        type=str,
        default="ilg",
        choices=representation.REPRESENTATIONS,
        help="graph representation to use",
    )
    parser.add_argument(
        "-k",
        "--features",
        type=str,
        default="1wl",
        choices=models.wlf.model.GRAPH_FEATURE_GENERATORS,
        help="wl algorithm to use",
    )
    parser.add_argument(
        "-l",
        "--iterations",
        type=int,
        default=4,
        help="number of iterations for wl algorithms",
    )
    parser.add_argument(
        "-p",
        "--prune",
        type=int,
        default=0,
        help="reduce feature sizes by discarding colours with total train count <= prune",
    )
    parser.add_argument("--seed", type=int, default=0, help="random seed")
    parser.add_argument("--planner", default="fd", choices=["fd", "pwl"])

    args = parser.parse_args()

    return args


def main():
    args = parse_args()
    print_arguments(args)
    np.random.seed(args.seed)

    # load dataset
    graphs, y_true = get_dataset_from_args(args)

    graphs_train, graphs_val, y_train, y_val = train_test_split(
        graphs, y_true, test_size=0.33, random_state=2023
    )

    # parse schema count strategy
    schema_strat = args.schema_count_strategy
    schemata = sorted(list(y_train[0].keys())) if schema_strat else [ALL_KEY]
    if schema_strat == _SCS_NONE:
        schemata = [ALL_KEY]
    elif schema_strat == _SCS_ALL:
        pass
    elif schema_strat in {_SCS_SCHEMA_EXACT, _SCS_SCHEMA_APPROX}:
        schemata.remove(ALL_KEY)
    args.schemata = schemata

    # class decides whether to use classifier or regressor
    model = models.wlf.model.Model(args)
    model.train()
    scoring = _SCORING_HEURISTIC

    # training data
    print(f"Setting up training data...")
    t = time.time()
    train_histograms = model.compute_histograms(graphs_train)
    n_train_nodes = sum(len(G.nodes) for G in graphs_train)
    print(f"Initialised {args.features} for {len(graphs_train)} graphs")
    print(f"Collected {model.n_colours_} colours over {n_train_nodes} nodes")
    X_train = model.get_matrix_representation(graphs_train, train_histograms)
    y_train_true = {s: [] for s in schemata}
    for y_dict in y_train:
        for s in schemata:
            y_train_true[s].append(y_dict[s])
    print(f"Set up training data in {time.time()-t:.2f}s")

    # validation data
    print(f"Setting up validation data...")
    model.eval()
    t = time.time()
    val_histograms = model.compute_histograms(graphs_val)
    X_val = model.get_matrix_representation(graphs_val, val_histograms)
    y_val_true = {s: [] for s in schemata}
    for y_dict in y_val:
        for s in schemata:
            y_val_true[s].append(y_dict[s])
    print(f"Set up validation data in {time.time()-t:.2f}s")

    n_hit_colours = model.get_hit_colours()
    n_missed_colours = model.get_missed_colours()
    ratio = n_hit_colours / (n_hit_colours + n_missed_colours)
    print(f"hit colours: {n_hit_colours}")
    print(f"missed colours: {n_missed_colours}")
    print(f"ratio hit/all colours: {ratio:.2f}")

    # training
    print(f"Training {args.model}...")
    t = time.time()
    model.fit_all(X_train, y_train_true)
    print(f"Model training completed in {time.time()-t:.2f}s")

    # predict on train and val sets
    print("Predicting...")
    t = time.time()
    print("  For train...")
    y_train_pred = model.predict_all(X_train)
    print("  For val...")
    y_val_pred = model.predict_all(X_val)
    print(f"Predicting completed in {time.time()-t:.2f}s")

    # metrics
    print("Scores on prediction against h*:")
    itrs = list(product(scoring.keys(), schemata))
    train_scores = {
        (m, s): scoring[m](y_train_true[s], y_train_pred[s]) for m, s in itrs
    }
    val_scores = {
        (m, s): scoring[m](y_val_true[s], y_val_pred[s]) for m, s in itrs
    }
    t = time.time()
    schemata_to_keep = set()
    for metric in scoring:
        print(f"{metric:<10} {'schema':<20} {'train':<10} {'val':<10}")
        for schema in schemata:
            t = train_scores[(metric, schema)]
            v = val_scores[(metric, schema)]
            print(f"{'':<10} {schema:<20} {t:<10.4f} {v:<10.4f}")
            if (abs(v - 1) < _F1_KEEP_TOL and metric == F1_KEY) or \
               schema_strat == _SCS_SCHEMA_APPROX:
                schemata_to_keep.add(schema)
    if schema_strat in {_SCS_ALL, _SCS_NONE}:
        schemata_to_keep.add(ALL_KEY)

    # log schemata to learn
    print(f"{len(schemata_to_keep)} schemata to keep")
    for s in sorted(schemata_to_keep):
        print(f"  {s}")
    model.update_schemata_to_learn(schemata_to_keep)

    # save model
    save_kernel_model(model, args)

    # print % weights that are zero
    try:
        n_zero_weights = model.get_num_zero_weights()
        n_weights = model.get_num_weights()
        print(
            f"zero_weights: {n_zero_weights}/{n_weights} = {n_zero_weights/n_weights:.2f}"
        )
    except Exception as e:  # not possible for nonlinear kernel methods
        pass


if __name__ == "__main__":
    main()
