#!/usr/bin/env python

import os
import subprocess
from datetime import datetime


CUR_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.dirname(CUR_DIR)

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

for domain in DOMAINS:
    cmd = [
        "python3",
        f"{ROOT_DIR}/train.py",
        f"{ROOT_DIR}/configurations/data/ipc23lt/{domain}.toml",
        f"{ROOT_DIR}/configurations/classic.toml",
        "-s",
        f"{CUR_DIR}/ipc23lt-{domain}.model",
    ]
    print("*" * 80)
    print(domain)
    print("*" * 80)
    print("Current time:", datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    print(" ".join(cmd))
    out = subprocess.run(
        cmd,
        check=True,
        capture_output=True,
        text=True,
    )
    with open(f"{CUR_DIR}/ipc23lt-{domain}.log", "w") as f:
        f.write(out.stdout)
        f.write(out.stderr)
