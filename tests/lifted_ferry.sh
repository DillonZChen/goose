#!/bin/bash

DOMAIN=ferry
MODEL=$DOMAIN.model

set -e

rm -f $MODEL

python3 train.py configurations/model/h-wlf4.toml configurations/data/$DOMAIN-classic.toml --save_file $MODEL

python3 plan.py benchmarks/$DOMAIN/classic/domain.pddl benchmarks/$DOMAIN/classic/testing/p1_15.pddl $MODEL --lifted
