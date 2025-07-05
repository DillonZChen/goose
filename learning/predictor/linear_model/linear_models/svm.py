import warnings

from sklearn.exceptions import ConvergenceWarning
from sklearn.svm import LinearSVC

from learning.predictor.linear_model.unitary_classifier import UnitaryClassifier


warnings.filterwarnings("ignore", category=ConvergenceWarning)


class SupportVectorMachine(UnitaryClassifier):
    """Linear SVR"""

    IS_RANK = False

    def _fit_impl(self, X, y, sample_weight):
        model = LinearSVC(random_state=0, max_iter=10000)
        model.fit(X, y, sample_weight=sample_weight)
        self._weights = model.coef_
        self._X = X
        self._y = y
        self._sample_weight = sample_weight

    def predict(self, X):
        return X @ self._weights.T
