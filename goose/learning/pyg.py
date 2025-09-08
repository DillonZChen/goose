import logging
from dataclasses import dataclass
from typing import Any, Optional

import torch
from sklearn.model_selection import train_test_split
from torch import Tensor
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader
from wlplan.data import DomainDataset
from wlplan.graph_generator import Graph, GraphGenerator
from wlplan.planning import Action, Problem, State


@dataclass
class PyGGraph:
    x: Tensor
    edge_index: list[Tensor]  # list index corresponds to edge label

    def to(self, device: torch.device) -> "PyGGraph":
        return PyGGraph(
            x=self.x.to(device),
            edge_index=[edge_index.to(device) for edge_index in self.edge_index],
        )


def wlplan_state_to_pyg(
    graph_generator: GraphGenerator,
    problem: Problem,
    state: State,
    actions: Optional[list[Action]] = None,
) -> PyGGraph:
    graph_generator.set_problem(problem)
    if actions is None:
        graph = graph_generator.to_graph(state)
    else:
        graph = graph_generator.to_graph(state, actions)
    return wlplan_graph_to_pyg(graph_generator, graph)


def wlplan_graph_to_pyg(graph_generator: GraphGenerator, graph: Graph) -> PyGGraph:
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

    return PyGGraph(x=x, edge_index=edge_indices_list)


def get_data_loaders(
    domain_dataset: DomainDataset,
    labels: Any,
    graph_generator: GraphGenerator,
    batch_size: int,
) -> tuple[DataLoader, DataLoader]:

    graphs = graph_generator.to_graphs(domain_dataset)

    pyg_dataset = []
    for graph, y in zip(graphs, labels):
        pyg_graph = wlplan_graph_to_pyg(graph_generator=graph_generator, graph=graph)

        # Must use `edge_index` variable to correctly combine edge_indices_lists [(2, num_edges)]
        data = Data(x=pyg_graph.x, edge_index=pyg_graph.edge_index, y=y)
        pyg_dataset.append(data)

    train_set, val_set = train_test_split(pyg_dataset, test_size=0.15, random_state=4550)
    logging.info(f"{len(train_set)=}")
    logging.info(f"{len(val_set)=}")

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(val_set, batch_size=batch_size, shuffle=False)

    return train_loader, val_loader
