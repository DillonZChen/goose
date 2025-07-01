import os


_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
GOOSE_ROOT_DIR = os.path.normpath(f"{_CUR_DIR}/..")
DATA_CACHE_DIR = os.path.join(GOOSE_ROOT_DIR, ".data_cache")
