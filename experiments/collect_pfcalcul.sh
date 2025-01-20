#!/bin/bash

# directory of this file
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

rsync -avz pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/_log_train/pfcalcul/ $DIR/_log_train/
rsync -avz pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/_log_plan/pfcalcul/ $DIR/_log_plan/pfcalcul/
rsync -avz pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/_models/pfcalcul/ $DIR/_models/
