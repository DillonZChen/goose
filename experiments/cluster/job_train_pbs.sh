#!/bin/bash
#PBS -l mem=32gb
#PBS -l walltime=03:00:00
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

# run command
$CMD

date
