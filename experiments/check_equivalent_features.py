#!/usr/bin/env python3

import json
import os
from itertools import product

from wlplan.feature_generation import Features, WLFeatures

CUR_DIR = os.path.dirname(os.path.abspath(__file__))
MDL_DIR = os.path.normpath(f"{CUR_DIR}/_models")

with open(f"{CUR_DIR}/config.json") as f:
    CONFIG = json.load(f)

DOMAINS = CONFIG["domains"]
FEATURES = CONFIG["features"]
FEATURE_PRUNING = CONFIG["feature_pruning"]
DATA_PRUNING = CONFIG["data_pruning"]
OPTIMISERS = CONFIG["optimisers"]
DATA_GENERATION = CONFIG["data_generation"]
FACTS = CONFIG["facts"]

for config in product(
    DOMAINS,
    ["wl"],
    ["none"],
    DATA_PRUNING,
    OPTIMISERS,
    DATA_GENERATION,
    FACTS,
    ["4"],
    ["0"],
):
    domain, feature, fpruning, dpruning, optimiser, data_gen, facts, iterations, repeat = config
    job_description = "_".join(config)
    fp_none = f"{MDL_DIR}/{job_description}.model"
    config = list(config)
    config[2] = "collapse-all"
    fp_collapse_all = f"{MDL_DIR}/{job_description}.model"
    if not (os.path.exists(fp_none) and os.path.exists(fp_collapse_all)):
        continue

    fg_none = WLFeatures.load(fp_none)
    fg_collapse_all = WLFeatures.load(fp_collapse_all)

    breakpoint()
