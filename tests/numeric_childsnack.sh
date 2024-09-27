#!/bin/bash

python3 train.py configurations/model/r-wlf1.toml configurations/data/childsnack-numeric.toml --save_file childsnack.model
python3 plan.py benchmarks/childsnack/numeric/domain.pddl benchmarks/childsnack/numeric/testing/p2_30.pddl childsnack.model
