import logging
from abc import abstractmethod

import pulp

from learning.predictor.base_predictor import BasePredictor

# MIP_TIMEOUT = 60 * 10  # 10 minutes
MIP_TIMEOUT = 3600 * 24  # 24 hours


class MixedIntegerProgram(BasePredictor):
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
            solved_optimally = "integer optimal solution" in solver_output
            logging.info(f"{solver_output=}")
        except AttributeError:
            solved_optimally = self._m.status == pulp.constants.LpStatusOptimal
        obj_value = self._m.objective.value()
        main_obj_value = self._main_obj.value()
        regularisation_value = self._regularisation.value()
        logging.info(f"{obj_value=}")
        logging.info(f"{main_obj_value=}")
        logging.info(f"{regularisation_value=}")

        # if solved_optimally:
        #     logging.info(f"Fit perfectly with minimal weights")
        # elif abs(self._main_obj.value()) < 1e-5:
        #     logging.info(f"Fit perfectly on training data but not necessarily minimal weights")
        # else:
        #     logging.info(f"Failed to fit perfectly on training data")

    @abstractmethod
    def _fit_impl(self, X, y, sample_weight):
        pass

    @abstractmethod
    def predict(self, X):
        pass
