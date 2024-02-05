# ensure this script is run after activating your virtual environment

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
