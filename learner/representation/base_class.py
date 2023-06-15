import sys
import matplotlib.pyplot as plt
import torch
import networkx as nx
import copy
import time
import torch.nn.functional as F
import signal
import os
import util
import random

from typing import FrozenSet, List, NamedTuple, TypeVar, Tuple, Dict, Optional, Union
from torch import Tensor
from asg.instantiate import instantiate, explore
from enum import Enum

from util import get_domain_name, get_problem_name
from planning import get_strips_problem
from planning import Proposition
from util.stats import graph_density
from torch_geometric.loader import DataLoader
from torch_geometric.data import Data
from torch_geometric.utils.convert import to_networkx, from_networkx
from tqdm import tqdm
from abc import ABC, abstractmethod
from tqdm.auto import tqdm
from .config import CONFIG


""" Graph representations """

class Representation(ABC):
  def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
    self.num_nodes = None
    self.num_edges = None
    self.domain_pddl = domain_pddl
    self.problem_pddl = problem_pddl
    self.store_applicable_actions = False
    self.rep_name = None
    self.state = None

    if (domain_pddl == "" and problem_pddl == ""):
      self.problem = None
      self.domain_name = ""
      self.problem_name = ""
      self.s0 = set()
      self.n_prop = 0
      self.n_act = 0
    else:
      self.problem = get_strips_problem(domain_pddl=self.domain_pddl,
                                        problem_pddl=self.problem_pddl)

    self.graph_data = None
    self.x = None
    self.node_dim = None
    self.edge_dim = None
    self.edge_type = None
    self.action = {}
    self._init()
    self.directed = CONFIG[self.rep_name]["directed"]
    return

  def _create_graph(self) -> Union[nx.Graph, nx.DiGraph]:
    return nx.DiGraph() if self.directed else nx.Graph()

  def _one_hot_node(self, index, size=-1) -> Tensor:
    if size==-1:
      ret = torch.zeros(self.node_dim)
    else:
      ret = torch.zeros(size)
    ret[index] = 1
    return ret

  def _one_hot_edge(self, index) -> Tensor:
    ret = torch.zeros(self.edge_dim)
    ret[index] = 1
    return ret

  def _zero_node(self) -> Tensor:
    ret = torch.zeros(self.node_dim)
    return ret

  def _zero_edge(self) -> Tensor:
    ret = torch.zeros(self.edge_dim)
    return ret

  def _dump_stats(self, start_time) -> None:
    assert self.rep_name is not None
    tqdm.write(f'{self.rep_name} for {self.problem.name} created!')
    tqdm.write(f'time taken: {time.time() - start_time:.4f}s')
    tqdm.write(f'num nodes: {self.num_nodes}')
    tqdm.write(f'num edges: {self.num_edges}')
    tqdm.write(f'graph density: {graph_density(self.num_nodes, self.num_edges, directed=self.directed)}')
    return
  
  def _graph_to_representation(self, G) -> None:
    pass
  
  def _graph_to_el_representation(self, G) -> None:
    self.G = G
    pyg_G = from_networkx(G)
    self.x = pyg_G.x

    edge_index_T = pyg_G.edge_index.T
    self.edge_indices = [[] for _ in range(self.n_edge_types)]
    for i, edge_type in enumerate(pyg_G.edge_type):   # this is slow
      self.edge_indices[edge_type].append(edge_index_T[i])
    for i in range(self.n_edge_types):
      if len(self.edge_indices[i]) > 0:
        self.edge_indices[i] = torch.vstack(self.edge_indices[i]).long().T
      else:
        self.edge_indices[i] = torch.tensor([[], []]).long()

    self.num_nodes = len(G.nodes)
    self.num_edges = len(G.edges)
    return

  def update_representation(self, data: Data) -> None:
    self.x = data.x
    return

  @abstractmethod
  def _init(self) -> None:
    raise NotImplementedError

  @abstractmethod
  def _compute_graph_representation(self) -> None:
    raise NotImplementedError

  @abstractmethod
  def get_state_enc(self, state: FrozenSet[Proposition]):
    raise NotImplementedError
