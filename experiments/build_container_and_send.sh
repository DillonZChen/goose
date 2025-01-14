#!/bin/bash

apptainer build Goose.sif Goose.def
./experiments/send_sif_gadi.sh
./experiments/send_sif_pfcalcul.sh 
