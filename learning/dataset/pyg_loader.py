import logging

import torch
from sklearn.model_selection import train_test_split
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader

from learning.dataset.creator.classic_labelled_dataset_creator import LabelledDataset
from learning.predictor.neural_network.policy_type import PolicyType
from wlplan.data import DomainDataset, ProblemDataset
from wlplan.graph_generator import GraphGenerator
from wlplan.planning import Domain


def _get_value_function_dataset(domain: Domain, dataset: LabelledDataset) -> DomainDataset:
    problem_dataset_list = []
    labels = []
    for data in dataset:
        problem = data.problem
        states = []
        for state_and_successors in data.states_and_successors_labelled:
            states.append(state_and_successors.state)  # s
            labels.append(state_and_successors.value)  # Q(s)
            for successors in state_and_successors.successors_labelled:
                succ = successors.successor_state  # s
                value = successors.value  # Q(s)
                if value is not None:
                    states.append(succ)
                    labels.append(value)

        problem_dataset_list.append(ProblemDataset(problem=problem, states=states))
    domain_dataset = DomainDataset(domain=domain, data=problem_dataset_list)
    return domain_dataset


def _get_quality_function_dataset(domain: Domain, dataset: LabelledDataset) -> DomainDataset:
    problem_dataset_list = []
    labels = []
    for data in dataset:
        problem = data.problem
        states = []
        actions = []
        for state_and_successors in data.states_and_successors_labelled:
            state = state_and_successors.state  # s
            value = state_and_successors.value  # do nothing with the current value
            for successors in state_and_successors.successors_labelled:
                succ_state = successors.successor_state  # do nothing with the succ_state
                succ_value = successors.value  # use the succ value as Q(s, a)
                action = successors.action  # a
                if succ_value is not None:
                    states.append(state)
                    actions.append(action)
                    labels.append(succ_value)

        problem_dataset_list.append(ProblemDataset(problem=problem, states=states, actions=actions))
    domain_dataset = DomainDataset(domain=domain, data=problem_dataset_list)
    return domain_dataset


def get_data_loaders(
    domain: Domain,
    dataset: LabelledDataset,
    graph_generator: GraphGenerator,
    batch_size: int,
    policy_type: str,
) -> tuple[DataLoader, DataLoader]:

    match policy_type:
        case PolicyType.VALUE_FUNCTION.value:
            domain_dataset = _get_value_function_dataset(domain, dataset)
        case PolicyType.QUALITY_FUNCTION.value:
            domain_dataset = _get_quality_function_dataset(domain, dataset)
        case _:
            raise ValueError(f"Unknown policy type: {policy_type}")

    graphs = graph_generator.to_graphs(domain_dataset)

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
