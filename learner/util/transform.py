import os
import sys

from gen_data.graphs import get_graph_data

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import random
import torch

import models
from torch import Tensor
from typing import Dict, List, Optional, Tuple
from torch_geometric.data import DataLoader, Data
from tqdm import tqdm, trange


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
  heuristic: str,
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
    # e = data.edge_index.shape[1]
    if heuristic != "opt":
       if heuristic not in data.heuristics:
          continue
       data.y = data.heuristics[heuristic]
    y = data.y

    if 0 < n_hi < n:  # upper bound on nodes
        continue

    if n < n_lo:  # lower bound on nodes
        continue

    if 0 < c_hi < y:  # upper bound on cost
        continue

    if y < c_lo:  # lower bound on cost
        continue

    # if e == 0:  # no edges
    #     continue

    if n == 0:  # no nodes
        continue

    new_data_list.append(data)
  data_list = new_data_list

  # model preprocessing
  if model_name == "FFNet":
      data_list = [models.ffnet.transform(data) for data in data_list]
  elif model_name == "PPGN":
      print("Transforming data into dense matrices for PPGN.")

      # max_nodes so we can batch (not used)
      max_nodes = 0
      for data in data_list:
          max_nodes = max(max_nodes, data.x.shape[0])
      print("max nodes:", max_nodes)

      new_data = []
      for data in tqdm(data_list):
          x = models.ppgn.preprocess_data_PPGN(x=data.x, edge_index=data.edge_index, max_nodes=max_nodes)
          new_data.append(Data(x=x, edge_index=None, y=data.y))

      data_list = new_data

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
