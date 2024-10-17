# https://stackoverflow.com/a/47488721
# In [176]: group_dup_rowids(np.abs(a))
# Out[176]: [[0, 2], [1, 4]]
from itertools import combinations

import numpy as np


def group_dup_rowids(array):
    """
    Groups duplicate row indices in a 2D array.

    Args:
        array (numpy.ndarray): The input 2D array.

    Returns:
        list: A list of lists, where each inner list contains the indices of duplicate rows.
    """
    sidx = np.lexsort(array.T)
    b = array[sidx]
    m = np.concatenate(([False], (b[1:] == b[:-1]).all(1), [False]))
    idx = np.flatnonzero(m[1:] != m[:-1])
    C = sidx.tolist()
    return [C[i:j] for i, j in zip(idx[::2], idx[1::2] + 1)]


def distinguish(X, y):
    y = np.array(y)
    dup_rows = group_dup_rowids(X)
    n_rows = X.shape[0]
    n_pairs = n_rows * (n_rows - 1) // 2
    n_indistinguished = 0
    for group in dup_rows:
        group = np.array(group)
        y_group = y[group]
        cnt = {}
        for label in y_group:
            cnt[label] = cnt.get(label, 0) + 1
        cnt = list(cnt.values())
        for ci, cj in combinations(cnt, 2):
            n_indistinguished += ci * cj
    print(f"{n_indistinguished=}")
    print(f"{n_pairs=}")


if __name__ == "__main__":
    a = np.array(
        [
            [1, 2, 3],
            [3, 4, 5],
            [1, 2, 3],
            [5, 6, 5],
            [3, 4, 5],
            [3, 4, 5],
        ]
    )
    print(group_dup_rowids(a))
