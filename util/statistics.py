import logging

import numpy as np


def log_quartiles(array: np.array):
    min_val = np.min(array)
    max_val = np.max(array)
    q1 = np.percentile(array, 25)
    q2 = np.percentile(array, 50)
    q3 = np.percentile(array, 75)
    logging.info(f"target (y) statistics:")
    logging.info(f" min {min_val}")
    logging.info(f"  q1 {q1}")
    logging.info(f"  q2 {q2}")
    logging.info(f"  q3 {q3}")
    logging.info(f" max {max_val}")
