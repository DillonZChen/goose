import argparse
from typing import List, Optional

import torch
import torch.nn as nn
import torch.nn.functional as F
from torch import Tensor
from torch.nn import Linear, ReLU, Sequential
from torch.nn.modules.module import Module
from torch_geometric.nn import (
    MessagePassing,
    global_add_pool,
    global_max_pool,
    global_mean_pool,
)


def construct_mlp(in_features: int, out_features: int, n_hid: int) -> torch.nn.Module:
    return Sequential(
        Linear(in_features, n_hid),
        ReLU(),
        Linear(n_hid, out_features),
    )


class RGNNLayer(Module):
    def __init__(self, n_relations: int, in_feat: int, out_feat: int, aggr: str) -> None:
        super(RGNNLayer, self).__init__()
        self.convs = torch.nn.ModuleList()
        for _ in range(n_relations):
            self.convs.append(LinearConv(in_feat, out_feat, aggr=aggr))
        self.root = Linear(in_feat, out_feat, bias=True)
        return

    def forward(self, x: Tensor, edge_indices_list: List[Tensor]) -> Tensor:
        x_out = self.root(x)
        # bottleneck; difficult to parallelise efficiently
        for i, conv in enumerate(self.convs):
            x_out += conv(x, edge_indices_list[i])
        return x_out


class LinearConv(MessagePassing):
    propagate_type = {"x": Tensor}

    def __init__(self, in_features: int, out_features: int, aggr: str) -> None:
        super().__init__(aggr=aggr)
        self.f = Linear(in_features, out_features, bias=False)

    def forward(self, x: Tensor, edge_index: Tensor) -> Tensor:
        # propagate_type = {'x': Tensor }
        x = self.f(x)
        x = self.propagate(edge_index=edge_index, x=x, size=None)
        return x


class RGNN(nn.Module):
    def __init__(
        self,
        n_relations: int,
        in_feat: int,
        out_feat: int,
        n_hid: int,
        n_layers: int,
        aggr: str,
        pool: str,
    ) -> None:
        super().__init__()
        self.n_relations = n_relations
        self.in_feat = in_feat
        self.out_feat = out_feat
        self.n_hid = n_hid
        self.n_layers = n_layers
        self.aggr = aggr
        self.pool = pool

        match pool:
            case "max":
                self.pool_fn = global_max_pool
            case "mean":
                self.pool_fn = global_mean_pool
            case "sum":
                self.pool_fn = global_add_pool
            case _:
                raise ValueError(f"Unknown value {pool=}")

        self.initialise_layers()

    @staticmethod
    def init_from_opts(opts: argparse.Namespace) -> "RGNN":
        return RGNN(
            n_relations=opts._n_relations,
            in_feat=opts._n_features,
            out_feat=1,
            n_hid=opts.num_hidden,
            n_layers=opts.iterations,
            aggr="max",
            pool="sum",
        )

    def initialise_layers(self) -> None:
        self.emb = torch.nn.Linear(self.in_feat, self.n_hid)
        self.layers = torch.nn.ModuleList()
        for _ in range(self.n_layers):
            layer = RGNNLayer(n_relations=self.n_relations, in_feat=self.n_hid, out_feat=self.n_hid, aggr=self.aggr)
            self.layers.append(layer)
        self.mlp = construct_mlp(in_features=self.n_hid, n_hid=self.n_hid, out_features=self.out_feat)

    def node_embedding(self, x: Tensor, edge_indices_list: List[Tensor]) -> Tensor:
        x = self.emb(x)
        for layer in self.layers:
            x = layer(x, edge_indices_list)
            x = F.relu(x)
        return x

    def graph_embedding(self, x: Tensor, edge_indices_list: List[Tensor], batch: Optional[Tensor]) -> Tensor:
        x = self.node_embedding(x, edge_indices_list)
        x = self.pool_fn(x, batch)
        return x

    def forward(self, x: Tensor, edge_indices_list: List[Tensor], batch: Optional[Tensor]) -> Tensor:
        x = self.graph_embedding(x, edge_indices_list, batch)
        h = self.mlp(x)
        h = h.squeeze(1)
        return h
