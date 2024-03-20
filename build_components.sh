# ensure this script is run after activating your virtual environment

### Submodules
git submodule update --init --recursive

### Build dlplan
cd dlplan
#cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
#cmake --build build -j4
#cmake --install build --prefix=../planners/downward_cpu/src/search/ext/dlplan

# Configure with installation prefixes of all dependencies
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${PWD}/dependencies/installs
# Build
cmake --build build -j16
# Install (optional)
cmake --install build --prefix=../planners/downward_cpu/src/search/ext/dlplan
cd ..

### Build planners
#for planner in downward_cpu downward_gpu powerlifted; do
#planner=downward_cpu
#echo
#echo "========= Building $planner ========="
#echo
#cd planners/$planner
#rm -rf builds
#python3 build.py
#cd ../..
#done
