""" Main training pipeline script. """

import time
import argparse
import numpy as np
import warnings
from sklearn.model_selection import train_test_split
from dataset.factory import ALL_KEY, state_cost_dataset_from_spaces
from models.save_load import print_arguments
from models.dlf.core import Model
from models.sml.core import add_sml_args, predict
from models.sml.schema_count_strategy import SCS_NONE

warnings.filterwarnings("ignore")


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("domain_pddl")
    parser.add_argument("tasks_dir")
    parser.add_argument("plans_dir")

    parser = add_sml_args(parser)

    # feature arguments
    parser.add_argument("--feature_limit", type=int, default=10000)
    parser.add_argument("--concept_complexity", type=int, default=10)
    parser.add_argument("--role_complexity", type=int, default=10)
    parser.add_argument("--boolean_complexity", type=int, default=10)
    parser.add_argument("--count_num_complexity", type=int, default=10)
    parser.add_argument("--distance_num_complexity", type=int, default=10)

    args = parser.parse_args()

    return args


def main():
    args = parse_args()
    print_arguments(args)
    np.random.seed(args.seed)

    assert args.schema_count_strategy == SCS_NONE

    # load dataset
    problem_states_dict, vocabulary_info = state_cost_dataset_from_spaces(
        args.domain_pddl, args.tasks_dir
    )

    # init model
    model = Model(args)
    model.train()
    X, y = model.convert_training_data(problem_states_dict, vocabulary_info)
    X_tr, X_va, y_tr, y_va = train_test_split(X, y, test_size=0.33, random_state=2024)
    y_tr = {ALL_KEY: y_tr}  # because of action schemata learning
    y_va = {ALL_KEY: y_va}

    # training
    print(f"Training {args.model}...")
    t = time.time()
    model.fit_all(X_tr, y_tr)
    print(f"Model training completed in {time.time()-t:.2f}s")

    # predict logging
    predict(model, X_tr, y_tr, X_va, y_va, [ALL_KEY], SCS_NONE)


if __name__ == "__main__":
    main()
