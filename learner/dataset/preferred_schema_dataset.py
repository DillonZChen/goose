from argparse import Namespace

import numpy as np
from sklearn.metrics import accuracy_score, precision_score, recall_score
from sklearn.model_selection import train_test_split

from learner.dataset.dataset import Dataset
from learner.representation import Representation
from util.statistics import dump_several_stats, f1_macro
from util.timer import TimerContextManager


def optimal_actions_to_multilabel_schema(schemata, optimal_actions):
    y_dict = {s: 0 for s in schemata}
    for action in optimal_actions:
        found = False
        # this is slow but an effect of unified_planning
        for s in schemata:
            if s in action:
                y_dict[s] = 1
                found = True
        assert found, f"Action {action} not schemata {schemata}"
    return y_dict


class PreferredSchemaDataset(Dataset):
    def __init__(self, opts: Namespace, representation: Representation) -> None:
        super().__init__(opts, representation)

        if opts.schemata_strategy != "each":
            raise ValueError("Expected schemata_strategy to be 'each'.")

        with TimerContextManager("converting data to model inputs"):
            raw_dataset = self._raw_dataset
            self.dataset = self.representation.transform_prefschema_dataset(raw_dataset)

        self._schemata_keys = next(iter(raw_dataset.keys())).schemata_names

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
        else:
            X_tr, X_val, y_tr_true, y_va_true = _X, [], _y_true, []

        print("X_tr shape:", X_tr.shape)
        print("X_val shape:", X_val.shape)

        # reshape y from array of dicts to dict of arrays
        y_tr_true = {k: np.array([d[k] for d in y_tr_true]) for k in self.schemata_keys}
        y_va_true = {k: np.array([d[k] for d in y_va_true]) for k in self.schemata_keys}

        return X_tr, X_val, y_tr_true, y_va_true

    def get_metrics(self):
        return {
            "acc": accuracy_score,
            "prec": precision_score,
            "rec": recall_score,
            "f1": f1_macro,
        }
