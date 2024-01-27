# ensure this script is run after activating your virtual environment

### Install XGBoost
git submodule update --init --recursive

cd xgboost
mkdir -p build
cd build
cmake ..
make install

### Build XGBoost trainer
cd learner/xgboost_cpp
rm -rf build
python3 build.py
