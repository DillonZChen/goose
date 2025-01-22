#!/bin/bash
#SBATCH --mem=8gb
#SBATCH --time=00:05:00

# Show commands
set -x

# set to english
export LC_ALL=C

# log some hardware stats
pwd; hostname; lscpu; date; 

# run command
cd $TMP_DIR
apptainer run --bind /pfcalcul/work/dchen/code/goose:/pfcalcul/work/dchen/code/goose /pfcalcul/work/dchen/code/goose/scorpion.sif --translate $DOMAIN_PDDL $PROBLEM_PDDL
cp output.sas $TAR_FILE
cd ..
rm -rf $TMP_DIR

date
