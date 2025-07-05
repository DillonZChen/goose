import logging

import numpy as np
from sklearn.metrics import f1_score, mean_squared_error

from learning.predictor.linear_model.linear_model import LinearModel


class UnitaryRegressor(LinearModel):
    IS_RANK = False

    def _evaluate_impl(self):
        X, y = self._X, self._y
        y_pred = self.predict(X)
        mse_loss = mean_squared_error(y, y_pred, sample_weight=self._sample_weight)
        logging.info(f"{mse_loss=}")
        y_pred = np.round(y_pred)
        f1_macro = f1_score(y, y_pred, average="macro", sample_weight=self._sample_weight)
        logging.info(f"{f1_macro=}")
