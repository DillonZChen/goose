import numpy as np
from models.sml.core import BaseModel, ALL_KEY
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

    def convert_training_data(self, problem_states_dict, vocabulary_info):
        self.factory = SyntacticElementFactory(vocabulary_info)

        states_to_y = {}
        for _, states in problem_states_dict.items():
            for state, y in states.items():
                states_to_y[state] = y

        args = self._args
        features = generate_features(
            self.factory,
            list(states_to_y.keys()),
            feature_limit=args.feature_limit,
            concept_complexity_limit=args.concept_complexity,
            role_complexity_limit=args.role_complexity,
            boolean_complexity_limit=args.boolean_complexity,
            count_numerical_complexity_limit=args.count_num_complexity,
            distance_numerical_complexity_limit=args.distance_num_complexity,
        )
        features = sorted([f for f in features if f[0] in {"b", "n"}])

        xs = []
        ys = []

        for state, y in tqdm(states_to_y.items()):
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
            ys.append(y)

        xs = np.array(xs)
        ys = np.array(ys)

        return xs, ys
