import logging

import numpy as np
from sklearn.metrics import f1_score

from .base_predictor import BasePredictor


class MSEMinimiser(BasePredictor):
    IS_RANK = False

    def evaluate(self):
        X, y = self._X, self._y
        y_pred = self.predict(X)
        mse_loss = np.mean((y - y_pred) ** 2)
        logging.info(f"{mse_loss=}")
        y_pred = np.round(y_pred)
        f1_macro = f1_score(y, y_pred, average="macro")
        logging.info(f"{f1_macro=}")
