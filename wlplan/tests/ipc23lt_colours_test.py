import logging

import numpy as np
import pytest
from ipc23lt import get_dataset

from util import print_mat
from wlplan.feature_generation import WLFeatures

LOGGER = logging.getLogger(__name__)

## https://dillonzchen.github.io/publications/chen-trevizan-thiebaux-icaps2024.pdf
FD_COLOURS = {
    "blocksworld": 10444,
    "childsnack": 251,
    "ferry": 3228,
    ### performing all the tests takes too long
    # "floortile": 7616,
    # "miconic": 108,
    # "rovers": 23202,
    # "satellite": 22155,
    # "sokoban": 110,
    # "spanner": 350,
    # "transport": 3787,
}


@pytest.mark.parametrize("domain_name", sorted(FD_COLOURS.keys()))
def test_domain(domain_name):
    n_features = {}

    configs = {
        "static-set": {"keep_statics": True, "multiset_hash": False},
        "static-mset": {"keep_statics": True, "multiset_hash": True},
        "schema-non-static-set": {"keep_statics": False, "multiset_hash": False},
        "schema-non-static-mset": {"keep_statics": False, "multiset_hash": True},
    }

    for desc, config in configs.items():
        domain, dataset, _ = get_dataset(domain_name, keep_statics=config["keep_statics"])
        feature_generator = WLFeatures(
            domain=domain,
            graph_representation="ilg",
            iterations=4,
            prune_features=None,
            multiset_hash=config["multiset_hash"],
        )
        feature_generator.collect(dataset)
        X = np.array(feature_generator.embed(dataset)).astype(float)
        n_features[desc] = feature_generator.get_n_features()
        assert X.shape[1] == n_features[desc]

    n_features["fd-non-static-mset-icaps24"] = FD_COLOURS[domain_name]

    mat = [["config", "n_feat"]]
    for desc, n_feat in n_features.items():
        mat.append([desc, n_feat])
    print_mat(mat)

    ## multiset should always give more features
    assert n_features["static-set"] <= n_features["static-mset"]
    assert n_features["schema-non-static-set"] <= n_features["schema-non-static-mset"]

    ## removing statics by schema analysis should always give fewer features
    assert n_features["schema-non-static-set"] <= n_features["static-set"]
    assert n_features["schema-non-static-mset"] <= n_features["static-mset"]
