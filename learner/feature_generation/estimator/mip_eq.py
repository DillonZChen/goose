import numpy as np
import pulp
from pulp import LpVariable as Var
from pulp import lpDot, lpSum
from tqdm import trange

from learner.feature_generation.estimator.mip import Mip
from util.timer import TimerContextManager


class MipEq(Mip):
    def __init__(self):
        super().__init__()
        self._initialised = False

    def fit(self, X, y):
        n, d = X.shape

        ### PuLP is slow at initialising variables and expressions that we reuse
        if not self._initialised:
            print("Initialising MIP expressions (done once for all schemata)...")
            self.weights = [
                Var(f"w:{j}", cat=pulp.const.LpInteger, lowBound=-1, upBound=1)
                for j in range(d)
            ]
            self.weights_abs = [Var(f"w_abs:{j}") for j in range(d)]
            self.diffs = [Var(f"diff:{i}") for i in range(n)]

            self.predictions = [
                lpDot(self.weights, X[i])
                for i in trange(n, desc="Constructing MIP dot products")
            ]

            self._initialised = True
            print("Initialisation complete!")

        ### Initialise new problem
        with TimerContextManager("initialising MIP model"):
            m = pulp.LpProblem()

            # minimise L1 distance loss
            for i in range(n):
                m += self.diffs[i] >= self.predictions[i] - y[i]
                m += self.diffs[i] >= y[i] - self.predictions[i]
            main_obj = lpSum(self.diffs)  # abs value of differences

            # minimise weights for tie breaking
            for j in range(d):
                m += self.weights_abs[j] >= -self.weights[j]
                m += self.weights_abs[j] >= self.weights[j]

            # m += sum(y) * main_obj + lpDot(weights_abs, tiebreaker)
            reg_obj = lpSum(self.weights_abs)
            m += d * main_obj + reg_obj

        ### Solve
        self._solve(m, main_obj, reg_obj)

        ### Set learned weights
        self.coef_ = np.array([w.value() for w in self.weights])

        return self
