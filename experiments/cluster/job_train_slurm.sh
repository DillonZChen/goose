#!/bin/bash
#SBATCH --mem=16gb
#SBATCH --time=03:00:00

# Show commands
set -x

# set to english
export LC_ALL=C

# log some hardware stats
pwd; hostname; lscpu; date; 

# run command
$CMD

date
