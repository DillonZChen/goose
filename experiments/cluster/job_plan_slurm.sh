#!/bin/bash
#SBATCH --mem=8gb
#SBATCH --time=00:05:15

# Show commands
set -x

# set to english
export LC_ALL=C

# log some hardware stats
pwd; hostname; lscpu; date; 

# make tmp dir
mkdir -p $TMP_DIR
cd $TMP_DIR

# run command
$CMD

# clean up
rm -rf $TMP_DIR

date
