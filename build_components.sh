# ensure this script is run after activating your virtual environment

### Build dlplan
cd dlplan
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build -j4
cmake --install build --prefix=../planners/downward_cpu/src/search/ext/dlplan
cd ..

### Build planners
for planner in downward_cpu downward_gpu powerlifted; do
    echo
    echo "========= Building $planner ========="
    echo
    cd planners/$planner
    rm -rf builds
    python3 build.py
    cd ../..
done
