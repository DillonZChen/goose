import itertools
import logging
import random

import numpy as np
from sklearn.gaussian_process import GaussianProcessClassifier as GPC
from sklearn.gaussian_process.kernels import DotProduct

from .base_predictor import BasePredictor


class GaussianProcessRanker(BasePredictor):
    """Linear GPC for Ranking with Laplace Approximation"""

    IS_RANK = True

    def fit(self, X, y):
        kernel = DotProduct(sigma_0=0, sigma_0_bounds="fixed")
        model = GPC(kernel=kernel, random_state=0)
        X_in = []
        y_in = []
        for ranking_groups in y:
            good_group = ranking_groups.good_group
            maybe_group = ranking_groups.maybe_group
            bad_group = ranking_groups.bad_group
            # Non-support vectors do not contribute strongly to predictions
            for bad_group, diff in [(maybe_group, 1), (bad_group, 1)]:
                for good_i, bad_i in itertools.product(good_group, bad_group):
                    # random flips as GPC requires both classes
                    if random.random() > 0.5:
                        X_in.append(X[bad_i] - X[good_i])
                        y_in.append(diff)
                    else:
                        X_in.append(X[good_i] - X[bad_i])
                        y_in.append(-diff)

        X_in = np.array(X_in)
        y_in = np.array(y_in)
        model.fit(X_in, y_in)

        self._model = model
        self._X = X_in
        self._y = y_in
        # See 3.4.3 [Rasmussen, Carl Edward and Williams, Christopher K. I., 2006]
        self._weights = np.dot(
            (model.base_estimator_.y_train_ - model.base_estimator_.pi_).T,
            model.base_estimator_.X_train_,
        )

    def _evaluate(self):
        mean_accuracy = self._model.score(self._X, self._y)
        logging.info(f"{mean_accuracy=}")

    def predict(self, X):
        return X @ self._weights.T
