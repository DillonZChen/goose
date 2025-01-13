#!/usr/bin/env python

import argparse
import os
import subprocess
from itertools import product

from tqdm import tqdm

DOMAINS = [
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
DATA_GENERATIONS = ["plan", "state-space"]
PRUNING = ["none", "collapse"]

_CUR_DIR = os.path.dirname(os.path.realpath(__file__))
_LOG_DIR = f"{_CUR_DIR}/_collect_logs"
os.makedirs(_LOG_DIR, exist_ok=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--clear_logs", action="store_true")
    args = parser.parse_args()

    if args.clear_logs:
        os.system(f"rm -rf {_LOG_DIR}")
        print(f"Deleted {_LOG_DIR}")
        exit(0)

    try:
        pbar = tqdm(list(product(DOMAINS, DATA_GENERATIONS, PRUNING)))
        for domain, data_generation, pruning in pbar:
            description = f"{domain}_{data_generation}_{pruning}"
            pbar.set_description(description)
            log_file = f"{_LOG_DIR}/{description}.log"
            if os.path.exists(log_file):
                with open(log_file, "r") as f:
                    if "Exiting after collecting colours." in f.read():
                        continue
            subprocess.run(
                [
                    "python3",
                    "train.py",
                    f"configurations/data/ipc23lt/{domain}.toml",
                    "-d",
                    data_generation,
                    "-p",
                    pruning,
                    "--collect_only",
                ],
                stdout=open(log_file, "w"),
                stderr=subprocess.STDOUT,
            )
    except KeyboardInterrupt:
        print("KeyboardInterrupt")
        exit(0)
