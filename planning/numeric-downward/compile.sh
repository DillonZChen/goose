#!/bin/bash

cd src/search/bliss-0.73
make
cd ../../../

python3 build.py -j8 release64
echo
echo "Build complete!"
echo
