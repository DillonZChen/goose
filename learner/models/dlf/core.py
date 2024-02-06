import numpy as np
from dataset.factory import StateCostDataset
from models.sml.core import BaseModel
from dlplan.generator import generate_features
from dlplan.core import SyntacticElementFactory
from tqdm import tqdm


class Model(BaseModel):
    def __init__(self, args) -> None:
        super().__init__(args)
        self.factory = None

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

    def convert_training_data(self, dataset: StateCostDataset):
        self.factory = SyntacticElementFactory(dataset.vocabulary_info)

        states = []
        ys = []

        for data in dataset.state_cost_data_list:
            states.append(data.state)
            ys.append(data.cost)

        args = self._args
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

        xs = []

        for state in tqdm(states):
            x = []

            ## using caches does not give us a speed up
            # caches = DenotationsCaches()
            # for feature_repr in features:
            #     feature = parse_feature(feature_repr, factory)
            #     x.append(int(feature.evaluate(state, caches)))

            for feature_repr in features:
                feature = self.parse_feature(feature_repr)
                x.append(int(feature.evaluate(state)))

            xs.append(x)

        xs = np.array(xs)

        return xs, ys
