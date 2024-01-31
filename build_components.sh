# ensure this script is run after activating your virtual environment

### Install XGBoost
git submodule update --init --recursive

cd xgboost
mkdir -p build
cd build
cmake ..
make install

### Build planners
for planner in downward_cpu downward_gpu powerlifted; do
    cd planners/$planner
    rm -rf builds
    python3 build.py
    cd ../..
done

### Build XGBoost trainer
cd learner/xgboost_cpp
rm -rf build
python3 build.py
