#!/bin/bash

arg=$1

# check optional arg is valid and show usage
if [ -n "$arg" ]; then
    case "$arg" in
    wlplan | planners | fd | nfd | pwl | fresh)
        # Valid argument, continue
        ;;
    *)
        echo "Usage: $0 [wlplan|planners|fd|nfd|pwl|fresh]"
        exit 1
        ;;
    esac
fi

# planners
PLANNERS_DIR="planning/ext"
PLANNERS="downward powerlifted numeric-downward"

if [ "$arg" = "fd" ]; then
    PLANNERS="downward"
elif [ "$arg" = "nfd" ]; then
    PLANNERS="numeric-downward"
elif [ "$arg" = "pwl" ]; then
    PLANNERS="powerlifted"
fi

# Show commands
set -x

# Exit on the first error
set -e

# Install Python wlplan interface
if [ "$arg" != "planners" ]; then
    cd wlplan/
    mkdir -p _wlplan/
    pip install . -v
    cd -
fi

# Build c++ wlplan interface and planners
if [ "$arg" != "wlplan" ]; then
    # Install c++ wlplan interfaces
    cd wlplan/
    for planner in $PLANNERS; do
        install_dir=../$PLANNERS_DIR/$planner/src/search/ext/wlplan
        rm -rf $install_dir
        python3 cmake_build.py $install_dir
    done
    cd -

    # Build planners
    for planner in $PLANNERS; do
        cd $PLANNERS_DIR/$planner
        if [ "$arg" = "fresh" ]; then
            rm -r builds
        fi
        if [ -f "compile.sh" ]; then
            ./compile.sh # numeric-downward
        else
            python3 build.py
        fi
        cd -
    done
fi
