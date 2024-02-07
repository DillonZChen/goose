import time
import numpy as np
from typing import List
from dataset.factory import StateCostDataset
from models.sml.core import BaseModel
from dlplan.generator import generate_features
from dlplan.core import SyntacticElementFactory, DenotationsCaches


_MAX_VAL = 2147483647

class Model(BaseModel):
    def __init__(self, args) -> None:
        super().__init__(args)
        self.factory = None
        self.feature = None

    def parse_feature(self, feature_repr: str):
        feature_type = feature_repr[0]
        parse = {
            "b": self.factory.parse_boolean,
            "n": self.factory.parse_numerical,
            "c": self.factory.parse_concept,
            "r": self.factory.parse_role,
        }[feature_type]
        feature = parse(feature_repr)
        return feature

    def generate_features(self, dataset: StateCostDataset):
        self.factory = SyntacticElementFactory(dataset.vocabulary_info)

        states = []
        ys = []

        for data in dataset.state_cost_data_list:
            states.append(data.state)
            ys.append(data.cost)

        args = self._args
        print("Generating DLF...", flush=True)
        features = generate_features(
            self.factory,
            states,
            feature_limit=args.feature_limit,
            concept_complexity_limit=args.concept_complexity_limit,
            role_complexity_limit=args.role_complexity_limit,
            boolean_complexity_limit=args.boolean_complexity_limit,
            count_numerical_complexity_limit=args.count_numerical_complexity_limit,
            distance_numerical_complexity_limit=args.distance_numerical_complexity_limit,
        )
        features = sorted([f for f in features if f[0] in {"b", "n"}])
        self.features = features

        print("Computing DLF for training data...")
        t = time.time()
        xs = []
        for state in states:
            x = []
            caches = DenotationsCaches()
            for feature_repr in features:
                feature = self.parse_feature(feature_repr)
                val = int(feature.evaluate(state, caches))
                val = 0 if val == _MAX_VAL else val
                x.append(val)
            xs.append(x)
        xs = np.array(xs)
        t = time.time() - t
        print(f"Finished computing DLF for training data in {t:.2f}s")

        return xs, ys

    def get_features(self) -> List[str]:
        return self.features

    def setup_for_saving(self, save_file: str) -> None:
        super().setup_for_saving(save_file)
        self.factory = None  # cannot be pickled :(
