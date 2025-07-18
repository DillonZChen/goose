import logging

import numpy as np

from goose.util.logging import mat_to_str


def log_quartiles(array: np.array, description: str) -> None:
    n_data = len(array)
    min_val = np.min(array)
    max_val = np.max(array)
    q1 = np.percentile(array, 25)
    q2 = np.percentile(array, 50)
    q3 = np.percentile(array, 75)
    mat = [["n_data", n_data], ["q1", q1], ["q2", q2], ["q3", q3], ["min", min_val], ["max", max_val]]
    mat = mat_to_str(mat, rjust=[True, True])
    logging.info(f"{description}\n{mat}")
