#!/bin/bash

DOMAIN=blocksworld
f=wl
L=4
p=collapse-all
d=plan
multiset_hash=1
facts=fd
save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.model
log_file=experiments/_short/train/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/train

python3 train.py configurations/data/ipc23lt/$DOMAIN.toml \
    -f $f \
    -L $L \
    -p $p \
    -d $d \
    --multiset_hash $multiset_hash \
    --facts $facts \
    -s $save_file | tee $log_file
