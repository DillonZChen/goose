#!/bin/bash
#PBS -l mem=8gb
#PBS -l walltime=00:05:15
#PBS -P cd85
#PBS -q normal
#PBS -l wd
#PBS -M dongbang4204@gmail.com

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
