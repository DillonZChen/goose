#!/bin/bash

# directory of this file
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

rsync -avz gadi:/scratch/cd85/dc6693/goose/experiments/_log_plan/gadi/ $DIR/_log_plan/gadi/
rsync -avz gadi:/scratch/cd85/dc6693/goose/experiments/_plans/ $DIR/_plans/
