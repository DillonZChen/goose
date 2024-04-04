import itertools
import numpy as np
import random

# helper function for svms
def transform_pairwise(X, y):
    """
    Transforms data into pairs for convex relaxation of kendal rank correlation coef
    In this method, all pairs are choosen, except for those that have the same target value or equal cost
    Inputs
    ----------
    X : array, shape (n_samples, n_features)
        The input feature vec of states from of several problems
    y : array, shape (n_samples,) or (n_samples, 2)
        The input cost vector. If it's a 2D array, the second column represents
        the problem index
    Returns
    -------
    X_trans : array, shape (k, n_feaures)
        Difference between features of states (si - sj), only consider the state pair from the same problem
    y_trans : array, shape (k,)
        Output rank labels of values {-1, +1}, 1 represent si has potentially larger cost than sj (further away from goal)
    """
    X_new = []
    y_new = []
    if y.ndim == 1:
        y = np.c_[y, np.ones(y.shape[0])]  
    comb = itertools.combinations(range(X.shape[0]), 2)
    for k, (i, j) in enumerate(comb):
        if y[i, 0] == y[j, 0] or y[i, 1] != y[j, 1]:
            # skip if they have the same cost or are from different problem group
            continue
        # otherwise, make the new pair-wise data
        X_new.append(X[i] - X[j])
        y_new.append(np.sign(y[i, 0] - y[j, 0])) # y = 1 if xi further away (larger cost), Vice Vesa
#         randomly output some negative values for training purpose
#         if y_new[-1] != (-1) ** k:
#             y_new[-1] = - y_new[-1]
#             X_new[-1] = - X_new[-1]
    length = len(y_new)
    random_indices = random.sample(range(length), length // 2)
    for i in random_indices:
        y_new[i] = - y_new[i]
        X_new[i] = - X_new[i]
    return np.asarray(X_new), np.asarray(y_new)

def transform_pairwise_sequential(X, y):
    """
    Transforms data into pairs for convex relaxation of kendal rank correlation coef
    In this method, all pairs are choosen, except for those that have the same target value or equal cost
    Inputs
    ----------
    X : array, shape (n_samples, n_features)
        The input feature vec of states from of several problems
    y : array, shape (n_samples,) or (n_samples, 2)
        The input cost vector. If it's a 2D array, the second column represents
        the problem index
    Returns
    -------
    X_trans : array, shape (k, n_feaures)
        Difference between features of states (si - sj), only consider the state pair from the same problem
    y_trans : array, shape (k,)
        Output rank labels of values {-1, +1}, 1 represent si has potentially larger cost than sj (further away from goal)
    """
    X_new = []
    y_new = []
    if y.ndim == 1:
        y = np.c_[y, np.ones(y.shape[0])]
    # Get unique prpblem indecies from the second column
    problems = np.unique(y[:, 1])

    # Group the indices based on the categories
    groups = [list(np.where(y[:, 1] == prob)[0]) for prob in problems] 
    # make the new pair-wise data
    for prob in groups:
        for i in range(len(prob)-1):
            # add sequential pairs
            index = prob[i]
            X_new.append(X[index]-X[index+1])
            y_new.append(np.sign(y[index,0] - y[index+1,0]))
    # randomly output some negative values for training purpose
    length = len(y_new)
    random_indices = random.sample(range(length), length // 2)
    for i in random_indices:
        y_new[i] = - y_new[i]
        X_new[i] = - X_new[i]
    return np.asarray(X_new), np.asarray(y_new)


def transform_neighbor(X, y):
    p_idxs = np.unique(y[:, 3])
    diffs = None
    for p_idx in p_idxs:
        encodes = X[y[:,3] == p_idx, :]
        coord_x = y[y[:,3] == p_idx, 1]
        coord_y = y[y[:,3] == p_idx, 2]
        encodes_xy = np.concatenate([encodes, coord_x.reshape(-1, 1), coord_y.reshape(-1, 1)], axis=1)
        unique = np.unique(coord_x)
        split_by_x = [encodes_xy[coord_x == i] for i in unique]
        diff = []
        last_best = None
        for s in split_by_x:
            # init state, no states worse than it
            if s[0, -2] == 0:
                last_best = s
                continue
            sort_s = s[s[:, -1].argsort()]
            # print(torch.logical_and(data.coord_x == (s[0, -2].item()-1), data.coord_y == 0))
            diff.append(sort_s[1:, :-2] - sort_s[0, :-2])
            diff.append((last_best[0, :-2] - sort_s[0, :-2]).reshape([1, -1]))
            last_best = sort_s[0, :].reshape((1, -1))
        diff = np.concatenate(diff, axis=0)

        if diffs is not None:
            diffs = np.concatenate([diffs, diff], axis=0)
        else:
            diffs = diff

    # randomly flip half of data as SVM requires both signs in data
    polarity_copy = np.random.choice([-1, 1], size=diffs.shape[0], p=[.5, .5])
    diffs = np.multiply(diffs, polarity_copy[:, np.newaxis])

    return diffs, polarity_copy