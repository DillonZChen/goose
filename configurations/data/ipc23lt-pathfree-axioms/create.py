#!/usr/bin/env python

import itertools
import os

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
os.system(f"rm -rf {_CUR_DIR}/*.toml")

DOMAINS = [
    "rovers",
    "sokoban",
    "transport",
]


for domain in DOMAINS:
    file = f"{_CUR_DIR}/{domain}.toml"
    with open(file, "w") as f:
        f.write(f"domain_pddl = 'benchmarks/ipc23lt-pathfree-axioms/{domain}/domain.pddl'\n")
        f.write(f"tasks_dir = 'benchmarks/ipc23lt-pathfree-axioms/{domain}/training'\n")
        f.write(f"plans_dir = 'benchmarks/ipc23lt-pathfree-axioms/{domain}/training_plans'\n")
