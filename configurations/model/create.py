#!/usr/bin/env python

import itertools
import os

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))

for f in os.listdir(_CUR_DIR):
    if f.endswith(".toml"):
        os.remove(f"{_CUR_DIR}/{f}")


# WL
OPTIMISERS = ["gpr", "svr", "rank-mip", "rank-lp", "rank-svm", "rank-gpc"]
ITERATIONS = [1, 2, 3, 4]
DATA_LABELS = ["plan", "state-space", "all"]
WL_CONFIGURATIONS = itertools.product(OPTIMISERS, ITERATIONS, DATA_LABELS)

# 2-LWL
OPTIMISERS = ["svr", "rank-lp"]
ITERATIONS = [2]
DATA_LABELS = ["plan"]
LWL2_CONFIGURATIONS = itertools.product(OPTIMISERS, ITERATIONS, DATA_LABELS)

# ccWL
OPTIMISERS = ["gpr", "svr", "rank-mip", "rank-lp"]
ITERATIONS = [0, 1, 2, 3, 4]
DATA_LABELS = ["plan"]
CCWL_CONFIGURATIONS = itertools.product(OPTIMISERS, ITERATIONS, DATA_LABELS)

# iWL
OPTIMISERS = ["svr", "rank-lp"]
ITERATIONS = [2]
DATA_LABELS = ["plan"]
IWL_CONFIGURATIONS = itertools.product(OPTIMISERS, ITERATIONS, DATA_LABELS)

# niWL
OPTIMISERS = ["svr", "rank-lp"]
ITERATIONS = [2]
DATA_LABELS = ["plan"]
NIWL_CONFIGURATIONS = itertools.product(OPTIMISERS, ITERATIONS, DATA_LABELS)


def main():
    for description, configurations in [
        ("wl", WL_CONFIGURATIONS),
        ("lwl2", LWL2_CONFIGURATIONS),
        ("ccwl", CCWL_CONFIGURATIONS),
        ("iwl", IWL_CONFIGURATIONS),
        ("niwl", NIWL_CONFIGURATIONS),
    ]:
        for optimisation, iterations, data_generation in configurations:
            write_configurations(description, optimisation, iterations, data_generation)


def write_configurations(features, optimisation, iterations, data_generation):
    if data_generation == "state-space":
        file = f"{optimisation}-ss_{iterations}.toml"
    elif data_generation == "all":
        file = f"{optimisation}-all_{iterations}.toml"
    else:
        file = f"{optimisation}_{iterations}.toml"
    file = f"{features}_{file}"
    file = f"{_CUR_DIR}/{features}/{file}"
    os.makedirs(os.path.dirname(file), exist_ok=True)
    rank = "true" if optimisation.startswith("rank") else "false"
    if rank == "true" and data_generation == "state-space":
        return
    print(file)
    with open(file, "w") as f:
        f.write(f"features = '{features}'\n")
        f.write(f"optimisation = '{optimisation}'\n")
        f.write(f"iterations = {iterations}\n")
        # f.write(f"rank = {rank}\n")
        f.write(f"data_generation = '{data_generation}'\n")


if __name__ == "__main__":
    main()
