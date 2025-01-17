#!/bin/bash

DOMAIN=blocksworld
f=wl
L=4
p=collapse-all
d=plan
multiset_hash=1
facts=fd
save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.model
log_file=experiments/_short/plan/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/plan

PROBLEM=1_01

domain_pddl=benchmarks/ipc23lt/$DOMAIN/domain.pddl
problem_pddl=benchmarks/ipc23lt/$DOMAIN/testing/p${PROBLEM}.pddl

python3 plan.py $domain_pddl $problem_pddl $save_file | tee $log_file
