""" Base class for graph representations """
import torch
import networkx as nx
from abc import ABC, abstractmethod
from enum import Enum
from typing import List, Tuple, Union, Iterable
from torch import Tensor
from .planning import get_planning_problem, Proposition

# grounded state is a list of facts represented as strings
GroundedState = Iterable[Proposition]

# lifted state is a list of facts represented as predicate and arguments
LiftedState = Iterable[Tuple[str, List[str]]]

# graph representation represented as a tensor for GNNs
TGraph = Union[Tuple[Tensor, Tensor], Tuple[Tensor, List[Tensor]]]

# graph representation represented as a nx.graph for graph kernels
CGraph = Union[nx.Graph, nx.DiGraph]


class AaaiAchievedWlColours(Enum):
    ACH_POS_GOAL = 100
    ACH_NEG_GOAL = 101
    ACH_NON_GOAL = 102


class Representation(ABC):
    @property
    def name(self):
        raise NotImplementedError

    @property
    def lifted(self):
        raise NotImplementedError

    def __init__(
        self,
        domain_pddl: str,
        problem_pddl: str,
        n_node_features: int,
        n_edge_labels: int,
    ) -> None:
        self.n_node_features = n_node_features
        self.n_edge_labels = n_edge_labels

        self._get_problem_info(domain_pddl, problem_pddl)

        self._pos_goal_nodes = set()
        self._neg_goal_nodes = set()
        self._compute_graph_representation()
        self.num_nodes = len(self.G.nodes)
        self.num_edges = len(self.G.edges)

        self.x = None
        self.edge_indices = None

        self.c_graph = None
        return

    def _get_problem_info(self, domain_pddl, problem_pddl) -> None:
        if self.name == "flg":
            self.problem = get_planning_problem(
                domain_pddl=domain_pddl, problem_pddl=problem_pddl, fdr=True
            )
            return

        if hasattr(self, "problem"):
            return

        self.domain_pddl = domain_pddl
        self.problem_pddl = problem_pddl

        self.problem = get_planning_problem(
            domain_pddl=domain_pddl, problem_pddl=problem_pddl, fdr=False
        )

        self.predicates = sorted(
            [p for p in self.problem.predicates if p.name != "="]
        )
        self.n_predicates = len(self.predicates)
        self.pred_to_idx = {
            pred.name: i for i, pred in enumerate(self.predicates)
        }
        largest_predicate = 0
        for pred in self.predicates:
            largest_predicate = max(largest_predicate, len(pred.arguments))
        self.largest_predicate_size = largest_predicate
        return

    def _init_graph(self) -> nx.Graph:
        """Initialises a networkx graph"""
        return nx.Graph()

    def _one_hot_node(self, index, size=-1) -> Tensor:
        """Returns a one hot tensor"""
        if size == -1:
            ret = torch.zeros(self.n_node_features)
        else:
            ret = torch.zeros(size)
        ret[index] = 1
        return ret

    def _zero_node(self) -> Tensor:
        """Returns a tensor of zeros"""
        ret = torch.zeros(self.n_node_features)
        return ret

    def _dump_stats(self) -> None:
        """Dump graph stats"""
        assert self.name is not None
        from util.stats import graph_density

        print(f"graph rep: {self.name}")
        print(f"num nodes: {self.num_nodes}")
        print(f"num edges: {self.num_edges}")
        print(
            f"graph density: {graph_density(self.num_nodes, self.num_edges, directed=self.directed)}"
        )
        return

    def convert_to_pyg(self) -> None:
        """Converts nx graph into pytorch_geometric tensors and stores them.

        The tensors are (x, edge_index or edge_indices)
        x: torch.tensor(N x F)  # N = num_nodes, F = num_features
        if n_edge_labels = 1:
          edge_index: torch.tensor(2 x E)  # E = num_edges
        else:
          edge_indices: List[torch.tensor(2 x E_i)]
        """
        from torch_geometric.utils.convert import from_networkx

        G: nx.Graph = self.G.copy()
        for node in G.nodes:
            G.nodes[node]["x"] = self._colour_to_tensor(G.nodes[node]["x"])
        pyg_G = from_networkx(G)
        self.x = pyg_G.x

        if self.n_edge_labels == 1:
            self.edge_indices = pyg_G.edge_index
        else:
            assert self.n_edge_labels > 1
            self.edge_indices = [[] for _ in range(self.n_edge_labels)]
            edge_index_T = pyg_G.edge_index.T
            for i, edge_label in enumerate(pyg_G.edge_label):
                self.edge_indices[edge_label].append(edge_index_T[i])
            for i in range(self.n_edge_labels):
                if len(self.edge_indices[i]) > 0:
                    self.edge_indices[i] = (
                        torch.vstack(self.edge_indices[i]).long().T
                    )
                else:
                    self.edge_indices[i] = torch.tensor([[], []]).long()
        return

    def write_to_file(self) -> None:
        from datetime import datetime

        df = self.domain_pddl
        pf = self.problem_pddl
        t = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
        file_path = "_".join(["graph", df, pf, t])
        file_path = repr(hash(file_path)).replace("-", "n")
        file_path = file_path + ".graph"

        def node_to_name(node_name):
            for symbol in [" ", "'", ")", "("]:
                node_name = node_name.replace(symbol, "")
            if node_name[-1] == ",":
                node_name = node_name[:-1]
            return node_name

        G = self.G

        with open(file_path, "w") as f:
            # line number = node
            # <node_name> <node_colour> [<neighbour_node> <edge_label>]
            f.write(f"{len(G.nodes)} nodes\n")
            for u in G.nodes:
                # node_name = str(self._node_to_name[u])
                node_name = str(u)
                node_name = node_to_name(node_name)
                f.write(f"{node_name} {G.nodes[u]['x']} ")
                for v in G[u]:
                    v_index = self._node_to_i[v]
                    f.write(f"{v_index} {int(G[u][v]['edge_label'])} ")
                f.write("\n")

            f.write(f"{len(self._pos_goal_nodes)} pos goals\n")
            for node_name in self._pos_goal_nodes:
                node_name = str(node_name)
                node_name = node_to_name(node_name)
                f.write(node_name + "\n")

            f.write(f"{len(self._neg_goal_nodes)} neg goals\n")
            for node_name in self._neg_goal_nodes:
                node_name = str(node_name)
                node_name = node_to_name(node_name)
                f.write(node_name + "\n")

            f.write(f"{len(self.pred_to_idx)} predicates\n")
            for pred, i in self.pred_to_idx.items():
                f.write(f"{pred} {i}\n")

            f.write(f"{self.largest_predicate_size} largest_predicate_size\n")

        self._graph_file_path = file_path
        # import os; os.system(f"cp {self._graph_file_path} debug.graph")
        return

    def get_graph_file_path(self) -> str:
        return self._graph_file_path

    @abstractmethod
    def _compute_graph_representation(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def str_to_state(self, state) -> Union[GroundedState, LiftedState]:
        raise NotImplementedError

    @abstractmethod
    def _colour_to_tensor(self, colour: int) -> Tensor:
        raise NotImplementedError

    @abstractmethod
    def state_to_tgraph(self, state) -> TGraph:
        raise NotImplementedError

    @abstractmethod
    def state_to_cgraph(self, state) -> CGraph:
        raise NotImplementedError
