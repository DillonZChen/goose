#!/bin/bash

PROBLEM=1_02
DOMAIN=blocksworld
f=wl
L=4
p=collapse-layer
d=plan
save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}.model
log_file=experiments/_short/plan/${DOMAIN}_${f}_${L}_${p}_${d}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/plan

domain_pddl=benchmarks/ipc23lt/$DOMAIN/domain.pddl
problem_pddl=benchmarks/ipc23lt/$DOMAIN/testing/p${PROBLEM}.pddl

python3 plan.py $domain_pddl $problem_pddl $save_file | tee $log_file
