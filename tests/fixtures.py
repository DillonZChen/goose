import os
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

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
_ROOT_DIR = os.path.normpath(f"{_CUR_DIR}/..")
_BENCHMARKS_DIR = os.path.join(_ROOT_DIR, "benchmarks")


def get_data_input_argument(benchmark_group: str, domain_name: str) -> str:
    data_input_arg = f"{_BENCHMARKS_DIR}/{benchmark_group}/{domain_name}"
    assert os.path.exists(data_input_arg)
    return data_input_arg
