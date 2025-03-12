import itertools
import logging

import numpy as np
import pulp
from pulp import LpVariable as Var
from pulp import lpDot, lpSum
from tqdm import trange

from learning.dataset.container.ranking_dataset import RankingGroup
from learning.predictor.mip import MixedIntegerProgram


class MixedIntegerProgramRanker(MixedIntegerProgram):
    """Ranking with MIP formulation"""

    IS_RANK = True

    def _fit_impl(self, X, y: list[RankingGroup], sample_weight):
        n, d = X.shape

        m = pulp.LpProblem()

        # Variables
        weights = [
            Var(f"w:{j}", cat=pulp.const.LpInteger, lowBound=-1, upBound=1) for j in range(d)
        ]
        weights_abs = [Var(f"w_abs:{j}", lowBound=0) for j in range(d)]
        f_pred = [lpDot(weights, X[i]) for i in trange(n, desc="Constructing MIP dot products")]
        slacks = []

        # Ranking
        for ranking_groups, w in zip(y, sample_weight):
            good_group = ranking_groups.good_group
            maybe_group = ranking_groups.maybe_group
            bad_group = ranking_groups.bad_group
            # logging.info(f"{good_group=}, {maybe_group=}, {bad_group=}")
            for bad_group, diff in [(maybe_group, 0), (bad_group, 1)]:
                for good_i, bad_i in itertools.product(good_group, bad_group):
                    slack_var = Var(f"s:{len(slacks)}", lowBound=0, cat=pulp.const.LpContinuous)
                    slacks.append(slack_var * w)
                    m += f_pred[bad_i] - f_pred[good_i] >= diff - slack_var

        # Minimise complexity
        for j in range(d):
            m += weights_abs[j] >= -weights[j]
            m += weights_abs[j] >= weights[j]

        main_obj = lpSum(slacks)
        reg_obj = lpSum(weights_abs)
        m += main_obj + reg_obj

        # Solve
        self._solve(m, main_obj, reg_obj)

        # Set learned weights
        self._weights = np.array([w.value() for w in weights])

    def predict(self, X):
        return X @ self._weights.T
