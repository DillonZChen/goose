from abc import ABC, abstractmethod
from argparse import Namespace
from typing import List, Optional, Union

import torch
import torch.nn.functional as F
import torch_geometric
from torch import Tensor
from torch.nn import Linear, Module, ReLU, Sequential
from torch_geometric.data import Data

from learner.deep_learning.representation.dynamic_feature import DynamicEvaluator


def construct_mlp(in_dim, hid_dim, out_dim):
    return Sequential(Linear(in_dim, hid_dim), ReLU(), Linear(hid_dim, out_dim))


class Gnn(ABC, Module):
    def __init__(self, in_dim: int, out_dim: int, opts: Namespace) -> None:
        super().__init__()
        self.in_dim = in_dim
        self.out_dim = out_dim
        self.aggr = opts.aggr
        self.nhid = opts.nhid
        self.nlayers = opts.nlayers
        self.dynamic_features = opts.dynamic_features
        self.jumping_knowledge = opts.jumping_knowledge
        self._opts = opts

        self._pool = {
            "max": torch_geometric.nn.global_max_pool,
            "mean": torch_geometric.nn.global_mean_pool,
            "sum": torch_geometric.nn.global_add_pool,
        }[opts.pool]

    @abstractmethod
    def initialise_layers(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def delete_layers(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def evaluate(
        self,
        x: Tensor,
        edge_indices: List[Tensor],
        d_eval: DynamicEvaluator,
        batch: Optional[Tensor] = None,
    ) -> Tensor:
        raise NotImplementedError

    def forward(self, data: Data) -> Tensor:
        return self.evaluate(
            x=data.x,
            edge_indices=data.edge_index,
            batch=data.batch,
            d_eval=data.d_eval,
        )

    def update_dynamic_features(
        self,
        x: Tensor,
        d_eval: Union[List[DynamicEvaluator], DynamicEvaluator],
        batch: Optional[Tensor] = None,
    ) -> Tensor:
        # this operation is done in place on x
        if not self.dynamic_features:
            return

        if batch is not None:
            # goodbye speedup from batching... TODO somehow optimise
            offsets = torch.bincount(batch, minlength=len(d_eval))
            offsets = torch.roll(offsets, 1)
            offsets[0] = 0

            for offset, d_eval_unbatched in zip(offsets, d_eval):
                d_eval_unbatched.transform(x, offset.item())
        else:
            d_eval.transform(x, offset=0)

    @property
    def name(self) -> str:
        return type(self).__name__

    @property
    def num_parameters(self) -> int:
        """Count number of weight parameters"""
        # https://stackoverflow.com/a/62764464/13531424
        # e.g. to deal with case of sharing layers
        params = sum(
            dict(
                (p.data_ptr(), p.numel()) for p in self.parameters() if p.requires_grad
            ).values()
        )
        return params

    def dump(self) -> None:
        print(f"GNN configuration:")
        print(f"name: {self.name}")
        print(f"in_dim: {self.in_dim}")
        print(f"out_dim: {self.out_dim}")
        print(f"nhid: {self.nhid}")
        print(f"nlayers: {self.nlayers}")
        print(f"aggr: {self.aggr}")
        print(f"pool: {self._opts.pool}")
        print(f"num_parameters: {self.num_parameters}")
