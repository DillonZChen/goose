from itertools import product

IPC23LT_DOMAINS = [
    "blocksworld",
    "childsnack",
    "ferry",
    "floortile",
    "miconic",
    "rovers",
    "satellite",
    "sokoban",
    "spanner",
    "transport",
]

CONFIGS_MAP = {
    "features": ["wl"],
    "graph_representation": ["ilg", "ploig"],
    "iterations": ["2"],
    "optimisation": ["svr", "rank-svm", "rank-lp"],
    "feature_pruning": ["none", "i-mf"],
    "data_pruning": ["equivalent-weighted"],
    "data_generation": ["plan"],
    "facts": ["fd", "all"],
    "hash": ["set"],
}
CONFIGS = [dict(zip(CONFIGS_MAP.keys(), values)) for values in product(*CONFIGS_MAP.values())]
