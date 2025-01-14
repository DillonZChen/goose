import json
import os
import re
from itertools import product

import numpy as np
import pandas as pd
import plotly.express as px
from tqdm import tqdm

CUR_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.normpath(f"{CUR_DIR}/../..")

LOG_PLAN_DIR = os.path.normpath(f"{CUR_DIR}/../_log_plan/gadi")
LOG_TRAIN_DIR = os.path.normpath(f"{CUR_DIR}/../_log_train")

PLOT_DIR = os.path.normpath(f"{CUR_DIR}/../_plots")
os.makedirs(PLOT_DIR, exist_ok=True)

with open(f"{ROOT_DIR}/experiments/config.json") as f:
    CONFIG = json.load(f)

DOMAINS = CONFIG["domains"]
FEATURES = CONFIG["features"]
PRUNING = CONFIG["pruning"]
OPTIMISERS = CONFIG["optimisers"]
DATA_GENERATION = CONFIG["data_generation"]
ITERATIONS = [str(i) for i in CONFIG["iterations"]]
REPEATS = [str(i) for i in range(CONFIG["repeats"])]

PROBLEMS = sorted([f"{x}_{y:02d}" for y in range(3, 31, 3) for x in [0, 1, 2]])

TIMEOUT = 300

CONFIG_KEYS = [
    "domain",
    "features",
    "pruning",
    "optimiser",
    "data_generation",
    "iterations",
]

PLAN_DF_KEYS = CONFIG_KEYS + [
    "problem",
    "repeat",
    "tried",
    "solved",
    "expanded",
    "plan_length",
    "runtime",
]

TRAIN_DF_KEYS = CONFIG_KEYS + [
    "tried",
    "completed",
    "oom",
    "n_colours",
    "n_data",
    "collection_time",
    "construction_time",
    "training_time",
]


def parse_plan_log(log_path: str):
    data = {
        "tried": False,
        "solved": False,
        "expanded": -1,
        "plan_length": -1,
        "runtime": TIMEOUT,
    }

    if not os.path.exists(log_path):
        return data
    with open(log_path, "r") as file:
        content = file.read()

    data["tried"] = True
    solved = "Solution found" in content
    if solved:
        # [t=0.016241s, 10888 KB] Plan length: 10 step(s).
        # [t=0.016241s, 10888 KB] Expanded 13 state(s).
        # INFO     Planner time: 0.15s
        plan_length = re.search(r"Plan length: (\d+) step\(s\)", content)
        expanded = re.search(r"Expanded (\d+) state\(s\)", content)
        runtime = re.search(r"Planner time: ([\d.]+)s", content)
        data["solved"] = True
        data["plan_length"] = int(plan_length.group(1))
        data["expanded"] = int(expanded.group(1))
        data["runtime"] = float(runtime.group(1))
    return data


def parse_train_log(log_path: str):
    data = {
        "tried": False,
        "completed": False,
        "oom": False,
        "n_colours": -1,
        "n_data": -1,
        "collection_time": -1,
        "construction_time": -1,
        "training_time": -1,
    }

    if not os.path.exists(log_path):
        return data
    with open(log_path, "r") as file:
        content = file.read()

    def try_match(pattern, default):
        match = re.search(pattern, content)
        return match.group(1) if match else default

    data["tried"] = True
    data["completed"] = "Finished saving model" in content
    data["oom"] = "OOM" in content
    data["n_colours"] = int(try_match(r"n_refined_colours=(\d+)", -1))
    data["n_data"] = int(try_match(r"X.shape=\((\d+),", -1))
    data["collection_time"] = float(try_match(r"Finished collecting colours in ([\d.]+)s", -1))
    data["construction_time"] = float(try_match(r"Finished constructing features in ([\d.]+)s", -1))
    data["training_time"] = float(try_match(r"Finished training model in ([\d.]+)s", -1))

    return data


def get_plan_df():
    data = {k: [] for k in PLAN_DF_KEYS}

    for config in tqdm(
        list(
            product(
                DOMAINS,
                FEATURES,
                PRUNING,
                OPTIMISERS,
                DATA_GENERATION,
                ITERATIONS,
                PROBLEMS,
                REPEATS,
            )
        )
    ):
        log_path = f"{LOG_PLAN_DIR}/{'_'.join(config)}.log"
        config_data = parse_plan_log(log_path)
        for k, v in config_data.items():
            data[k].append(v)
        data["problem"].append(config[-2])
        data["repeat"].append(config[-1])
        for i, k in enumerate(CONFIG_KEYS):
            data[k].append(config[i])

    df = pd.DataFrame(data)
    return df


def get_train_df():
    data = {k: [] for k in TRAIN_DF_KEYS}
    for config in tqdm(
        list(product(DOMAINS, FEATURES, PRUNING, OPTIMISERS, DATA_GENERATION, ITERATIONS, REPEATS))
    ):
        log_path = f"{LOG_TRAIN_DIR}/{'_'.join(config)}.log"
        config_data = parse_train_log(log_path)
        for k, v in config_data.items():
            data[k].append(v)
        for i, k in enumerate(CONFIG_KEYS):
            data[k].append(config[i])

    df = pd.DataFrame(data)
    return df
