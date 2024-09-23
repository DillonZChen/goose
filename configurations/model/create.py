#!/usr/bin/env python

import itertools
import os

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))

for f in os.listdir(_CUR_DIR):
    if f.endswith(".toml"):
        os.remove(f"{_CUR_DIR}/{f}")

OPTIMISERS = ["gpr", "svr", "rank-mip", "rank-lp", "rank-svm", "rank-gpc"]
ITERATIONS = [1, 2, 3, 4]
DATA_LABELS = ["plan", "state-space", "all"]

CONFIGURATIONS = itertools.product(OPTIMISERS, ITERATIONS, DATA_LABELS)

for optimisation, iterations, data_generation in CONFIGURATIONS:
    if data_generation == "state-space":
        file = f"{_CUR_DIR}/{optimisation}-ss_{iterations}.toml"
    elif data_generation == "all":
        file = f"{_CUR_DIR}/{optimisation}-all_{iterations}.toml"
    else:
        file = f"{_CUR_DIR}/{optimisation}_{iterations}.toml"
    rank = "true" if optimisation.startswith("rank") else "false"
    if rank == "true" and data_generation == "state-space":
        continue
    with open(file, "w") as f:
        f.write(f"optimisation = '{optimisation}'\n")
        f.write(f"iterations = {iterations}\n")
        f.write(f"rank = {rank}\n")
        f.write(f"data_generation = '{data_generation}'\n")
