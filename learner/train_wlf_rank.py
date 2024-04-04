""" Main training pipeline script. """
import argparse
import itertools
import time
import warnings

import numpy as np
import representation
from dataset.factory import (DATA_PATH, StateCostDataset, group_by_problem, reformat_y,
                             state_cost_dataset_from_plans)
from models.save_load import print_arguments, save_ml_model
from models.sml.core import add_sml_args
from models.sml.schema_count_strategy import get_schemata_from_data
from models.wlf.core import WL_FEATURE_GENERATORS, Model

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

    parser.add_argument(
        "--pair",
        type=str,
        default="combination",
        choices=["combination", "sequential", "neighbor"],
        help="How to train the ranker",
    )

    parser.add_argument(
        "--load",
        action="store_true",
        help="load data rather than generate",
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
    if args.pair == "neighbor":
        if args.load:
            dataset = StateCostDataset.load(DATA_PATH)
        else:
            dataset = state_cost_dataset_from_plans(domain_pddl, tasks_dir, plans_dir, planner="fd-rank")
    else:
        dataset = state_cost_dataset_from_plans(domain_pddl, tasks_dir, plans_dir)
    dataset_by_problem = group_by_problem(dataset)
    graphs_by_problem = []
    i = 0
    for problem_pddl, state_cost_data_list in dataset_by_problem.items():
        rep = representation.REPRESENTATIONS[args.rep](
            domain_pddl=domain_pddl, problem_pddl=problem_pddl
        )
        prob_list = []
        for data in state_cost_data_list:
            state = rep.str_to_state(data.state)
            graph = rep.state_to_cgraph(state)
            if args.pair == "neighbor":
                prob_list.append((graph, data.cost, (data.loc[0], data.loc[1], i)))
            else:
                prob_list.append((graph, data.cost, i))
        graphs_by_problem.append(prob_list)
        i += 1
    pairs_va = graphs_by_problem[::len(dataset_by_problem)//10]
    pairs_tr = [x for x in graphs_by_problem if x not in pairs_va]
    graphs_tr = [[graph for (graph, _, _) in d_pairs] for d_pairs in pairs_tr]
    graphs_va = [[graph for (graph, _, _) in d_pairs] for d_pairs in pairs_va]
    y_tr = [[value for (_, value, _) in d_pairs] for d_pairs in pairs_tr]
    y_va = [[value for (_, value, _) in d_pairs] for d_pairs in pairs_va]
    idx_tr = [[idx for (_, _, idx) in d_pairs] for d_pairs in pairs_tr]
    idx_va = [[idx for (_, _, idx) in d_pairs] for d_pairs in pairs_va]
    y_tr = reformat_y(list(itertools.chain.from_iterable(y_tr)))
    y_va = reformat_y(list(itertools.chain.from_iterable(y_va)))
    # parse schema count strategy
    schema_strat = args.schema_count_strategy
    schemata = get_schemata_from_data(schema_strat, dataset)

    # init model
    model = Model(args, schemata)
    model.train()

    # training data
    print(f"Setting up training data...")
    t = time.time()
    tr_histograms = model.compute_histograms(list(itertools.chain.from_iterable(graphs_tr)))
    n_tr_nodes = sum(len(G.nodes) for G in list(itertools.chain.from_iterable(graphs_tr)))
    print(f"Initialised {args.features} for {len(graphs_tr)} graphs")
    print(f"Collected {model.n_colours_} colours over {n_tr_nodes} nodes")
    X_tr = model.get_matrix_representation(list(itertools.chain.from_iterable(graphs_tr)), tr_histograms)
    idx_tr = np.array(list(itertools.chain.from_iterable(idx_tr)))
    if len(idx_tr.shape) == 1:
        idx_tr = idx_tr.reshape((-1, 1))
    # X_tr = np.concatenate((X_tr, idx_tr.reshape([-1, 1])), axis=1)
    for key in y_tr.keys():
        y_tr[key] = np.concatenate((np.array(y_tr[key]).reshape((-1, 1)),
                              idx_tr), axis=1)
    print(f"Set up training data in {time.time()-t:.2f}s")

    # validation data
    print(f"Setting up validation data...")
    model.eval()
    t = time.time()
    histograms_va = model.compute_histograms(list(itertools.chain.from_iterable(graphs_va)))
    X_va = model.get_matrix_representation(list(itertools.chain.from_iterable(graphs_va)), histograms_va)
    idx_va = np.array(list(itertools.chain.from_iterable(idx_va)))
    if len(idx_va.shape) == 1:
        idx_va = idx_va.reshape((-1, 1))
    # X_tr = np.concatenate((X_tr, idx_tr.reshape([-1, 1])), axis=1)
    for key in y_va.keys():
        y_va[key] = np.concatenate((np.array(y_va[key]).reshape((-1, 1)),
                                    idx_va), axis=1)
    # X_va = np.concatenate((X_va, idx_va.reshape([-1, 1])), axis=1)
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
    # predict(model, X_tr, y_tr, X_va, y_va, schemata, schema_strat)
    # svm_model:RankSVM = model.get_learning_model('_all_')
    model.score(X_tr, y_tr['_all_'])
    model.score(X_va, y_va['_all_'])

    # save model
    save_ml_model(model, args)


if __name__ == "__main__":
    main()
