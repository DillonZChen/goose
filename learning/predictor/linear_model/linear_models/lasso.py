import warnings

from sklearn.exceptions import ConvergenceWarning
from sklearn.linear_model import Lasso

from learning.predictor.linear_model.unitary_regressor import UnitaryRegressor


warnings.filterwarnings("ignore", category=ConvergenceWarning)


class LassoRegression(UnitaryRegressor):
    """L1 loss"""

    IS_RANK = False

    def _fit_impl(self, X, y, sample_weight):
        model = Lasso(alpha=1.0, random_state=0, max_iter=10000)
        model.fit(X, y, sample_weight=sample_weight)
        self._weights = model.coef_
        self._X = X
        self._y = y
        self._sample_weight = sample_weight

    def predict(self, X):
        return X @ self._weights.T
