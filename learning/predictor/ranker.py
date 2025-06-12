import itertools

import numpy as np


def handle_redundant_pairs(X, y):
    new_X = {}

    def get_row(x):
        if x in new_X:
            return new_X[x]
        else:
            new_X[x] = len(new_X)
            return new_X[x]

    maybe_pair = {}
    maybe_sample_weight = {}
    bad_pair = {}
    bad_sample_weight = {}

    for ranking_groups in y:
        good_group = ranking_groups.good_group
        maybe_group = ranking_groups.maybe_group
        bad_group = ranking_groups.bad_group

        for group, pair_dict, weight_dict in [
            (maybe_group, maybe_pair, maybe_sample_weight),
            (bad_group, bad_pair, bad_sample_weight),
        ]:
            for good_i, bad_i in itertools.product(good_group, group):
                x_g = X[good_i].tolist()
                x_b = X[bad_i].tolist()
                x_g = tuple(x_g)
                x_b = tuple(x_b)
                if (x_g, x_b) not in pair_dict:
                    i_g = get_row(x_g)
                    i_b = get_row(x_b)
                    pair_dict[(x_g, x_b)] = (i_g, i_b)
                    weight_dict[(x_g, x_b)] = 1
                else:
                    weight_dict[(x_g, x_b)] += 1

    X = [None] * len(new_X)
    for x, i in new_X.items():
        X[i] = x
    X = np.array(X)

    return X, maybe_pair, maybe_sample_weight, bad_pair, bad_sample_weight
