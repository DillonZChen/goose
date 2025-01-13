#!/bin/bash

# directory of this file
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

rsync -avz $DIR/_models/pfcalcul/ gadi:/scratch/cd85/dc6693/goose/experiments/_models/
