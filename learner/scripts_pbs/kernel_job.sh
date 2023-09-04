#!/bin/bash
 
#PBS -P sv11
#PBS -q normal
#PBS -l walltime=00:33:00
#PBS -l ncpus=2
#PBS -l mem=8GB
#PBS -l jobfs=20GB
#PBS -l wd

module load python3
source /scratch/sv11/dc6693/goose_env/bin/activate

echo python3 run.py $DOM_PATH $INS_PATH kernel-opt -m $MODEL_PATH -p fd -s gbfs -t $TIMEOUT --aux-file $AUX_FILE --plan-file $PLAN_FILE
python3 run.py $DOM_PATH $INS_PATH kernel-opt -m $MODEL_PATH -p fd -s gbfs -t $TIMEOUT --aux-file $AUX_FILE --plan-file $PLAN_FILE

rm $LOCK_FILE