#!/bin/bash
#SBATCH --mem=4gb
#SBATCH --time=00:01:00

# Show commands
set -x

# set to english
export LC_ALL=C

# make tmp dir
mkdir -p $TMP_DIR
cd $TMP_DIR

# run command
$CMD

# clean up
rm -rf $TMP_DIR

date
