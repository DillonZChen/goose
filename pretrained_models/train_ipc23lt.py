#!/usr/bin/env python

import subprocess
from datetime import datetime

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
        "train.py",
        f"configurations/data/ipc23lt/{domain}.toml",
        "configurations/model/classic.toml",
        "-s",
        f"ipc23lt-{domain}.model",
    ]
    print("*" * 80)
    print(domain)
    print("*" * 80)
    print("Current time:", datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    print(" ".join(cmd))
    subprocess.run(
        cmd,
        check=True,
        capture_output=True,
        text=True,
    )
