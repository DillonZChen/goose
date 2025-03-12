import warnings

from sklearn.exceptions import ConvergenceWarning
from sklearn.svm import LinearSVR

from .mse_minimiser import MSEMinimiser

warnings.filterwarnings("ignore", category=ConvergenceWarning)


class SupportVectorRegression(MSEMinimiser):
    """Linear SVR"""

    IS_RANK = False

    def _fit_impl(self, X, y, sample_weight):
        model = LinearSVR(random_state=0, max_iter=10000)
        model.fit(X, y, sample_weight=sample_weight)
        self._weights = model.coef_
        self._X = X
        self._y = y
        self._sample_weight = sample_weight

    def predict(self, X):
        return X @ self._weights.T
