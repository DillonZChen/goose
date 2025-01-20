import itertools
import logging
import random

import numpy as np
from sklearn import svm

from learning.dataset.container.ranking_dataset import RankingGroup
from learning.predictor.base_predictor import BasePredictor


class SVMRanker(BasePredictor):
    """Ranking with SVM formulation"""

    IS_RANK = True

    def fit(self, X, y: list[RankingGroup]):
        # n, d = X.shape

        model = svm.LinearSVC(loss="hinge", fit_intercept=False, max_iter=1000000, C=1.0)

        X_in = []
        y_in = []
        for ranking_groups in y:
            good_group = ranking_groups.good_group
            maybe_group = ranking_groups.maybe_group
            bad_group = ranking_groups.bad_group
            # Non-support vectors do not contribute strongly to predictions (RW2006)
            for bad_group, diff in [(maybe_group, 1), (bad_group, 1)]:
                for good_i, bad_i in itertools.product(good_group, bad_group):
                    # random flip as SVM requires both classes
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

    def _evaluate(self):
        mean_accuracy = self._model.score(self._X, self._y)
        logging.info(f"{mean_accuracy=}")

    def predict(self, X):
        w = self._model.coef_.reshape(-1)
        return X @ w.T
