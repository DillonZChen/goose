#!/bin/bash

arg=$1

DOMAIN=bw-small
DOMAIN=blocksworld
f=wl
L=4
p=collapse-layer-f
if [[ $1 != "" ]]; then
    p=$1
fi
d=plan
multiset_hash=1
facts=fd
optimisation=svr
planner=fd

save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}_${optimisation}_${multiset_hash}_${facts}.model
log_file=experiments/_short/plan/${DOMAIN}_${f}_${L}_${p}_${d}_${optimisation}_${multiset_hash}_${facts}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/plan

PROBLEM=1_12

if [ $DOMAIN == "blocksworld-hbf" ]; then
    domain_pddl=benchmarks/hbf/blocksworld/domain.pddl
    problem_pddl=benchmarks/hbf/blocksworld/testing/p${PROBLEM}.pddl
elif [ $DOMAIN == "warehouse-hbf" ]; then
    domain_pddl=benchmarks/hbf/warehouse/domain.pddl
    problem_pddl=benchmarks/hbf/warehouse/testing/p${PROBLEM}.pddl
else 
    domain_pddl=benchmarks/ipc23lt/$DOMAIN/domain.pddl
    problem_pddl=benchmarks/ipc23lt/$DOMAIN/testing/p${PROBLEM}.pddl
fi

python3 plan.py $domain_pddl $problem_pddl $save_file -p $planner | tee $log_file

# wl, 4, none pruning
# blocksworld 1_12
# [t=5.391743s, 76116 KB] Plan length: 324 step(s).
# [t=5.391743s, 76116 KB] Plan cost: 324
# [t=5.391743s, 76116 KB] Expanded 942 state(s).
# [t=5.391743s, 76116 KB] Reopened 0 state(s).
# [t=5.391743s, 76116 KB] Evaluated 25247 state(s).
# [t=5.391743s, 76116 KB] Evaluations: 25247
# [t=5.391743s, 76116 KB] Generated 26286 state(s).
# [t=5.391743s, 76116 KB] Dead ends: 0 state(s).
# [t=5.391743s, 76116 KB] Number of registered states: 25247
# [t=5.391743s, 76116 KB] Int hash set load factor: 25247/32768 = 0.770477
# [t=5.391743s, 76116 KB] Int hash set resizes: 15
# [t=5.391743s, 76116 KB] Search time: 5.306756s
# [t=5.391743s, 76116 KB] Total time: 5.391743s

# wl, 4, collapse-all pruning
# [t=2.491777s, 56984 KB] Plan length: 302 step(s).
# [t=2.491777s, 56984 KB] Plan cost: 302
# [t=2.491777s, 56984 KB] Expanded 509 state(s).
# [t=2.491777s, 56984 KB] Reopened 0 state(s).
# [t=2.491777s, 56984 KB] Evaluated 12396 state(s).
# [t=2.491777s, 56984 KB] Evaluations: 12396
# [t=2.491777s, 56984 KB] Generated 12903 state(s).
# [t=2.491777s, 56984 KB] Dead ends: 0 state(s).
# [t=2.491777s, 56984 KB] Number of registered states: 12396
# [t=2.491777s, 56984 KB] Int hash set load factor: 12396/16384 = 0.756592
# [t=2.491777s, 56984 KB] Int hash set resizes: 14
# [t=2.491777s, 56984 KB] Search time: 2.426787s
# [t=2.491777s, 56984 KB] Total time: 2.491777s

# wl, 4, collapse-all-x pruning
# [t=2.511108s, 56796 KB] Plan length: 310 step(s).
# [t=2.511108s, 56796 KB] Plan cost: 310
# [t=2.511108s, 56796 KB] Expanded 565 state(s).
# [t=2.511108s, 56796 KB] Reopened 0 state(s).
# [t=2.511108s, 56796 KB] Evaluated 13381 state(s).
# [t=2.511108s, 56796 KB] Evaluations: 13381
# [t=2.511108s, 56796 KB] Generated 13959 state(s).
# [t=2.511108s, 56796 KB] Dead ends: 0 state(s).
# [t=2.511108s, 56796 KB] Number of registered states: 13381
# [t=2.511108s, 56796 KB] Int hash set load factor: 13381/16384 = 0.816711
# [t=2.511108s, 56796 KB] Int hash set resizes: 14
# [t=2.511108s, 56796 KB] Search time: 2.436418s
# [t=2.511108s, 56796 KB] Total time: 2.511108s
