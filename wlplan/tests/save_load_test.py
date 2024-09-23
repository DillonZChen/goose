#!/usr/bin/env python

import itertools
import logging

import numpy as np
import pytest
from ipc23lt import get_dataset

from wlplan.feature_generation import WLFeatures

LOGGER = logging.getLogger(__name__)

DOMAINS = ["blocksworld", "childsnack", "ferry"]
CONFIGS = {
    "static-set": {"keep_statics": True, "multiset_hash": False},
    "static-mset": {"keep_statics": True, "multiset_hash": True},
    "schema-non-static-set": {"keep_statics": False, "multiset_hash": False},
    "schema-non-static-mset": {"keep_statics": False, "multiset_hash": True},
}
PARAMETERS = itertools.product(DOMAINS, list(CONFIGS.keys()))


@pytest.mark.parametrize("domain_name,desc", PARAMETERS)
def test_save_load(domain_name, desc):
    config = CONFIGS[desc]
    save_file = f"tests/models/save_load/{domain_name}_{desc}.json"
    domain, dataset, y = get_dataset(domain_name, keep_statics=config["keep_statics"])
    feature_generator = WLFeatures(
        domain=domain,
        graph_representation="ilg",
        iterations=4,
        prune_features=None,
        multiset_hash=config["multiset_hash"],
    )
    feature_generator.collect(dataset)
    X = np.array(feature_generator.embed(dataset)).astype(float)
    n_features = feature_generator.get_n_features()
    assert X.shape[1] == n_features

    ## save
    feature_generator.save(save_file)

    ## load
    feature_generator = WLFeatures.load(save_file)

    loaded_X = np.array(feature_generator.embed(dataset)).astype(float)
    assert loaded_X.shape == X.shape
    assert (loaded_X == X).all()


if __name__ == "__main__":
    test_save_load("blocksworld")
