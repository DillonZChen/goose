#!/bin/bash

python3 train.py configurations/model/r-wlf1.toml configurations/data/spanner-numeric.toml --save_file spanner.model
python3 plan.py benchmarks/spanner/numeric/domain.pddl benchmarks/spanner/numeric/testing/p2_30.pddl spanner.model
