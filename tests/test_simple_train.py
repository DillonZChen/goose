import logging
import os
from itertools import product

import pytest

from wlplan.feature_generation import (
    get_available_feature_generators,
    get_available_pruning_methods,
)

CONFIGS = {
    "features": ["wl"],
    "graph_representation": ["ilg"],
    "iterations": ["2"],
    "optimisation": ["svr", "rank-svm", "rank-lp"],
    "feature_pruning": ["none", "i-mf"],
    "data_pruning": ["equivalent-weighted"],
    "data_generation": ["plan"],
    "facts": ["fd", "all"],
    "hash": ["set"],
}


@pytest.mark.parametrize("domain", ["blocksworld", "childsnack", "satellite"])
@pytest.mark.parametrize("config", [dict(zip(CONFIGS.keys(), values)) for values in product(*CONFIGS.values())])
def test_train_ipc23lt(domain, config):
    data_config = f"configurations/data/ipc23lt/{domain}.toml"
    cmd = f"./goose.sif train {data_config} --num-data=1"
    for k, v in config.items():
        cmd += f" --{k.replace('_', '-')}={v}"
    logging.critical(cmd)
    rc = os.system(cmd)
    assert rc == 0
