#!/bin/bash

python3 train.py configurations/data/ipc23lt/blocksworld.toml \
    -f wl \
    -L 4 \
    -p collapse \
    -d plan \
    --collect_only
