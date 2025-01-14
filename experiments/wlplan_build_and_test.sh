#!/bin/bash

set -e
set -x

./build.sh
cd wlplan
pytest
cd -
