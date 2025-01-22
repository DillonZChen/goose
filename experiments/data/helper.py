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

LOG_PLAN_DIR = os.path.normpath(f"{CUR_DIR}/../_log_plan")
LOG_TRAIN_DIR = os.path.normpath(f"{CUR_DIR}/../_log_train")

PLOT_DIR = os.path.normpath(f"{CUR_DIR}/../_plots")
os.makedirs(PLOT_DIR, exist_ok=True)

with open(f"{ROOT_DIR}/experiments/config.json") as f:
    CONFIG = json.load(f)

DOMAINS = CONFIG["domains"]
FEATURES = CONFIG["features"]
FEATURE_PRUNING = CONFIG["feature_pruning"]
DATA_PRUNING = CONFIG["data_pruning"]
OPTIMISERS = CONFIG["optimisers"]
DATA_GENERATION = CONFIG["data_generation"]
FACTS = CONFIG["facts"]
ITERATIONS = [str(i) for i in CONFIG["iterations"]]
REPEATS = [str(i) for i in range(CONFIG["repeats"])]

FEATURE_GENERATION_PREFIX = {
    "none": 0,
    "collapse-all": 9,
    "collapse-all-x": 1,
    "collapse-layer": 2,
    "collapse-layer-x": 3,
}

PROBLEMS = []
PROBLEMS = sorted([f"{x}_{y:02d}" for y in range(3, 31, 3) for x in [0, 1, 2]])
PROBLEMS += sorted([f"{x}_{y:02d}" for y in range(2, 31, 3) for x in [0, 1, 2]])
PROBLEMS += sorted([f"{x}_{y:02d}" for y in range(1, 31, 3) for x in [0, 1, 2]])
PROBLEMS = sorted(PROBLEMS)

TIMEOUT = 300

CONFIG_KEYS = [
    "domain",
    "features",
    "feature_pruning",
    "data_pruning",
    "optimiser",
    "data_generation",
    "facts",
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
    "cpu",
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
    "mse_loss",
    "f1_macro",
    "mean_accuracy",
]


def _get_default_plan_data():
    return {
        "tried": False,
        "solved": False,
        "expanded": -1,
        "plan_length": -1,
        "runtime": TIMEOUT,
        "cpu": "",
    }


def parse_plan_log(log_path: str):
    data = _get_default_plan_data()

    if not os.path.exists(log_path):
        return data
    with open(log_path, "r") as file:
        content = file.read()

    # Model name:                           Intel(R) Xeon(R) CPU E5-2695 v4 @ 2.10GHz
    cpu = re.search(r"Model name:\s+(.*)", content)
    if cpu:
        cpu = cpu.group(1).strip()

    try:
        solved = "Solution found" in content
        if solved:
            # [t=0.016241s, 10888 KB] Plan length: 10 step(s).
            # [t=0.016241s, 10888 KB] Expanded 13 state(s).
            # INFO     Planner time: 0.15s
            plan_length = re.search(r"Plan length: (\d+) step\(s\)", content)
            expanded = re.search(r"Expanded (\d+) state\(s\)", content)
            runtime = re.search(r"Planner time: ([\d.]+)s", content)
            if runtime is None:
                # Total time: 0.002756 (for Powerlifted)
                runtime = re.search(r"Total time: ([\d.]+)", content)
            data["solved"] = True
            data["plan_length"] = int(plan_length.group(1))
            data["expanded"] = int(expanded.group(1))
            data["runtime"] = float(runtime.group(1))
            if data["runtime"] > TIMEOUT:
                data = _get_default_plan_data()
        data["tried"] = True
    except Exception as e:
        print(f"Error parsing {log_path}: {e}")
    data["cpu"] = cpu

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
        "mse_loss": -1,
        "f1_macro": -1,
        "mean_accuracy": -1,
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
    data["n_colours"] = int(try_match(r"X.shape=\(\d+, (\d+)\)", -1))
    data["n_data"] = int(try_match(r"X.shape=\((\d+),", -1))
    data["collection_time"] = float(try_match(r"Finished collecting colours in ([\d.]+)s", -1))
    data["construction_time"] = float(try_match(r"Finished constructing features in ([\d.]+)s", -1))
    data["training_time"] = float(try_match(r"Finished training model in ([\d.]+)s", -1))
    data["mse_loss"] = float(try_match(r"mse_loss=([\d.]+)", -1))
    data["f1_macro"] = float(try_match(r"f1_macro=([\d.]+)", -1))
    data["mean_accuracy"] = float(try_match(r"mean_accuracy=([\d.]+)", -1))

    return data


def get_plan_df(cluster: str):
    data = {k: [] for k in PLAN_DF_KEYS}

    for config in tqdm(
        list(
            product(
                DOMAINS,
                FEATURES,
                FEATURE_PRUNING,
                DATA_PRUNING,
                OPTIMISERS,
                DATA_GENERATION,
                FACTS,
                ITERATIONS,
                PROBLEMS,
                REPEATS,
            )
        )
    ):
        log_path = f"{LOG_PLAN_DIR}/{cluster}/{'_'.join(config)}.log"
        config_data = parse_plan_log(log_path)
        for k, v in config_data.items():
            data[k].append(v)
        data["problem"].append(config[-2])
        data["repeat"].append(config[-1])
        for i, k in enumerate(CONFIG_KEYS):
            value = config[i]
            if k == "feature_pruning":
                value = str(FEATURE_GENERATION_PREFIX[config[i]]) + value
            data[k].append(value)

    df = pd.DataFrame(data)
    return df


def get_train_df():
    data = {k: [] for k in TRAIN_DF_KEYS}
    for config in tqdm(
        list(
            product(
                DOMAINS,
                FEATURES,
                FEATURE_PRUNING,
                DATA_PRUNING,
                OPTIMISERS,
                DATA_GENERATION,
                FACTS,
                ITERATIONS,
                REPEATS,
            )
        )
    ):
        log_path = f"{LOG_TRAIN_DIR}/{'_'.join(config)}.log"
        config_data = parse_train_log(log_path)
        for k, v in config_data.items():
            data[k].append(v)
        for i, k in enumerate(CONFIG_KEYS):
            data[k].append(config[i])

    df = pd.DataFrame(data)
    return df
