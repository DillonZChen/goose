""" Main training pipeline script. """

import time
import argparse
import numpy as np
import warnings
from sklearn.model_selection import train_test_split
from models.save_load import print_arguments, save_ml_model
from models.dlf.core import Model
from models.sml.core import add_sml_args, predict
from models.sml.schema_count_strategy import SCS_NONE
from models.sml.schema_count_strategy import get_schemata_from_data
from dataset.factory import state_cost_dataset_from_plans, reformat_y

warnings.filterwarnings("ignore")


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("domain_pddl")
    parser.add_argument("tasks_dir")
    parser.add_argument("plans_dir")

    parser = add_sml_args(parser)

    # feature arguments
    parser.add_argument(
        "--feature_limit",
        type=int,
        default=10000,
    )
    parser.add_argument(
        "--concept_complexity_limit",
        type=int,
        default=10,
    )
    parser.add_argument(
        "--role_complexity_limit",
        type=int,
        default=10,
    )
    parser.add_argument(
        "--boolean_complexity_limit",
        type=int,
        default=10,
    )
    parser.add_argument(
        "--count_numerical_complexity_limit",
        type=int,
        default=10,
    )
    parser.add_argument(
        "--distance_numerical_complexity_limit",
        type=int,
        default=10,
    )

    args = parser.parse_args()

    return args


def main():
    args = parse_args()
    print_arguments(args)
    np.random.seed(args.seed)

    # load dataset
    dataset = state_cost_dataset_from_plans(
        args.domain_pddl, args.tasks_dir, args.plans_dir, dlplan_state=True
    )

    # init model
    model = Model(args)
    model.train()
    X, y = model.generate_features(dataset)
    X_tr, X_va, y_tr, y_va = train_test_split(
        X, y, test_size=0.33, random_state=2024
    )
    y_tr = reformat_y(y_tr)
    y_va = reformat_y(y_va)

    # parse schema count strategy
    schema_strat = args.schema_count_strategy
    assert schema_strat == SCS_NONE
    schemata = get_schemata_from_data(schema_strat, dataset)

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
