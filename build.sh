#!/bin/bash

# optional arg [wlplan|planners|""]
arg=$1

# planners
PLANNERS="downward powerlifted numeric-downward"

# Show commands
set -x

# Exit on the first error
set -e

# Install Python wlplan interface
if [ "$arg" != "planners" ]; then
    cd wlplan/
    mkdir -p _wlplan/
    pip install . -v
    cd ..
fi

# Build c++ wlplan interface and planners
if [ "$arg" != "wlplan" ]; then
    # Install c++ wlplan interfaces
    cd wlplan/
    for planner in $PLANNERS; do
        install_dir=../planning/$planner/src/search/ext/wlplan
        rm -rf $install_dir
        python3 cmake_build.py $install_dir
    done
    cd ..

    # Build planners
    for planner in $PLANNERS; do
        cd planning/$planner
        if [ "$arg" = "fresh"]; then
            rm -r builds
        fi
        if [ -f "compile.sh" ]; then
            ./compile.sh  # numeric-downward
        else
            python3 build.py
        fi
        cd ../..
    done
fi
