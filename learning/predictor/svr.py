import warnings

from sklearn.exceptions import ConvergenceWarning
from sklearn.svm import LinearSVR

from .base_predictor import BasePredictor

warnings.filterwarnings("ignore", category=ConvergenceWarning)


class SupportVectorRegression(BasePredictor):
    """Linear SVR"""

    def fit(self, X, y):
        model = LinearSVR(random_state=0, max_iter=10000)
        model.fit(X, y)
        self._weights = model.coef_

    def predict(self, X):
        return X @ self._weights.T
