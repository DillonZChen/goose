import os
import time
import torch
import networkx as nx
from typing import FrozenSet, List, NamedTuple, TypeVar, Tuple, Dict, Optional
from scipy.sparse.linalg import ArpackError
from torch_geometric.data import DataLoader, Data
from torch_geometric.utils.convert import to_networkx, from_networkx
from torch_geometric.transforms import AddLaplacianEigenvectorPE
from abc import ABC, abstractmethod
from tqdm.auto import tqdm


""" Module for appending additional node features. Used in thesis but not in 24-AAAI. """


RNI_SIZE = [1, 4, 0.5]
RNI_DIST = ["u", "n"]


def add_none(dataset: List[Data], args) -> List[Data]:
  return dataset


def add_rni(dataset: List[Data], args) -> List[Data]:
  ret = []
  distribution = args.rni_dist
  rni_size = args.rni_size
  for data in tqdm(dataset):
    n = data.x.shape[0]
    if rni_size in {1, 4}:
      nfeat = int(rni_size)
    else:
      nfeat = int(round(0.5*data.x.shape[1]))
    if distribution == "u":  # [-1, 1]
      rni = (-2 * torch.rand(n, nfeat)) + 1
    elif distribution == "n":  # N(0, 1)
      rni = torch.normal(mean=torch.zeros(n, nfeat), std=torch.ones(n, nfeat))
    else:
      raise ValueError(f"Invalid rni distribution: {distribution}")
    x=torch.cat([data.x, rni], dim=1)
    # data.x = x
    new_data = Data(x=x, y=data.y, edge_index=data.edge_index, domain=data.domain, problem=data.problem)
    ret.append(new_data)
  return ret


def add_gde(dataset: List[Data], args) -> List[Data]:
  ret = []
  for data in tqdm(dataset):
    n = data.x.shape[0]
    G = to_networkx(data)
    goal_nodes = set()
    for node in G.nodes:
      # TODO: extract goal nodes
      raise NotImplementedError
    pe = torch.zeros((n, 1))
    for g in goal_nodes:
      paths = nx.shortest_path_length(G, target=g)
      for i in paths:
        pe[i] += 1 / (paths[i] + 1)
    x = torch.cat([data.x, pe], dim=1)
    new_data = Data(x=x, y=data.y, edge_index=data.edge_index)
    ret.append(new_data)
  return ret


def add_lpe(dataset: List[Data], args) -> List[Data]:
  k = args.lpe_k
  ret = []
  # all our representations are undirected
  transform = AddLaplacianEigenvectorPE(k=k, attr_name=None, is_undirected=False)
  for data in tqdm(dataset):
    original_e = data.edge_index
    data2 = data
    if "-el" in args.rep:
      data2.edge_index = torch.hstack(data2.edge_index)
    try:
      data = transform(data2)
      data.edge_index = original_e
      ret.append(data)
    except Exception as e:
      # print(f"Encountered following error:")
      # print(type(e))
      # print(str(e))
      # print(f"For the following graph:")
      # print(data)
      save_dir = f"logs/errors"
      os.makedirs(save_dir, exist_ok=True)
      rep = args.rep
      save_file = f"{save_dir}/AddLaplacianEigenvectorPE{k}_failed_{rep}_{data.domain}_{data.problem}.data"
      error_log = f"{str(type(e))}\n{str(e)}"
      torch.save((data, error_log), f=save_file)
      # print(f"Graph saved to {save_file}")
  return ret


def add_features(dataset: List[Data], args):
  feature = args.features
  print(f"Adding {feature} features.")
  t = time.time()
  ret = NODE_FEAT[feature](dataset, args)
  print(f"Time to add {feature} features: {time.time() - t:.2f}")
  return ret


NODE_FEAT = {
  "none": add_none,
  "rni": add_rni,
  "gde": add_gde,
  "lpe": add_lpe
}
