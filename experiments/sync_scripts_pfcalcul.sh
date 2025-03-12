#!/bin/bash

# directory of this file
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# rsync -avz $DIR/cluster/ pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/cluster/
# rsync -avz $DIR/send_* pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/
# rsync -avz $DIR/config.json pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/config.json

rsync -avz --exclude '_*/' $DIR/ pfcalcul:/pfcalcul/work/dchen/code/goose/experiments/
