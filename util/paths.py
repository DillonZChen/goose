import os


_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
GOOSE_ROOT_DIR = os.path.normpath(f"{_CUR_DIR}/..")
PLANNERS_DIR = f"{GOOSE_ROOT_DIR}/planning/ext"
