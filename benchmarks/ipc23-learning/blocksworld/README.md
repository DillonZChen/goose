# Blocksworld

[Original PDDL generator](https://github.com/AI-Planning/pddl-generators/tree/main/blocksworld)

Our generator:

_Blocksworld Training_
`python blocksworld.py -f 2 -s 1 -t 101 -o training/easy/`

_Blocksworld Testing_
```shell
python blocksworld.py -f 5 -s 3 -t 92 -o testing/easy/
python blocksworld.py -f 95 -s 1 -t 124 -o testing/medium/
python blocksworld.py -f 125 -s 33 -t 1082 -o testing/hard/
```

Comments:
- pddl-generators repository limited to 1000 (since the generator is quadratic in memory), although all states have the same probability to be in the init / goal. The generator needs a code update to get to a new limit in the max number blocks and fix a seed for reproducibility.
- Since we want to go beyond this value, we discard the idea of generating any state with the same probability, and process all randomly shuffled blocks in sequence with a probability of 10% of a block to belong to a new tower. 
- In case we want to implement a uniform sampling of states, we need to first draw k ~ binom(n,k)/2^n for all k in [1,n], then random shuffle all blocks, the first k blocks will be ontable, the rest will be placed on top of the a random.choice block between towers 1 and k included.
- Autoscale - Agile/Satisficing: easier (at most ~300s) are around 100 blocks; medium are up to 125 (~1-2h); hard are above 125 blocks


