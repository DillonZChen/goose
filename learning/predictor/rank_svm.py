import itertools
import random

import numpy as np
from sklearn import svm

from learning.dataset.ranking_dataset import RankingGroup
from learning.predictor.base_predictor import BasePredictor


class SVMRanker(BasePredictor):
    def fit(self, X, y: list[RankingGroup]):
        # n, d = X.shape

        model = svm.LinearSVC(loss='hinge', fit_intercept=False, max_iter=1000000, C=1.0)

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

        # Set learned weights
        self._weights = model.coef_.reshape(-1,)

    def predict(self, X):
        return X @ self._weights.T
