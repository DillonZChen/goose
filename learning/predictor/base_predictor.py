from abc import ABC, abstractmethod

import numpy as np


class BasePredictor(ABC):
    def __init__(self) -> None:
        super().__init__()
        self._weights = None

    @abstractmethod
    def fit(self, X, y):
        pass

    @abstractmethod
    def predict(self, X):
        pass

    def get_weights(self) -> list:
        if self._weights is None:
            raise ValueError("Model has not been trained yet. Call `fit` to train the model.")
        ret = self._weights
        if isinstance(ret, np.ndarray):
            ret = ret.tolist()
        return ret
