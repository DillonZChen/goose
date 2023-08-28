import sys
import torch
import networkx as nx
import copy
import time
import os
import util
import random
import hashlib

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

# state is a list of facts represented as strings
State = List[Proposition]

# graph representation represented as a tensor for GNNs
TGraph = Union[Tuple[Tensor, Tensor], Tuple[Tensor, List[Tensor]]]

# graph representation represented as a nx.graph for graph kernels
CGraph = Union[nx.Graph, nx.DiGraph]

# additional hard coded colours
ACTIVATED_COLOUR = "-1"
ACTIVATED_POS_GOAL_COLOUR_SUFFIX = "-pos-node"
ACTIVATED_NEG_GOAL_COLOUR_SUFFIX = "-neg-node"


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
    self._pos_goal_nodes = set()
    self._neg_goal_nodes = set()
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
    """ Converts nx graph into pytorch_geometric tensors and stores them. 

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
      for i, edge_label in enumerate(pyg_G.edge_label):
        self.edge_indices[edge_label].append(edge_index_T[i])
      for i in range(self.n_edge_labels):
        if len(self.edge_indices[i]) > 0:
          self.edge_indices[i] = torch.vstack(self.edge_indices[i]).long().T
        else:
          self.edge_indices[i] = torch.tensor([[], []]).long()
    return
  
  def convert_to_coloured_graph(self) -> None:
    """ Converts nx graph into another nx graph but with colours instead of vector features. 

        Vector features are converted to colours with a hash. This can be hardcoded slightly more
        efficiently for each graph representation separately but takes more effort.

        Slightly optimised by converting node string names into ints and storing the map
    """

    colours = set()

    self._name_to_node = {}
    self._node_to_name = {}

    c_graph = self._create_graph()
    for node in self.G.nodes:
      feature = self.G.nodes[node]['x'].tolist()
      feature = str(tuple(feature))
      if self.name == "llg" and type(node) == tuple and len(node)==2 and \
        type(node[1]) == str and "var-" in node[1]:
        index = node[1].split('-')[-1]
        colour = -int(index)  # colour of IF is negative of index
      else:
        colour = hashlib.sha256(feature.encode('utf-8')).hexdigest()
      colours.add(colour)

      assert node not in c_graph.nodes
      idx = len(self._name_to_node)
      self._name_to_node[node] = idx
      self._node_to_name[idx] = node
      c_graph.add_node(idx, colour=colour)
    for edge in self.G.edges:
      u, v = edge
      u = self._name_to_node[u]
      v = self._name_to_node[v]
      assert u in c_graph.nodes
      assert v in c_graph.nodes
      c_graph.add_edge(u_of_edge=u, v_of_edge=v, edge_label=self.G.edges[edge]["edge_label"])

    self.c_graph = c_graph
    return
  
  def write_to_file(self) -> None:
    from datetime import datetime
    df = self.domain_pddl
    pf = self.problem_pddl
    t = datetime.now().strftime('%Y-%m-%d-%H-%M-%S')
    file_path = "_".join(["graph", df, pf, t])
    file_path = file_path.replace("/","-").replace(".pddl","").replace(".","")
    file_path = file_path + ".graph"

    f = open(file_path, 'w')

    # line number = node
    # <node_name> <node_colour> [<neighbour_node> <edge_label>]
    G = self.c_graph
    f.write(f"{len(G.nodes)} nodes\n")
    for u in G.nodes:
      node_name = str(self._node_to_name[u]).replace(' ', '')
      f.write(f"{node_name} {G.nodes[u]['colour']} ")
      for v in G[u]:
        f.write(f"{v} {G[u][v]['edge_label']} ")
      f.write("\n")

    f.close()

    self._graph_file_path = file_path
    return
  
  def get_graph_file_path(self) -> str:
    return self._graph_file_path
  
  @abstractmethod
  def _compute_graph_representation(self) -> None:
    raise NotImplementedError

  @abstractmethod
  def state_to_tensor(self, state: State) -> TGraph:
    raise NotImplementedError

  @abstractmethod
  def state_to_cgraph(self, state: State) -> CGraph:
    raise NotImplementedError
