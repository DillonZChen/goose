import logging
import random
import warnings

import numpy as np
from sklearn import svm
from sklearn.gaussian_process import GaussianProcessClassifier as GPC
from sklearn.gaussian_process.kernels import DotProduct
from sklearn.metrics import f1_score, mean_squared_error

from learning.dataset.container.ranking_dataset import RankingGroup
from learning.predictor.ranker import handle_redundant_pairs

from .predictor import Predictor


class Classifier(Predictor):
    IS_RANK = True

    def _fit_impl(self, X, y: list[RankingGroup], sample_weight):
        if self._class_name == "SupportVectorMachineRanker":
            if sample_weight is not None and not np.isclose(sample_weight, np.ones(len(y))).all():
                warnings.warn("sample_weights is not supported by Gaussian Processes")
                sample_weight_in = np.ones(len(y))
            model = svm.LinearSVC(loss="hinge", fit_intercept=False, max_iter=1000000, C=1.0)
        elif self._class_name == "GaussianProcessClassifierRanker":
            model = GPC(kernel=DotProduct(sigma_0=0, sigma_0_bounds="fixed"), random_state=0)
        else:
            raise NotImplementedError(f"Unknown class name: {self._class_name}")

        X, maybe_pair, maybe_sample_weight, bad_pair, bad_sample_weight = handle_redundant_pairs(X, y)

        X_in = []
        y_in = []
        sample_weight_in = []

        for pairs, sample_weight in [
            (maybe_pair, maybe_sample_weight),
            (bad_pair, bad_sample_weight),
        ]:
            for key, (i_g, i_b) in pairs.items():
                w = sample_weight[key]
                x = X[i_b] - X[i_g]
                y = 1

                # Non-support vectors do not contribute strongly to predictions (RW2006)
                # Random flip as SVM requires both classes
                if random.random() > 0.5:
                    x = -x
                    y = -y

                X_in.append(x)
                y_in.append(y)
                sample_weight_in.append(w)

        X_in = np.array(X_in)
        y_in = np.array(y_in)
        model.fit(X_in, y_in)

        self._model = model
        self._X = X_in
        self._y = y_in
        self._sample_weight = sample_weight_in

        if self._class_name == "SupportVectorMachineRanker":
            self._weights = model.coef_.reshape(-1)
        elif self._class_name == "GaussianProcessClassifierRanker":
            # See 3.4.3 [Rasmussen, Carl Edward and Williams, Christopher K. I., 2006]
            self._weights = np.dot(
                (model.base_estimator_.y_train_ - model.base_estimator_.pi_).T,
                model.base_estimator_.X_train_,
            )
        else:
            raise NotImplementedError(f"Unknown class name: {self._class_name}")

    def _evaluate_impl(self):
        mean_accuracy = self._model.score(self._X, self._y, sample_weight=self._sample_weight)
        logging.info(f"{mean_accuracy=}")

    def predict(self, X):
        return X @ self._weights.T
