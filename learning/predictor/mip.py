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

        logging.info("=" * 30)
        try:
            solver_output = solver.solverModel.solution.get_status_string()
            solved_optimally = "integer optimal solution" in solver_output
            logging.info(f"{solver_output=}")
        except AttributeError:
            solved_optimally = m.status == pulp.constants.LpStatusOptimal
        logging.info(f"{m.objective.value()=}")
        logging.info(f"{main_obj.value()=}")
        logging.info(f"{regularisation.value()=}")
        logging.info("=" * 30)

        if solved_optimally:
            logging.info(f"Fit perfectly with minimal weights")
        elif abs(main_obj.value()) < 1e-5:
            logging.info(f"Fit perfectly on training data but not necessarily minimal weights")
        else:
            logging.info(f"Failed to fit perfectly on training data")

    @abstractmethod
    def fit(self, X, y):
        pass

    @abstractmethod
    def predict(self, X):
        pass
