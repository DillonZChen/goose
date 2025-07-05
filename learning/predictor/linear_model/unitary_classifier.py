import logging

import numpy as np
from sklearn.metrics import f1_score, log_loss

from learning.predictor.linear_model.linear_model import LinearModel


class UnitaryClassifier(LinearModel):
    IS_RANK = False

    def _evaluate_impl(self):
        X, y = self._X, self._y
        y_pred = self.predict(X)
        bce_loss = log_loss(y, y_pred, sample_weight=self._sample_weight)
        logging.info(f"{bce_loss=}")
        y_pred = np.round(y_pred)
        f1_macro = f1_score(y, y_pred, average="macro", sample_weight=self._sample_weight)
        logging.info(f"{f1_macro=}")

    def get_weights(self) -> list:
        ret = super().get_weights()[0]
        return ret
