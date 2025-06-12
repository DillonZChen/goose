import itertools
import logging

import numpy as np
import pulp
from pulp import LpVariable as Var
from pulp import lpDot, lpSum
from tqdm import trange

from learning.predictor.predictor import Predictor
from learning.predictor.ranker import handle_redundant_pairs

# MIP_TIMEOUT = 60 * 10  # 10 minutes
MIP_TIMEOUT = 3600 * 24  # 24 hours


class MixedIntegerProgram(Predictor):
    IS_RANK = True

    def __init__(self):
        self._weights = None

    def _make_solver(self, timeout: float):
        if pulp.apis.CPLEX_PY().available():
            logging.info("Using CPLEX solver")
            return pulp.getSolver("CPLEX_PY", timeLimit=timeout)
        else:
            logging.info("Using CBC solver")
            return pulp.getSolver("PULP_CBC_CMD", timeLimit=timeout)

    def _solve(self, m, main_obj, regularisation, timeout=MIP_TIMEOUT):
        m.checkDuplicateVars()
        solver = self._make_solver(timeout=timeout)
        m.solve(solver)

        self._solver = solver
        self._m = m
        self._main_obj = main_obj
        self._regularisation = regularisation

    def _evaluate_impl(self):
        try:
            solver_output = self._solver.solverModel.solution.get_status_string()
            logging.info(f"{solver_output=}")
        except AttributeError:
            pass
        obj_value = self._m.objective.value()
        main_obj_value = self._main_obj.value()
        regularisation_value = self._regularisation.value()
        logging.info(f"{obj_value=}")
        logging.info(f"{main_obj_value=}")
        logging.info(f"{regularisation_value=}")

    def _fit_impl(self, X, y, sample_weight):
        # Remove duplicate pairs
        X, maybe_pair, maybe_sample_weight, bad_pair, bad_sample_weight = handle_redundant_pairs(X, y)

        n, d = X.shape

        m = pulp.LpProblem()

        # Variables
        if self._class_name == "MixedIntegerProgramRanker":
            weights = [Var(f"w:{j}", cat=pulp.const.LpInteger, lowBound=-1, upBound=1) for j in range(d)]
        elif self._class_name == "LinearProgramRanker":
            weights = [Var(f"w:{j}", cat=pulp.const.LpContinuous) for j in range(d)]
        else:
            raise ValueError(f"Unknown class {self._class_name}")
        weights_abs = [Var(f"w_abs:{j}", lowBound=0) for j in range(d)]
        f_pred = [lpDot(weights, X[i]) for i in trange(n, desc="Constructing MIP dot products")]
        slacks = []

        # Ranking
        for pairs, sample_weight, diff in [
            (maybe_pair, maybe_sample_weight, 0),
            (bad_pair, bad_sample_weight, 1),
        ]:
            for key, (i_g, i_b) in pairs.items():
                w = sample_weight[key]
                slack_var = Var(f"s:{len(slacks)}", lowBound=0, cat=pulp.const.LpContinuous)
                slacks.append(slack_var * w)
                m += f_pred[i_b] - f_pred[i_g] >= diff - slack_var

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
