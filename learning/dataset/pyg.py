import logging

import torch
from sklearn.model_selection import train_test_split
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader

from wlplan.data import DomainDataset
from wlplan.graph_generator import GraphGenerator


def get_data_loaders(
    dataset: DomainDataset, labels: list[int], graph_generator: GraphGenerator, batch_size: int
) -> tuple[DataLoader, DataLoader]:
    graphs = graph_generator.to_graphs(dataset)

    pyg_dataset = []
    for graph, y in zip(graphs, labels):
        nodes = graph.node_colours
        edges = graph.edges

        # x is (n, d)
        x = torch.zeros(len(nodes), graph_generator.get_n_features())
        x[torch.arange(len(nodes)), nodes] = 1

        # edge_indices is list of (2, e)
        edge_indices_list = [[[], []] for _ in range(graph_generator.get_n_relations())]
        for u, neighbours in enumerate(edges):
            for r, v in neighbours:
                edge_indices_list[r][0].append(u)
                edge_indices_list[r][1].append(v)
        edge_indices_list = [torch.tensor(edges, dtype=torch.long) for edges in edge_indices_list]

        # Must use `edge_index` variable to correctly combine edge_indices_lists [(2, num_edges)]
        data = Data(x=x, edge_index=edge_indices_list, y=y)
        pyg_dataset.append(data)

    train_set, val_set = train_test_split(pyg_dataset, test_size=0.15, random_state=4550)
    logging.info(f"{len(train_set)=}")
    logging.info(f"{len(val_set)=}")

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(val_set, batch_size=batch_size, shuffle=False)

    return train_loader, val_loader
