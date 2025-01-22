#!/bin/bash

arg=$1

DOMAIN=bw-small
DOMAIN=blocksworld
f=wl
L=4
p=collapse-all
if [[ $1 != "" ]]; then
    p=$1
fi
d=plan
multiset_hash=1
facts=fd
optimisation=svr
planner=fd

save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}_${optimisation}_${multiset_hash}_${facts}.model
log_file=experiments/_short/train/${DOMAIN}_${f}_${L}_${p}_${d}_${optimisation}_${multiset_hash}_${facts}.log
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

# wl, 4, collapse-all pruning
# [t=6.308636s, 72804 KB] Plan length: 352 step(s).
# [t=6.308636s, 72804 KB] Plan cost: 352
# [t=6.308636s, 72804 KB] Expanded 1164 state(s).
# [t=6.308636s, 72804 KB] Reopened 0 state(s).
# [t=6.308636s, 72804 KB] Evaluated 29825 state(s).
# [t=6.308636s, 72804 KB] Evaluations: 29825
# [t=6.308636s, 72804 KB] Generated 31173 state(s).
# [t=6.308636s, 72804 KB] Dead ends: 0 state(s).
# [t=6.308636s, 72804 KB] Number of registered states: 29825
# [t=6.308636s, 72804 KB] Int hash set load factor: 29825/32768 = 0.910187
# [t=6.308636s, 72804 KB] Int hash set resizes: 15
# [t=6.308636s, 72804 KB] Search time: 6.235496s
# [t=6.308636s, 72804 KB] Total time: 6.308636s
