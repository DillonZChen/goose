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

from typing import Set, FrozenSet, List, NamedTuple, TypeVar, Tuple, Dict, Optional, Union
from torch import Tensor
from planning.translate.instantiate import instantiate, explore
from enum import Enum
from collections import OrderedDict
from dataset import get_domain_name, get_problem_name
from planning import get_planning_problem
from planning import Proposition
from util.stats import graph_density
from torch_geometric.loader import DataLoader
from torch_geometric.data import Data
from torch_geometric.utils.convert import to_networkx, from_networkx
from tqdm import tqdm
from abc import ABC, abstractmethod
from tqdm.auto import tqdm

State = List[Proposition]


""" Base class for graph representations """
class Representation(ABC):

  @property
  def name(self):
    raise NotImplementedError

  @property
  def n_node_features(self):
    raise NotImplementedError

  @property
  def n_edge_labels(self):
    raise NotImplementedError

  @property
  def directed(self):
    raise NotImplementedError

  @property
  def lifted(self):
    raise NotImplementedError

  def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
    self.domain_pddl = domain_pddl
    self.problem_pddl = problem_pddl

    self.problem = get_planning_problem(
      domain_pddl=self.domain_pddl,
      problem_pddl=self.problem_pddl,
      fdr=self.name=="flg"
    )

    t = time.time()
    self._compute_graph_representation()
    self.num_nodes = len(self.G.nodes)
    self.num_edges = len(self.G.edges)
    self._dump_stats(t)
    return

  def _create_graph(self) -> Union[nx.Graph, nx.DiGraph]:
    """ Initialises a networkx graph """
    return nx.DiGraph() if self.directed else nx.Graph()

  def _one_hot_node(self, index, size=-1) -> Tensor:
    """ Returns a one hot tensor """
    if size==-1:
      ret = torch.zeros(self.n_node_features)
    else:
      ret = torch.zeros(size)
    ret[index] = 1
    return ret

  def _zero_node(self) -> Tensor:
    """ Returns a tensor of zeros """
    ret = torch.zeros(self.n_node_features)
    return ret

  def _dump_stats(self, start_time) -> None:
    """ Dump stats for graph construction """
    assert self.name is not None
    tqdm.write(f'{self.name} created!')
    tqdm.write(f'time taken: {time.time() - start_time:.4f}s')
    tqdm.write(f'num nodes: {self.num_nodes}')
    tqdm.write(f'num edges: {self.num_edges}')
    tqdm.write(f'graph density: {graph_density(self.num_nodes, self.num_edges, directed=self.directed)}')
    return
  
  def convert_to_pyg(self) -> None:
    """ Converts networkx graph object into pytorch_geometric tensors. 

        The tensors are (x, edge_index or edge_indices)
        x: torch.tensor(N x F)  # N = num_nodes, F = num_features
        if n_edge_labels = 1:
          edge_index: torch.tensor(2 x E)  # E = num_edges
        else:
          edge_indices: List[torch.tensor(2 x E_i)]
    """

    pyg_G = from_networkx(self.G)
    self.x = pyg_G.x
    
    if self.n_edge_labels == 1:
      self.edge_index = pyg_G.edge_index
    else:
      assert self.n_edge_labels > 1
      self.edge_indices = [[] for _ in range(self.n_edge_labels)]
      edge_index_T = pyg_G.edge_index.T
      for i, edge_type in enumerate(pyg_G.edge_type):
        self.edge_indices[edge_type].append(edge_index_T[i])
      for i in range(self.n_edge_labels):
        if len(self.edge_indices[i]) > 0:
          self.edge_indices[i] = torch.vstack(self.edge_indices[i]).long().T
        else:
          self.edge_indices[i] = torch.tensor([[], []]).long()
    return
  
  @abstractmethod
  def _compute_graph_representation(self) -> None:
    raise NotImplementedError

  @abstractmethod
  def get_state_enc(self, state: State):
    raise NotImplementedError
