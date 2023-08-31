This is a minimal example of using `pybind11` library with `planix`. It contains
a basic implementation of greedy best first search using `planix` and calls a
python module for the computation of a heuristic, in this case the goal count
heuristic with source located in `python_goal_count`.

### Prerequisites
- `pybind11` can be as a submodule or pip (see
  [here](https://pybind11.readthedocs.io/en/stable/installing.html)). I
  installed with pip with Python3.10 and `pip install "pybind11[global]"`
  although it appears more recommended to use a virtual environment.
- `g++-10` which can be installed with `sudo apt install g++-10`
- `boost` which can be installed with `sudo apt install libboost-all-dev`

### Example
- build with `make`
- set the `PYTHON_GOAL_COUNT` environment variable to point to
  `python_goal_count` e.g. by running in this directory `export
  PYTHON_GOAL_COUNT=python_goal_count` see `pybind_goal_count.cc` for more
  information
- execute e.g. with
```
./main gripper/domain.pddl gripper/gripper-n10.pddl
```