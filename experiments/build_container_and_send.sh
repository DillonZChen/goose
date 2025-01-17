#!/bin/bash

t_start=$(date +%s)
apptainer build Goose.sif Goose.def
t_end=$(date +%s)
t_duration=$((t_end - t_start))
./experiments/send_sif_gadi.sh
./experiments/send_sif_pfcalcul.sh 

# Print timing results
echo "----------------------------------------"
echo "Apptainer build time=: $t_duration seconds"
echo "----------------------------------------"
