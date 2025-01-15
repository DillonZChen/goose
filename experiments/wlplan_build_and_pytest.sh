#!/bin/bash

set -e
set -x

cd wlplan
pip install .
pytest
cd -
