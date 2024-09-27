from abc import abstractmethod
from itertools import combinations, product

import numpy as np
from sklearn.base import BaseEstimator
from sklearn.utils.validation import check_array, check_is_fitted
from tqdm import tqdm


class BruteForce(BaseEstimator):
    def __init__(self):
        self.coef_ = None
        self.bias_ = 0  # no bias
        self.intercept_ = 0  # no bias

    def fit(self, X, y):
        n, d = X.shape
        self.coef_ = np.zeros(d)

        minn_diff = np.inf

        for i in range(d):
            diff = np.linalg.norm(X[:, i] - y)
            minn_diff = min(minn_diff, diff)
            if diff < 1e-9:
                self.coef_[i] = 1
                print(f"Found match at {i}")
                return

        for i, j in tqdm(list(combinations(range(d), 2))):
            for w_i, w_j in product([-1, 1], repeat=2):
                if np.linalg.norm(w_i * X[:, i] + w_j * X[:, j] - y) < 1e-9:
                    self.coef_[i] = w_i
                    self.coef_[j] = w_j
                    print(f"Found match at w_{i}={w_i}, w_{j}={w_j}")
                    return


        print(f"No coef_ found")

    def predict(self, X):
        check_is_fitted(self)
        X = check_array(X)
        y = X @ self.coef_
        return y
