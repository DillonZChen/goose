import os


_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
GOOSE_ROOT_DIR = os.path.normpath(f"{_CUR_DIR}/..")
PLANNERS_DIR = os.path.normpath(f"{GOOSE_ROOT_DIR}/../ext/planners")
DOWNWARD_SCRIPT = f"{PLANNERS_DIR}/scorpion/fast-downward.py"
