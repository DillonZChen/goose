#!/bin/bash

# optional arg [wlplan|planners|""]
arg=$1

# Show commands
set -x

# Exit on the first error
set -e

# Install Python wlplan interface
if [ "$arg" != "planners" ]; then
    cd wlplan/
    mkdir -p _wlplan/
    pip install .
    cd ..
fi

# Build c++ wlplan interface and planners
if [ "$arg" != "wlplan" ]; then
    # Install c++ wlplan interfaces
    cd wlplan/
    for planner in powerlifted downward; do
        install_dir=../planning/$planner/src/search/ext/wlplan
        rm -rf $install_dir
        python3 cmake_build.py $install_dir
    done
    cd ..

    # Build planners
    for planner in powerlifted downward; do
        cd planning/$planner
        if [ "$arg" = "fresh"]; then
            rm -r builds
        fi
        python3 build.py
        cd ../..
    done
fi
