from enum import Enum
from typing import List, Optional

import torch
from torch import Tensor

from learner.problem.numeric_domain import NumericDomain
from util.statistics import print_mat


# these are not the actual edge label indices, but the groups
class LabelType(Enum):
    NUM_VAR_ARG = 0
    BOOL_VAR_ARG = 1
    NUM_GOAL = 2


class LabelGenerator:
    def __init__(self, domain_pddl):
        self.domain = NumericDomain(domain_pddl)

        self.max_fluent_arity = self.domain.max_func_arity
        self.max_fact_arity = self.domain.max_pred_arity

        offset = 1
        self._num_var_arg_to_idx = {i: i + offset for i in range(self.max_fluent_arity)}

        offset += self.max_fluent_arity
        self._bool_var_arg_to_idx = {i: i + offset for i in range(self.max_fact_arity)}

        self.n_labels = 1 + self.max_fluent_arity + self.max_fact_arity
        assert offset + self.max_fact_arity == self.n_labels

        self._new_edges_cache = None
        self._reset_new_edges_cache()

    @property
    def bool_label_offset(self) -> int:
        return 1 + self.max_fluent_arity

    def _reset_new_edges_cache(self) -> None:
        self._new_edges_cache = {i: [] for i in range(self.n_labels)}

    def num_var_arg_label(self, arg: int) -> int:
        assert arg in self._num_var_arg_to_idx, arg
        return self._num_var_arg_to_idx[arg]

    def bool_var_arg_label(self, arg: int) -> int:
        assert arg in self._bool_var_arg_to_idx, arg
        return self._bool_var_arg_to_idx[arg]

    def goal_label(self) -> int:
        return 0

    def label_type(self, label: int) -> LabelType:
        if label == 0:
            return LabelType.NUM_GOAL
        if label in self._num_var_arg_to_idx.values():
            return LabelType.NUM_VAR_ARG
        if label in self._bool_var_arg_to_idx.values():
            return LabelType.BOOL_VAR_ARG
        raise ValueError(f"Unknown label {label}")

    def label_and_arg_to_index(self, label_type: LabelType, arg: Optional[int]) -> int:
        if label_type == LabelType.BOOL_VAR_ARG:
            return 0
        if label_type == LabelType.NUM_VAR_ARG:
            return self.num_var_arg_label(arg)
        if label_type == LabelType.BOOL_VAR_ARG:
            return self.bool_var_arg_label(arg)
        raise ValueError(f"Unknown label {label_type}")

    def cache_add_edge(self, i: int, j: int, label: int) -> None:
        self._new_edges_cache[label].append((i, j))
        self._new_edges_cache[label].append((j, i))

    def add_cached_edges(self, edge_indices: List[Tensor]) -> Tensor:
        for i, new_edges in self._new_edges_cache.items():
            if len(new_edges) == 0:
                continue
            new_edges = torch.tensor(new_edges).T
            edge_indices[i] = torch.hstack((edge_indices[i], new_edges)).long()
        self._reset_new_edges_cache()
        return edge_indices

    def dump(self) -> None:
        print(f"# edge labels: {self.n_labels}")
        features = []
        features.append(["numeric_goal", self.goal_label()])
        for arg in range(self.max_fluent_arity):
            features.append([f"fluent_arg_{arg}", self.num_var_arg_label(arg)])
        for arg in range(self.max_fact_arity):
            features.append([f"fact_arg_{arg}", self.bool_var_arg_label(arg)])
        print_mat(features)
