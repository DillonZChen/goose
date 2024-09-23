import logging
import time

import numpy as np
import pytest
from ipc23lt import DOMAINS, get_dataset
from util import print_mat

from wlplan.feature_generation import WLFeatures

LOGGER = logging.getLogger(__name__)

REPEATS = 10


@pytest.mark.parametrize("domain_name", sorted(DOMAINS))
def test_profile(domain_name):
    data = {}

    configs = {
        "set": {"multiset_hash": False},
        "mset": {"multiset_hash": True},
    }

    for desc, config in configs.items():
        domain, dataset, _ = get_dataset(domain_name, keep_statics=False)
        feature_generator = WLFeatures(
            domain=domain,
            iterations=4,
            prune_features=None,
            multiset_hash=config["multiset_hash"],
        )
        feature_generator.collect(dataset)
        t = time.time()
        for _ in range(REPEATS):
            X = feature_generator.embed(dataset)
        t = (time.time() - t) / REPEATS
        X = np.array(X)
        data[desc] = {
            "n_data": X.shape[0],
            "n_feat": X.shape[1],
            "time": f"{t:.4f}",
        }

    itr = next(iter(configs.keys()))
    mat = [["config"] + list(data[itr].keys())]
    for desc in configs:
        mat.append([desc] + list(data[desc].values()))
    print_mat(mat)
