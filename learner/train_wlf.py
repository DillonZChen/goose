""" Main training pipeline script. """

import time
import argparse
import numpy as np
import representation
import warnings
from sklearn.model_selection import train_test_split
from models.save_load import print_arguments, save_ml_model
from models.wlf.core import WL_FEATURE_GENERATORS, Model
from models.sml.core import add_sml_args, predict
from models.sml.schema_count_strategy import get_schemata_from_data
from dataset.factory import (
    state_cost_dataset_from_plans,
    group_by_problem,
    reformat_y,
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

    # load dataset and convert to graphs
    domain_pddl = args.domain_pddl
    tasks_dir = args.tasks_dir
    plans_dir = args.plans_dir
    dataset = state_cost_dataset_from_plans(domain_pddl, tasks_dir, plans_dir)
    dataset_by_problem = group_by_problem(dataset)
    graphs = []
    y_true = [d.cost for d in dataset.state_cost_data_list]
    for problem_pddl, state_cost_data_list in dataset_by_problem.items():
        rep = representation.REPRESENTATIONS[args.rep](
            domain_pddl=domain_pddl, problem_pddl=problem_pddl
        )
        for data in state_cost_data_list:
            state = rep.str_to_state(data.state)
            graph = rep.state_to_cgraph(state)
            graphs.append(graph)
    graphs_tr, graphs_va, y_tr, y_va = train_test_split(
        graphs,
        y_true,
        test_size=0.33,
        random_state=2023,
    )
    y_tr = reformat_y(y_tr)
    y_va = reformat_y(y_va)

    # parse schema count strategy
    schema_strat = args.schema_count_strategy
    schemata = get_schemata_from_data(schema_strat, dataset)

    # init model
    model = Model(args, schemata)
    model.train()

    # training data
    print(f"Setting up training data...")
    t = time.time()
    tr_histograms = model.compute_histograms(graphs_tr)
    n_tr_nodes = sum(len(G.nodes) for G in graphs_tr)
    print(f"Initialised {args.features} for {len(graphs_tr)} graphs")
    print(f"Collected {model.n_colours_} colours over {n_tr_nodes} nodes")
    X_tr = model.get_matrix_representation(graphs_tr, tr_histograms)
    print(f"Set up training data in {time.time()-t:.2f}s")

    # validation data
    print(f"Setting up validation data...")
    model.eval()
    t = time.time()
    histograms_va = model.compute_histograms(graphs_va)
    X_va = model.get_matrix_representation(graphs_va, histograms_va)
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
    model.fit_all(X_tr, y_tr)
    print(f"Model training completed in {time.time()-t:.2f}s")

    # predict logging
    predict(model, X_tr, y_tr, X_va, y_va, schemata, schema_strat)

    # save model
    save_ml_model(model, args)


if __name__ == "__main__":
    main()
