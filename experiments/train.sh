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
log_file=experiments/_short/train/${DOMAIN}_${f}_${L}_${p}_${d}_${optimisation}_${multiset_hash}_${facts}.log
mkdir -p experiments/_short/model
mkdir -p experiments/_short/train

if [ $DOMAIN == "bw-small" ]; then
    CONFIG_PATH=experiments/bw-small/config.toml
elif [ $DOMAIN == "blocksworld-hbf" ]; then
    CONFIG_PATH=configurations/data/hbf/blocksworld.toml
elif [ $DOMAIN == "warehouse-hbf" ]; then
    CONFIG_PATH=configurations/data/hbf/warehouse.toml
else 
    CONFIG_PATH=configurations/data/ipc23lt/$DOMAIN.toml
fi

python3 train.py $CONFIG_PATH \
    -f $f \
    -L $L \
    -fp $p \
    -d $d \
    --multiset_hash $multiset_hash \
    --facts $facts \
    --optimisation $optimisation \
    -s $save_file | tee $log_file

echo "===================================================="
echo $save_file
echo $log_file
echo "===================================================="
