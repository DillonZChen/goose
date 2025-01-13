#!/bin/bash

# directory of this file
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

scp $DIR/../Goose.sif gadi:/scratch/cd85/dc6693/goose/Goose.sif
