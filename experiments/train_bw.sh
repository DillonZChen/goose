#!/bin/bash

python3 train.py configurations/data/ipc23lt/blocksworld.toml -mc configurations/model/wl/wl_gpr_4.toml -s tests/models/blocksworld.model -f iwl
