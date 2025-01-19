#!/bin/bash

arg=$1

DOMAIN=bw-small
# DOMAIN=blocksworld
f=lwl2
L=2
p=collapse-all
if [[ $1 != "" ]]; then
    p=$1
fi
d=plan
multiset_hash=1
facts=fd
save_file=experiments/_short/model/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.model
log_file=experiments/_short/train/${DOMAIN}_${f}_${L}_${p}_${d}_${multiset_hash}_${facts}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/train

if [ $DOMAIN == "bw-small" ]; then
    CONFIG_PATH=experiments/bw-small/config.toml
else
    CONFIG_PATH=configurations/data/ipc23lt/$DOMAIN.toml
fi

python3 train.py $CONFIG_PATH \
    -f $f \
    -L $L \
    -p $p \
    -d $d \
    --multiset_hash $multiset_hash \
    --facts $facts \
    -s $save_file | tee $log_file

echo "===================================================="
echo $save_file
echo $log_file
echo "===================================================="
