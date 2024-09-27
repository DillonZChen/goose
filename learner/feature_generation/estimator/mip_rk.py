import logging
import pickle
from itertools import product
from typing import List

import numpy as np
import pulp
from pulp import LpVariable as Var
from pulp import lpDot, lpSum
from tqdm import trange

from learner.feature_generation.estimator.mip import Mip
from util.timer import TimerContextManager


TIMEOUT = 3600 * 24  # 24 hours


class MipRk(Mip):
    def __init__(self):
        super().__init__()
        self._succ_groups = None

    def set_succ_groups(self, succ_groups):
        self._succ_groups = succ_groups

    def fit(self, X, y):
        n, d = X.shape

        m = pulp.LpProblem()

        ### variables
        weights = [
            # Var(f"w:{j}", cat=pulp.const.LpInteger)
            Var(f"w:{j}", cat=pulp.const.LpInteger, lowBound=-1, upBound=1)
            for j in range(d)
        ]
        weights_abs = [Var(f"w_abs:{j}", lowBound=0) for j in range(d)]
        f_pred = [
            lpDot(weights, X[i])
            for i in trange(n, desc="Constructing MIP dot products")
        ]
        slacks = []

        ## ranking
        for good_groups, maybe_bad_groups, def_bad_groups in self._succ_groups:
            # print(len(good_groups), len(maybe_bad_groups), len(def_bad_groups))
            for bad_group, diff in [(maybe_bad_groups, 0), (def_bad_groups, 1)]:
                for good_i, bad_i in product(good_groups, bad_group):
                    slack_var = Var(f"s:{len(slacks)}", lowBound=0, cat=pulp.const.LpContinuous)
                    slacks.append(slack_var)
                    m += f_pred[bad_i] - f_pred[good_i] >= diff - slack_var

        ## minimise complexity
        for j in range(d):
            m += weights_abs[j] >= -weights[j]
            m += weights_abs[j] >= weights[j]

        main_obj = lpSum(slacks)
        reg_obj = lpSum(weights_abs)
        m += main_obj + reg_obj

        ### Solve
        self._solve(m, main_obj, reg_obj, timeout=TIMEOUT)

        ### Set learned weights
        self.coef_ = np.array([w.value() for w in weights])

        return self
