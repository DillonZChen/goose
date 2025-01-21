#!/bin/bash

arg=$1

# DOMAIN=bw-small  # don't call this for planning
DOMAIN=blocksworld
f=wl
L=4
p=none
if [[ $1 != "" ]]; then
    p=$1
fi
d=plan
multiset_hash=1
facts=fd

save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.model
log_file=experiments/_short/plan/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/plan

PROBLEM=1_12

domain_pddl=benchmarks/ipc23lt/$DOMAIN/domain.pddl
problem_pddl=benchmarks/ipc23lt/$DOMAIN/testing/p${PROBLEM}.pddl

python3 plan.py $domain_pddl $problem_pddl $save_file | tee $log_file

# wl, 4, none pruning
# blocksworld 1_12
# [t=6.038954s, 76116 KB] Plan length: 334 step(s).
# [t=6.038954s, 76116 KB] Plan cost: 334
# [t=6.038954s, 76116 KB] Expanded 1193 state(s).
# [t=6.038954s, 76116 KB] Reopened 0 state(s).
# [t=6.038954s, 76116 KB] Evaluated 29499 state(s).
# [t=6.038954s, 76116 KB] Evaluations: 29499
# [t=6.038954s, 76116 KB] Generated 31064 state(s).
# [t=6.038954s, 76116 KB] Dead ends: 0 state(s).
# [t=6.038954s, 76116 KB] Number of registered states: 29499
# [t=6.038954s, 76116 KB] Int hash set load factor: 29499/65536 = 0.450119
# [t=6.038954s, 76116 KB] Int hash set resizes: 16
# [t=6.038954s, 76116 KB] Search time: 5.952955s
# [t=6.038954s, 76116 KB] Total time: 6.038954s
