#!/bin/bash

# directory of this file
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

scp $DIR/../Goose.sif pfcalcul:/pfcalcul/work/dchen/code/goose/Goose.sif
