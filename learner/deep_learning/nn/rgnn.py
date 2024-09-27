from argparse import Namespace
from typing import List, Optional, Union

import torch
import torch.nn.functional as F
from torch import Tensor
from torch.nn import Linear, Module, ReLU, Sequential
from torch_geometric.data import Data
from torch_geometric.nn import MessagePassing

from learner.deep_learning.nn.base import Gnn, construct_mlp
from learner.deep_learning.representation.dynamic_feature import DynamicEvaluator


def identity(x: Tensor) -> Tensor:
    ## cannot use lambda function below as this causes pickling errors
    return x


class RGnn(Gnn):
    def __init__(
        self, in_dim: int, out_dim: int, n_edge_labels: int, opts: Namespace
    ) -> None:
        super().__init__(in_dim, out_dim, opts)
        self.n_edge_labels = n_edge_labels
        self.initialise_layers()

    def initialise_layers(self) -> None:
        self._emb = torch.nn.Linear(self.in_dim, self.nhid)
        self._layers = torch.nn.ModuleList()
        for _ in range(self.nlayers):
            self._layers.append(
                RGnnLayer(
                    self.nhid,
                    self.nhid,
                    n_edge_labels=self.n_edge_labels,
                    aggr=self.aggr,
                )
            )

        if self.jumping_knowledge:
            final_dim = self.nhid * (self.nlayers + 1)
        else:
            final_dim = self.nhid
        self._readout = Linear(final_dim, self.out_dim, bias=False)
        # self._readout = construct_mlp(final_dim, final_dim, self.out_dim)

        if self._opts.target in {"p", "d"}:
            self._activation = F.sigmoid  # for classification
        else:
            self._activation = identity

    def delete_layers(self) -> None:
        del self._emb
        del self._layers
        del self._readout

    def embed(
        self,
        x: Tensor,
        edge_indices: List[Tensor],
        d_eval: Union[List[DynamicEvaluator], DynamicEvaluator],
        batch: Optional[Tensor] = None,
    ) -> List[Tensor]:
        """useful to separate this function in case we only want graph emb"""
        # embed initial node features
        x = self._emb(x)

        if self.jumping_knowledge:
            intermediate = [x]

        # message passing layers
        for layer in self._layers:
            x = layer(x, edge_indices)
            x = F.leaky_relu(x)
            self.update_dynamic_features(x=x, d_eval=d_eval, batch=batch)
            if self.jumping_knowledge:
                intermediate.append(x)

        if self.jumping_knowledge:
            x = torch.cat(intermediate, dim=1)

        # pool all node embeddings
        x = self._pool(x, batch)
        return x

    def evaluate(
        self,
        x: Tensor,
        edge_indices: List[Tensor],
        d_eval: Union[List[DynamicEvaluator], DynamicEvaluator],
        batch: Optional[Tensor] = None,
    ) -> Tensor:
        x = self.embed(x=x, edge_indices=edge_indices, batch=batch, d_eval=d_eval)
        h = self._readout(x)
        h = h.squeeze(1)
        if self._opts.target != "h":
            h = self._activation(h)
        return h


class RGnnLayer(Module):
    def __init__(
        self,
        in_dim: int,
        out_dim: int,
        n_edge_labels: int,
        aggr: str,
    ):
        super(RGnnLayer, self).__init__()
        self.convs = torch.nn.ModuleList()
        for _ in range(n_edge_labels):
            conv = LinearConv(in_dim, out_dim, aggr=aggr)
            self.convs.append(conv)
        self.root = Linear(in_dim, out_dim, bias=True)
        return

    def forward(self, x: Tensor, edge_indices: List[Tensor]) -> Tensor:
        x_out = self.root(x)

        # bottleneck; difficult to parallelise efficiently
        # FastRGCNConv usually takes more memory and not faster
        for i, conv in enumerate(self.convs):
            x_out += conv(x, edge_indices[i])

        return x_out


class LinearConv(MessagePassing):
    def __init__(self, in_features: int, out_features: int, aggr: str) -> None:
        super().__init__(aggr=aggr)
        self.f = Linear(in_features, out_features, bias=False)

    def forward(self, x: Tensor, edge_index: Tensor) -> Tensor:
        x = self.f(x)
        x = self.propagate(edge_index=edge_index, x=x, size=None)
        return x
