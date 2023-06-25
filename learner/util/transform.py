import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import random
import torch

import models
from torch import Tensor
from typing import Dict, List, Optional, Tuple
from torch_geometric.data import DataLoader, Data
from tqdm import tqdm, trange
from dataset.graphs import get_graph_data


def extract_testset_domain(
  dataset: List[Data],
  domain: str,
):
  if domain=="":
    return dataset, []
  keep_data = []
  extr_data = []
  for graph in dataset:
    if domain in graph.domain.replace("-", ""):
      extr_data.append(graph)
    else:
      keep_data.append(graph)
  return keep_data, extr_data


def extract_testset_ipc(
  dataset: List[Data],
  ipc: str,
):
  if ipc=="":
    return dataset, []
  keep_data = []
  extr_data = []
  for graph in dataset:
    if ipc in graph.domain:
      extr_data.append(graph)
    else:
      keep_data.append(graph)
  return keep_data, extr_data



def sample_strategy(
  data_list: List[Data],
  strategy: str,
) -> List[Data]:
  random.seed(2929)

  ret = []
  if strategy == "entire":
    ret = data_list
  else:
    graph = {}
    for data in data_list:
      k = (data.domain, data.problem)
      if k not in graph:
        graph[k] = []
      graph[k].append(data)

    if strategy == "init":
      for k in graph:
        init_state = sorted(graph[k], key=lambda g: g.y, reverse=True)[0]
        ret.append(init_state)
    elif strategy == "random":
      for k in graph:
        random_state = random.choice(graph[k])
        ret.append(random_state)
    else:
      raise ValueError(strategy)

  print(f"Train size after {strategy} sample strategy: {len(ret)}")

  return ret



def preprocess_data(
  model_name: Optional[str],
  data_list: List[Data],
  small_train: bool,
  n_hi: int,
  c_hi: int,
  n_lo: int=-1,
  c_lo: int=-1,
) -> List[Data]:
  new_data_list = []
  print("Preprocessing data...")

  for data in data_list:
    n = data.x.shape[0]
    edges = data.edge_index
    if type(edges) == list:  # edge label implementation
      e = sum(edge.shape[1] for edge in edges)
    else:
      e = edges.shape[1]
    y = data.y

    if 0 < n_hi < n:  # upper bound on nodes
        continue

    if n < n_lo:  # lower bound on nodes
        continue

    if 0 < c_hi < y:  # upper bound on cost
        continue

    if y < c_lo:  # lower bound on cost
        continue

    if n == 0:  # no nodes
        continue

    new_data_list.append(data)
  data_list = new_data_list

  if small_train:
      random.seed(123)
      data_list = random.sample(data_list, k=1000)

  # collect dataset stats
  domain = {}
  problem = {}
  for data in data_list:
    d = data.domain
    if d not in domain:
      domain[d] = 0
      problem[d] = set()
    domain[d] += 1
    problem[d].add(data.problem)

  col_l = 40
  col_l2 = 10
  print("domain"+" "*(col_l-len("domain")), "num_data"+" "*(col_l2-len("num_data")), "num_plans")
  for d in sorted(list(domain.keys())):
      n_data = str(domain[d])
      problems = problem[d]
      d = d.replace("visitall-multidimensional", "visitall").replace("-dim-visitall", "D")
      print(d+" "*(col_l-len(d)), n_data+" "*(col_l2-len(n_data)), len(problems))

  print(f"Data size after preprocessing: {len(data_list)}")
  return data_list
