#!/usr/bin/env python

import os

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
os.system(f"rm -rf {_CUR_DIR}/*.toml")

DOMAINS = [
    "blocksworld",
    "childsnack",
    "ferry",
    "miconic",
    "rovers",
    "satellite",
    "spanner",
    "transport",
]


for domain in DOMAINS:
    file = f"{_CUR_DIR}/{domain}.toml"
    with open(file, "w") as f:
        f.write(f"domain_pddl = 'benchmarks/neurips24/{domain}/domain.pddl'\n")
        f.write(f"tasks_dir = 'benchmarks/neurips24/{domain}/training'\n")
        f.write(f"plans_dir = 'benchmarks/neurips24/{domain}/training_plans'\n")
