from abc import ABC, abstractmethod

import numpy as np

from util.custom_decorators import abstract_class_attributes
from util.timer import TimerContextManager


@abstract_class_attributes("IS_RANK")
class LinearModel(ABC):
    def __init__(self) -> None:
        super().__init__()
        self._class_name = self.__class__.__name__
        self._weights = None

    def fit_evaluate(self, X, y, sample_weight):
        with TimerContextManager("training model"):
            self.fit(X, y, sample_weight)
        with TimerContextManager("evaluating model"):
            self.evaluate()

    def fit(self, X, y, sample_weight=None):
        if sample_weight is None:
            sample_weight = np.ones(len(y))
        self._fit_impl(X, y, sample_weight)

    def evaluate(self):
        if self._weights is None:
            raise RuntimeError("Model has not been trained yet. Call `fit` to train the model.")
        self._evaluate_impl()

    @abstractmethod
    def _fit_impl(self, X, y, sample_weight):
        pass

    @abstractmethod
    def predict(self, X):
        pass

    @abstractmethod
    def _evaluate_impl(self):
        """Evaluation of training data after calling fit"""
        pass

    def get_weights(self) -> list:
        if self._weights is None:
            raise RuntimeError("Model has not been trained yet. Call `fit` to train the model.")
        ret = self._weights
        if isinstance(ret, np.ndarray):
            ret = ret.tolist()
        return ret
