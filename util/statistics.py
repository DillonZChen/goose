from typing import Iterable, Tuple

import numpy as np
from sklearn.metrics import f1_score


def f1_macro(y_true, y_pred):
    return f1_score(
        np.rint(y_true).astype(int), np.rint(y_pred).astype(int), average="macro"
    )


def print_mat(mat: Iterable[Iterable], rjust: bool = True):
    if not mat:
        print("Empty mat")
        return

    max_lengths = [max(len(str(row[i])) for row in mat) for i in range(len(mat[0]))]

    for row in mat:
        for i, cell in enumerate(row):
            if rjust:
                print(str(cell).rjust(max_lengths[i]), end="  ")
            else:
                print(str(cell).ljust(max_lengths[i]), end="  ")
        print()


def dump_several_stats(*args: Tuple[Iterable, str]):
    datas, descriptions = zip(*args)
    mat = [
        [""] + list(descriptions),
        ["min"] + [min(data) for data in datas],
        ["max"] + [max(data) for data in datas],
        ["q1"] + [np.percentile(data, 25) for data in datas],
        ["q2"] + [np.percentile(data, 50) for data in datas],
        ["q3"] + [np.percentile(data, 75) for data in datas],
    ]

    print_mat(mat)
