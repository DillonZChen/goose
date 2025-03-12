#!/bin/bash

set -e
# set -x

cd wlplan

# Store start time for pip install
pip_start=$(date +%s)
pip install . -v
pip_end=$(date +%s)
pip_duration=$((pip_end - pip_start))

cd -

# Store start time for training
train_start=$(date +%s)
./experiments/train.sh
train_end=$(date +%s)
train_duration=$((train_end - train_start))

# Print timing results
echo "----------------------------------------"
echo "Execution Time Summary:"
echo "WLPlan installation time: $pip_duration seconds"
echo "Training script time: $train_duration seconds"
echo "Total time: $((pip_duration + train_duration)) seconds"
echo "----------------------------------------"
