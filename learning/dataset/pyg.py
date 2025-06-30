import logging

import torch
from sklearn.model_selection import train_test_split
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader

from learning.dataset.container.base_dataset import Dataset
from wlplan.graph_generator import GraphGenerator


def get_data_loaders(
    dataset: Dataset, graph_generator: GraphGenerator, batch_size: int
) -> tuple[DataLoader, DataLoader]:
    graphs = graph_generator.to_graphs(dataset.wlplan_dataset)
    pyg_dataset = []
    for graph in graphs:
        nodes = graph.node_colours
        edges = graph.edges

        # x is (n, d)
        x = torch.zeros(len(nodes), graph_generator.get_n_features())
        x[torch.arange(len(nodes)), nodes] = 1

        # edge_indices is list of (2, e)
        edge_indices = [torch.zeros(2, len(e)) for e in edges]
        for i, edge_group in enumerate(edges):
            for j, (a, b) in enumerate(edge_group):
                edge_indices[i][0, j] = a
                edge_indices[i][1, j] = b

        pyg_dataset.append(Data(x=x, edge_indices=edge_indices))

    train_set, val_set = train_test_split(pyg_dataset, test_size=0.15, random_state=4550)
    logging.info(f"{len(train_set)=}")
    logging.info(f"{len(val_set)=}")

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(val_set, batch_size=batch_size, shuffle=False)

    return train_loader, val_loader
