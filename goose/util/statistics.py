import logging

import numpy as np
from tabulate import tabulate


def log_quartiles(array: np.array, description: str) -> None:
    n_data = len(array)
    min_val = np.min(array)
    max_val = np.max(array)
    q1 = np.percentile(array, 25)
    q2 = np.percentile(array, 50)
    q3 = np.percentile(array, 75)
    mat = [["n_data", n_data], ["q1", q1], ["q2", q2], ["q3", q3], ["min", min_val], ["max", max_val]]
    mat = tabulate(mat)
    logging.info(f"{description}\n{mat}")
