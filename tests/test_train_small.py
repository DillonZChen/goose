import json
import os
import sys
import traceback
from itertools import product

import pytest

CUR_DIR = os.path.dirname(os.path.abspath(__file__))

# so pytest can import from root directory
sys.path.append(os.path.realpath(os.path.dirname(__file__) + "/.."))
from learning.options import get_parser
from train import train

with open(f"{CUR_DIR}/test_configs.json") as f:
    CONFIG = json.load(f)
keys = [
    "features",
    "feature_pruning",
    "data_pruning",
    "optimisers",
    "data_generation",
    "iterations",
    "facts",
]
fixture_keys = ",".join(keys)
fixture_values = product(*[CONFIG[key] for key in keys])

print(fixture_keys)
print(fixture_values)


# @pytest.mark.filterwarnings("ignore:not supported")
@pytest.mark.parametrize(fixture_keys, fixture_values)
def test_train_small(features, feature_pruning, data_pruning, optimisers, data_generation, iterations, facts):
    data_config = f"{CUR_DIR}/bw-small/config.toml"
    parser = get_parser()
    args = [
        data_config,
        "--features",
        features,
        "--graph_representation",
        "ilg",
        "--feature_pruning",
        feature_pruning,
        "--data_pruning",
        data_pruning,
        "--optimisation",
        optimisers,
        "--data_generation",
        data_generation,
        "--iterations",
        iterations,
        "--facts",
        facts,
    ]
    args = [str(arg) for arg in args]
    opts = parser.parse_args(args)
    try:
        train(opts)
    except Exception as e:
        # check if `not supported' in error message
        if "not supported" in str(e):
            pytest.skip("Configuration not supported")
        else:
            traceback.print_exc()
            assert False
