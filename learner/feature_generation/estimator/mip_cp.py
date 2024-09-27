import logging
import pickle

import numpy as np
import pulp
from pulp import LpVariable as Var
from pulp import lpDot, lpSum
from tqdm import trange

from learner.feature_generation.estimator.mip import Mip
from util.timer import TimerContextManager


class MipCp(Mip):
    def __init__(self):
        super().__init__()

    def fit(self, X, y):
        # X = [[1, 10, 10, 10], [10, 1, 1, 1]]
        # y = [0, 1]
        # X = np.array(X)
        # y = np.array(y)

        # print(f"Select subset for faster MIP solving")        
        # y_0_indices = np.where(y == 0)[0]
        # y_1_indices = np.where(y == 1)[0]

        # size = min(20, len(y_0_indices), len(y_1_indices))

        # y_0_indices = np.random.choice(y_0_indices, size=size, replace=False)
        # y_1_indices = np.random.choice(y_1_indices, size=size, replace=False)

        # X = np.concatenate([X[y_0_indices], X[y_1_indices]])
        # y = np.concatenate([y[y_0_indices], y[y_1_indices]])

        n, d_og = X.shape

        ### pair min max
        same_columns = set()
        for i in range(d_og):
            same_columns.add(tuple(X[:, i]))
        assert d_og % 2 == 0
        n_cat = d_og // 2
        n_con = d_og // 2
        to_add = []
        paired = []
        non_zero_indices = []
        for i in range(n_cat, n_cat + n_con):
            if np.linalg.norm(X[:, i]) != 0:
                non_zero_indices.append(i)
        for ii in trange(len(non_zero_indices), desc="Constructing min pairs"):
            i = non_zero_indices[ii]
            for jj in range(ii + 1, len(non_zero_indices)):
                j = non_zero_indices[jj]
                minn = np.minimum(X[:, i], X[:, j])
                if tuple(minn) not in same_columns:
                    to_add.append(minn)
                    paired.append((i, j))
        X = np.hstack((X, np.vstack(to_add).T))
        print(f"new X shape: {X.shape}")

        n, d = X.shape

        assert set(y.tolist()) == {0, 1}  # only binary classification

        BIG_M = 3 + np.max(np.sum(X, axis=1))
        print(f"{BIG_M=}")

        m = pulp.LpProblem()

        ### variables
        weights = [
            Var(f"w:{j}", cat=pulp.const.LpInteger, lowBound=-1, upBound=1)
            for j in range(d)
        ]
        weights_abs = [Var(f"w_abs:{j}") for j in range(d)]
        f_pred = [
            lpDot(weights, X[i])
            for i in trange(n, desc="Constructing MIP dot products")
        ]
        u = [
            Var(f"u:{i}", cat=pulp.const.LpInteger, lowBound=0, upBound=1)
            for i in range(n)
        ]
        diffs = [Var(f"diff:{i}") for i in range(n)]

        for i in range(n):
            u[i].setInitialValue(y[i])

        # use Big-M to determine y_pred based on whether f_pred >= 0
        for i in range(n):
            m += 1 - BIG_M * (1 - u[i]) <= f_pred[i]
            m += f_pred[i] <= BIG_M * u[i]

        # minimise L1 distance loss
        for i in range(n):
            m += diffs[i] >= u[i] - y[i]
            m += diffs[i] >= y[i] - u[i]
        main_obj = lpSum(diffs)  # abs value of differences

        # minimise weights for tie breaking
        for j in range(d):
            m += weights_abs[j] >= -weights[j]
            m += weights_abs[j] >= weights[j]

        # m += sum(y) * main_obj + lpDot(weights_abs, tiebreaker)
        reg_obj = lpSum(weights_abs)
        m += 100 * main_obj + reg_obj

        ### Solve
        self._solve(m, main_obj, reg_obj)

        ### Set learned weights
        self.coef_ = [0] * d_og
        weight_vals = [w.value() for w in weights]
        print("Learned weights:")
        for idx in range(d):
            val = weight_vals[idx]
            if val == 0:
                continue
            log = f"{idx=}, {val=}"
            if idx >= d_og:
                pair = paired[idx - d_og]
                log += f" (min {pair})"
                self.pair_mins.append(pair)
                self.coef_.append(val)
            else:
                self.coef_[idx] = val
            print(log)
        self.coef_ = np.array(self.coef_)

        return self
