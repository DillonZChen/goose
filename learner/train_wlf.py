""" Main training pipeline script. """

import time
import argparse
import numpy as np
import representation
import models.wlf
import warnings
from sklearn.model_selection import tr_test_split
from dataset.wlf import ALL_KEY, get_dataset_from_args
from models.save_load import print_arguments, save_kernel_model
from models.wlf.core import WL_FEATURE_GENERATORS
from models.sml.core import add_sml_args, predict
from models.sml.schema_count_strategy import (
    SCS_NONE,
    SCS_ALL,
    SCS_SCHEMA_APPROX,
    SCS_SCHEMA_EXACT,
)

warnings.filterwarnings("ignore")


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("domain_pddl")
    parser.add_argument("tasks_dir")
    parser.add_argument("plans_dir")

    parser = add_sml_args(parser)

    # feature arguments
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
        choices=WL_FEATURE_GENERATORS,
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
        help="reduce feature sizes by discarding colours with count <= prune",
    )

    args = parser.parse_args()

    return args


def main():
    args = parse_args()
    print_arguments(args)
    np.random.seed(args.seed)

    # load dataset
    graphs, y_true = get_dataset_from_args(args)
    graphs_tr, graphs_va, y_tr, y_va = tr_test_split(
        graphs,
        y_true,
        test_size=0.33,
        random_state=2023,
    )

    # parse schema count strategy
    schema_strat = args.schema_count_strategy
    schemata = sorted(list(y_tr[0].keys())) if schema_strat else [ALL_KEY]
    if schema_strat == SCS_NONE:
        schemata = [ALL_KEY]
    elif schema_strat == SCS_ALL:
        pass
    elif schema_strat in {SCS_SCHEMA_EXACT, SCS_SCHEMA_APPROX}:
        schemata.remove(ALL_KEY)
    args.schemata = schemata

    # class decides whether to use classifier or regressor
    model = models.wlf.core.Model(args)
    model.train()

    # training data
    print(f"Setting up training data...")
    t = time.time()
    tr_histograms = model.compute_histograms(graphs_tr)
    n_tr_nodes = sum(len(G.nodes) for G in graphs_tr)
    print(f"Initialised {args.features} for {len(graphs_tr)} graphs")
    print(f"Collected {model.n_colours_} colours over {n_tr_nodes} nodes")
    X_tr = model.get_matrix_representation(graphs_tr, tr_histograms)
    y_tr_true = {s: [] for s in schemata}
    for y_dict in y_tr:
        for s in schemata:
            y_tr_true[s].append(y_dict[s])
    print(f"Set up training data in {time.time()-t:.2f}s")

    # validation data
    print(f"Setting up validation data...")
    model.eval()
    t = time.time()
    histograms_va = model.compute_histograms(graphs_va)
    X_va = model.get_matrix_representation(graphs_va, histograms_va)
    y_va_true = {s: [] for s in schemata}
    for y_dict in y_va:
        for s in schemata:
            y_va_true[s].append(y_dict[s])
    print(f"Set up validation data in {time.time()-t:.2f}s")

    # wl colour information
    n_hit_colours = model.get_hit_colours()
    n_missed_colours = model.get_missed_colours()
    ratio = n_hit_colours / (n_hit_colours + n_missed_colours)
    print(f"hit colours: {n_hit_colours}")
    print(f"missed colours: {n_missed_colours}")
    print(f"ratio hit/all colours: {ratio:.2f}")

    # training
    print(f"Training {args.model}...")
    t = time.time()
    model.fit_all(X_tr, y_tr_true)
    print(f"Model training completed in {time.time()-t:.2f}s")

    # predict logging and model saving
    predict(model, X_tr, y_tr_true, X_va, y_va_true, schemata, schema_strat)

    # save model
    save_kernel_model(model, args)


if __name__ == "__main__":
    main()
