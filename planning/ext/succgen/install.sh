#!/bin/bash

# Exit on the first error
set -e

# Build with all threads
export MAKEFLAGS="-j$(nproc)"

# Install the package from sources
mkdir -p _succgen
pip install . -v

# Make sure required tools are installed
pip install pybind11-stubgen

# Generate stubs
rm -rf _succgen/*.pyi
pybind11-stubgen _succgen -o .
