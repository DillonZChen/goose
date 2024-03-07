""" Main training pipeline script. """
import time
import argparse
import numpy as np
import random
import representation
import models.wl
import warnings
import os
import pathlib
from itertools import product
from sklearn.model_selection import train_test_split
from sklearn.metrics import log_loss, mean_squared_error
from models.wl.wl import BAYESIAN_MODELS, FREQUENTIST_MODELS
from dataset.dataset_wl import ALL_KEY, get_dataset_from_args
from models.save_load import print_arguments, save_kernel_model
from util.metrics import f1_macro

warnings.filterwarnings("ignore")

_TRAIN_ALL = True

_SC_STRAT_ALL = "all"
_SC_STRAT_NONE = "none"
_SC_STRAT_SCHEMA_ONLY = "schema"

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
        "--schema-count",
        default=_SC_STRAT_NONE,
        choices=[_SC_STRAT_NONE, _SC_STRAT_ALL, _SC_STRAT_SCHEMA_ONLY],
        help="Strategy for learning schema counts.\n"
        + "none: do not learn schema counts and learn h* prediction.\n"
        + "all: learn schema counts and sum with h* prediction.\n"
        + "schema: learn schema coutns only and not h* prediction.",
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
        choices=models.wl.GRAPH_FEATURE_GENERATORS,
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


def save_matrices(args, X_train, y_train_true, X_val, y_val_true):
    s = ALL_KEY
    domain = args.domain_pddl.split("/")[-2]
    os.makedirs(f"matrices", exist_ok=True)
    pfx = f"{domain}_{args.rep}_{args.features}_{args.iterations}"
    pfx = os.path.abspath(f"matrices/{pfx}")
    np.savetxt(f"{pfx}_X_train.csv", X_train, delimiter=" ")
    np.savetxt(f"{pfx}_y_train.csv", y_train_true[s], delimiter=" ")
    np.savetxt(f"{pfx}_X_val.csv", X_val, delimiter=" ")
    np.savetxt(f"{pfx}_y_val.csv", y_val_true[s], delimiter=" ")
    return pfx


def main():
    args = parse_args()
    print_arguments(args)
    np.random.seed(args.seed)
    random.seed(args.seed)

    # load dataset
    graphs, y_true = get_dataset_from_args(args)

    schema_strat = args.schema_count
    schemata = sorted(list(y_true[0].keys())) if schema_strat else [ALL_KEY]
    if schema_strat == _SC_STRAT_NONE:
        schemata = [ALL_KEY]
    elif schema_strat == _SC_STRAT_ALL:
        pass
    elif schema_strat == _SC_STRAT_SCHEMA_ONLY:
        schemata.remove(ALL_KEY)
    args.schemata = schemata

    if _TRAIN_ALL:
        graphs_train = graphs
        y_train = y_true
        assert schema_strat == _SC_STRAT_NONE
    else:
        graphs_train, graphs_val, y_train, y_val = train_test_split(
            graphs, y_true, test_size=0.33, random_state=args.seed
        )

    # class decides whether to use classifier or regressor
    model = models.wl.Model(args)
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
    X_train = X_train.astype(np.float64)
    print(f"X_train shape: {X_train.shape}")
    y_train_true = {s: [] for s in schemata}
    for y_dict in y_train:
        for s in schemata:
            y_train_true[s].append(y_dict[s])
    print(f"Set up training data in {time.time()-t:.2f}s")

    # validation data
    if not _TRAIN_ALL:
        print(f"Setting up validation data...")
        model.eval()
        t = time.time()
        val_histograms = model.compute_histograms(graphs_val)
        X_val = model.get_matrix_representation(graphs_val, val_histograms)
        X_val = X_val.astype(np.float64)
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
    if not _TRAIN_ALL:
        print("  For val...")
        y_val_pred = model.predict_all(X_val)
    print(f"Predicting completed in {time.time()-t:.2f}s")

    # metrics
    print("Scores on prediction against h*:")
    itrs = list(product(scoring.keys(), schemata))
    train_scores = {
        (m, s): scoring[m](y_train_true[s], y_train_pred[s]) for m, s in itrs
    }
    if not _TRAIN_ALL:
        val_scores = {
            (m, s): scoring[m](y_val_true[s], y_val_pred[s]) for m, s in itrs
        }
    t = time.time()
    schemata_to_keep = set()
    for metric in scoring:
        if not _TRAIN_ALL:
            print(f"{metric:<10} {'schema':<20} {'train':<10} {'val':<10}")
            for schema in schemata:
                t = train_scores[(metric, schema)]
                v = val_scores[(metric, schema)]
                print(f"{'':<10} {schema:<20} {t:<10.4f} {v:<10.4f}")
                if abs(v - 1) < _F1_KEEP_TOL and metric == F1_KEY:
                    schemata_to_keep.add(schema)
        else:
            print(f"{metric:<10} {'schema':<20} {'train':<10}")
            for schema in schemata:
                t = train_scores[(metric, schema)]
                print(f"{'':<10} {schema:<20} {t:<10.4f}")

    if schema_strat in {_SC_STRAT_ALL, _SC_STRAT_NONE}:
        schemata_to_keep.add(ALL_KEY)
    elif schema_strat == _SC_STRAT_SCHEMA_ONLY and ALL_KEY in schemata_to_keep:
        schemata_to_keep.remove(ALL_KEY)

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
