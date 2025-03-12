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
plan_start=$(date +%s)
./experiments/plan.sh
plan_end=$(date +%s)
plan_duration=$((plan_end - plan_start))

# Print timing results
echo "----------------------------------------"
echo "Execution Time Summary:"
echo "WLPlan installation time: $pip_duration seconds"
echo "Planning script time: $plan_duration seconds"
echo "Total time: $((pip_duration + plan_duration)) seconds"
echo "----------------------------------------"
