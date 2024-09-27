from enum import Enum
from itertools import product
from typing import Dict, List, Optional, Tuple, Union

import torch
from torch import Tensor

from learner.problem.numeric_domain import NumericDomain
from util.statistics import print_mat

Assignments = Dict[int, Union[float, bool]]


class ObjectFeature(Enum):
    # no multiplication, object types too annoying due to tree structure
    # arg encodings and predicate definitions should make object types implicit
    TYPE = 0


class FactFeature(Enum):
    # each is multiplied by number of fact predicates
    T_GOAL = 0
    F_GOAL = 1
    T_FACT = 2


class FluentFeature(Enum):
    # each is multiplied by number of fluent predicates
    PREDICATE = 0
    VALUE = 1


class NumericalGoalFeature(Enum):
    # no multiplication
    IS_NUMERICAL_GOAL = 0
    NUMERICAL_GOAL_ERROR = 1
    NUMERICAL_GOAL_ACHIEVED = 2
    INEQUALITY = 3
    EQUALITY = 4

    @staticmethod
    def condition_type(comparator: str):
        if comparator in {"<=", "<"}:
            condition_type = NumericalGoalFeature.INEQUALITY
        else:
            assert comparator == "=="
            condition_type = NumericalGoalFeature.EQUALITY
        return condition_type


class FeatureGenerator:
    def __init__(self, domain_pddl):
        self.domain = NumericDomain(domain_pddl)

        predicates: List[str] = list(map(str, self.domain.predicates))
        functions: List[str] = list(map(str, self.domain.functions))
        types: List[str] = list(map(str, self.domain.object_types))

        self._pred_to_idx: Dict[str, int] = {
            p: i for i, p in enumerate(predicates)
        }
        self._func_to_idx: Dict[str, int] = {
            p: i for i, p in enumerate(functions)
        }
        self._type_to_idx: Dict[str, int] = {t: i for i, t in enumerate(types)}

        n_preds = len(predicates)
        n_funcs = len(functions)
        n_types = len(types)

        self._obj_inc = 0
        self._fact_inc = len(ObjectFeature) * 1
        self._fluent_inc = len(FactFeature) * n_preds + self._fact_inc
        self._num_goal_inc = len(FluentFeature) * n_funcs + self._fluent_inc

        self.n_features = len(NumericalGoalFeature) + self._num_goal_inc

        # test_feature_indices
        seen_indices = set()
        seen_indices.add(0)
        # for obj_type in types:
        #     idx = self.object_node_feat_idx(obj_type)
        #     assert idx not in seen_indices
        #     seen_indices.add(idx)
        for pred, feature in product(predicates, FactFeature):
            idx = self.fact_node_feat_idx(pred, feature)
            assert idx not in seen_indices
            seen_indices.add(idx)
        for pred, feature in product(functions, FluentFeature):
            idx = self.fluent_node_feat_idx(pred, feature)
            assert idx not in seen_indices
            seen_indices.add(idx)
        for feature in NumericalGoalFeature:
            idx = self.num_goal_node_feat_idx(feature)
            assert idx not in seen_indices
            seen_indices.add(idx)
        assert len(seen_indices) == self.n_features

        # for dealing with tensors
        self._new_rows = None
        self._reset_new_rows_cache()

    @property
    def num_goal_offset(self) -> int:
        return self._num_goal_inc

    @property
    def fluent_offset(self) -> int:
        return self._fluent_inc

    def _reset_new_rows_cache(self) -> None:
        self._new_rows = []

    def _zero_vector(self) -> Tensor:
        return torch.zeros(self.n_features)

    # get feature indices
    def object_node_feat_idx(self, object_type: str) -> int:
        idx = self._obj_inc  # = 0
        # idx += len(ObjectFeature)
        # idx += self._type_to_idx[object_type] * len(ObjectFeature)
        return idx

    def fact_node_feat_idx(self, predicate: str, feature: FactFeature) -> int:
        idx = self._fact_inc
        idx += self._pred_to_idx[predicate] * len(FactFeature)
        idx += feature.value
        return idx

    def fluent_node_feat_idx(self, predicate: str, feature: FluentFeature) -> int:
        idx = self._fluent_inc
        idx += self._func_to_idx[predicate] * len(FluentFeature)
        idx += feature.value
        return idx

    def num_goal_node_feat_idx(self, feature: NumericalGoalFeature) -> int:
        idx = self._num_goal_inc
        idx += feature.value
        return idx

    # construct feature
    def feature(self, assignments: Assignments) -> Tensor:
        ret = self._zero_vector()
        for f_idx, val in assignments.items():
            ret[f_idx] = float(val)
        return ret

    # done in place
    def update_x(self, x: Tensor, idx: int, assignments: Assignments) -> None:
        for f_idx, val in assignments.items():
            x[idx][f_idx] = float(val)

    def cache_add_node(self, x: Tensor, assignments: Assignments) -> int:
        i = x.shape[0] + len(self._new_rows)
        new_row = torch.zeros(self.n_features)
        for f_idx, val in assignments.items():
            new_row[f_idx] = float(val)
        self._new_rows.append(new_row)
        return i

    # NOT done in place
    def add_cached_nodes(self, x: Tensor) -> Tensor:
        if len(self._new_rows) == 0:
            return x
        new_rows = torch.stack(self._new_rows)
        x = torch.cat((x, new_rows), dim=0)
        self._reset_new_rows_cache()
        return x

    def dump(self) -> None:
        print("# node features:", self.n_features)
        features = []
        features.append(["object", self.object_node_feat_idx("")])
        for pred, feature in product(self._pred_to_idx, FactFeature):
            features.append([(pred, feature), self.fact_node_feat_idx(pred, feature)])
        for pred, feature in product(self._func_to_idx, FluentFeature):
            features.append([(pred, feature), self.fluent_node_feat_idx(pred, feature)])
        for feature in NumericalGoalFeature:
            features.append([feature, self.num_goal_node_feat_idx(feature)])
        print_mat(features)
