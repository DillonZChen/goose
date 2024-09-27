from argparse import Namespace
from typing import Dict

import numpy as np
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split

from learner.dataset.dataset import ALL_KEY, Dataset
from learner.representation import Representation
from util.statistics import dump_several_stats, f1_macro
from util.timer import TimerContextManager


class HeuristicDataset(Dataset):
    def __init__(self, opts: Namespace, representation: Representation) -> None:
        super().__init__(opts, representation)

        s_strat = opts.schemata_strategy
        if s_strat != "all":
            raise NotImplementedError("TODO: determine a canonical set. e.g. min/max")

        self._schemata_keys = {ALL_KEY}

        with TimerContextManager("converting data to model inputs"):
            raw_dataset = self._raw_dataset
            self.dataset = self.representation.transform_heuristic_dataset(raw_dataset)

        if len(self.dataset) == 0:
            print("Training data is empty! Terminating.")
            exit(0)

    def get_dataset_split(self):
        val_ratio = self._opts.val_ratio
        seed = self._opts.seed
        
        # cannot convert to array yet, bunch of dicts
        _y_true = [d[1] for d in self.dataset]
        _X = np.array([d[0] for d in self.dataset])

        print("X shape:", _X.shape)
        train_all = val_ratio == 0
        if not train_all:
            X_tr, X_val, y_tr_true, y_va_true = train_test_split(
                _X, _y_true, test_size=val_ratio, random_state=seed
            )
            print("X_tr shape:", X_tr.shape)
            print("X_val shape:", X_val.shape)
        else:
            X_tr, X_val, y_tr_true, y_va_true = _X, [], _y_true, []
        

        # reshape y from array of dicts to dict of arrays
        y_tr_true = {k: np.array([d[k] for d in y_tr_true]) for k in self.schemata_keys}
        y_va_true = {k: np.array([d[k] for d in y_va_true]) for k in self.schemata_keys}

        return X_tr, X_val, y_tr_true, y_va_true

    def get_metrics(self) -> Dict[str, callable]:
        return {"mse": mean_squared_error, "f1": f1_macro}
