import logging

import numpy as np
from sklearn.gaussian_process import GaussianProcessRegressor as GPR
from sklearn.gaussian_process.kernels import DotProduct
from termcolor import colored

from .base_predictor import BasePredictor


class GaussianProcessRegressor(BasePredictor):
    """Linear GPR"""

    IS_RANK = False

    def _try_fit(self, X, y):
        kernel = DotProduct(sigma_0=0, sigma_0_bounds="fixed")
        model = GPR(kernel=kernel, alpha=1e-7, random_state=0)
        model.fit(X, y)
        self._weights = model.alpha_ @ model.X_train_

    def fit(self, X, y):
        alpha = 1e-7

        try:
            self._try_fit(X, y)
        except np.linalg.LinAlgError:
            logging.info(
                colored(
                    f"Encounterd LinAlgError, trying again with duplicate entries removed",
                    "light_red",
                )
            )
            y = np.array(y)
            Xy = np.hstack([X, y.reshape(-1, 1)])
            Xy_unique = np.unique(Xy, axis=0)
            X = Xy_unique[:, :-1]
            y = Xy_unique[:, -1]
            logging.info(f"{X.shape=}")
            logging.info(f"{y.shape=}")

        while True:
            try:
                self._try_fit(X, y)
                break
            except np.linalg.LinAlgError:
                alpha *= 10
                logging.info(colored(f"LinAlgError, setting alpha={alpha}", "light_red"))
            if alpha > 1:
                logging.info(
                    colored(
                        "Failed to train up to alpha=1. It is unlikely anything good will be learned so saving zero weights...",
                        "red",
                    )
                )
                self._weights = np.zeros(X.shape[1])
                break

    def predict(self, X):
        return X @ self._weights.T
