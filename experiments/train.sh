#!/bin/bash

DOMAIN=blocksworld
f=wl
L=4
p=collapse-layer
d=plan
save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}.model
log_file=experiments/_short/train/${DOMAIN}_${f}_${L}_${p}_${d}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/train

python3 train.py configurations/data/ipc23lt/$DOMAIN.toml \
    -f $f \
    -L $L \
    -p $p \
    -d $d \
    -s $save_file | tee $log_file
