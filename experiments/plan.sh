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
# [t=5.922040s, 76116 KB] Plan length: 334 step(s).
# [t=5.922040s, 76116 KB] Plan cost: 334
# [t=5.922040s, 76116 KB] Expanded 1193 state(s).
# [t=5.922040s, 76116 KB] Reopened 0 state(s).
# [t=5.922040s, 76116 KB] Evaluated 29499 state(s).
# [t=5.922040s, 76116 KB] Evaluations: 29499
# [t=5.922040s, 76116 KB] Generated 31064 state(s).
# [t=5.922040s, 76116 KB] Dead ends: 0 state(s).
# [t=5.922040s, 76116 KB] Number of registered states: 29499
# [t=5.922040s, 76116 KB] Int hash set load factor: 29499/65536 = 0.450119
# [t=5.922040s, 76116 KB] Int hash set resizes: 16
# [t=5.922040s, 76116 KB] Search time: 5.837055s
# [t=5.922040s, 76116 KB] Total time: 5.922040s

# wl, 4, collapse-all pruning
# blocksworld 1_12
# [t=3.014065s, 73820 KB] Plan length: 316 step(s).
# [t=3.014065s, 73820 KB] Plan cost: 316
# [t=3.014065s, 73820 KB] Expanded 682 state(s).
# [t=3.014065s, 73820 KB] Reopened 0 state(s).
# [t=3.014065s, 73820 KB] Evaluated 17777 state(s).
# [t=3.014065s, 73820 KB] Evaluations: 17777
# [t=3.014065s, 73820 KB] Generated 18465 state(s).
# [t=3.014065s, 73820 KB] Dead ends: 0 state(s).
# [t=3.014065s, 73820 KB] Number of registered states: 17777
# [t=3.014065s, 73820 KB] Int hash set load factor: 17777/32768 = 0.542511
# [t=3.014065s, 73820 KB] Int hash set resizes: 15
# [t=3.014065s, 73820 KB] Search time: 2.923480s
# [t=3.014065s, 73820 KB] Total time: 3.014065s

# wl, 4, collapse-layer pruning
# blocksworld 0_20
# [t=0.044998s, 13740 KB] Plan length: 82 step(s).
# [t=0.044998s, 13740 KB] Plan cost: 82
# [t=0.044998s, 13740 KB] Expanded 167 state(s).
# [t=0.044998s, 13740 KB] Reopened 0 state(s).
# [t=0.044998s, 13740 KB] Evaluated 1053 state(s).
# [t=0.044998s, 13740 KB] Evaluations: 1053
# [t=0.044998s, 13740 KB] Generated 1220 state(s).
# [t=0.044998s, 13740 KB] Dead ends: 0 state(s).
# [t=0.044998s, 13740 KB] Number of registered states: 1053
# [t=0.044998s, 13740 KB] Int hash set load factor: 1053/2048 = 0.514160
# [t=0.044998s, 13740 KB] Int hash set resizes: 11
# [t=0.044998s, 13740 KB] Search time: 0.036000s
# [t=0.044998s, 13740 KB] Total time: 0.044998s
