from abc import abstractmethod

import numpy as np
import pulp
from sklearn.base import BaseEstimator
from sklearn.utils.validation import check_array, check_is_fitted


class Mip(BaseEstimator):
    def __init__(self):
        self.coef_ = None
        self.bias_ = 0  # no bias
        self.intercept_ = 0  # no bias
        self.pair_mins = []

    def make_solver(self, timeout: float):
        if pulp.apis.CPLEX_PY().available():
            print("Using CPLEX solver")
            return pulp.getSolver("CPLEX_PY", timeLimit=timeout)
        else:
            print("Using CBC solver")
            return pulp.getSolver("PULP_CBC_CMD", timeLimit=timeout)
        
    def _solve(self, m, main_obj, reg_obj, timeout=5):
        m.checkDuplicateVars()
        solver = self.make_solver(timeout=timeout)
        m.solve(solver)

        print()
        print("=" * 80)
        try:
            solver_output = solver.solverModel.solution.get_status_string()
            solved_optimally = "integer optimal solution" in solver_output
            print("Solver output:", solver_output)
        except AttributeError:
            solved_optimally = m.status == pulp.constants.LpStatusOptimal
        print("Objective value:", m.objective.value())
        print("Main objective value:", main_obj.value())
        print("Regularisation value:", reg_obj.value())
        print("=" * 80)
        print()

        if solved_optimally:
            print(f"Fit perfectly with minimal weights")
        elif abs(main_obj.value()) < 1e-5:
            print(f"Fit perfectly on training data but not necessarily minimal weights")
        else:
            print(f"Failed to fit perfectly on training data")

    @abstractmethod
    def fit(self, X, y):
        raise NotImplementedError

    def predict(self, X):
        check_is_fitted(self)
        X = check_array(X)
        y = X @ self.coef_
        return y
