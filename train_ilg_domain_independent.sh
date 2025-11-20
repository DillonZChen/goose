#!/bin/bash

set +e
set +x

# 1. create environment 
if [ ! -f "goose_aaai24.sif" ]; then
    apptainer build goose_aaai24.sif goose_gpu.def
fi

# 2. build planner
if [ ! -d "planners/downward/builds/" ]; then
    cd planners/downward
    ../../goose_aaai24.sif python3 build.py
    cd -
fi

# 3. train
./goose_aaai24.sif python3 learner/train_gnn.py ipc -r llg --aggr max -L 8 --epochs 100000
