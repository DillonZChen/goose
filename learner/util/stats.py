import numpy as np
import os
import torch
import networkx as nx
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
from tqdm import tqdm
from torch_geometric.utils.convert import to_networkx
from torch_geometric.data import Data
from util.metrics import eval_admissibility, eval_f1_score

""" Module containing methods originall used in thesis inference experiments. """


def pyg_graph_diameter(x: torch.tensor, edge_index: torch.tensor) -> int:
  G = to_networkx(Data(x=x, edge_index=edge_index)).to_undirected()
  
  diameter = max(nx.diameter(G.subgraph(comp)) for comp in nx.connected_components(G))
  
  return diameter


def graph_density(n: int, e: int, directed: bool) -> float:
  if n == 0 or n == 1:
    return 0
  d = float(e) / float(n * (n - 1))
  if not directed:
    d *= 2
  return d


def print_quartile_desc(desc):
  print("{0:<20} {1:>10} {2:>10} {3:>10} {4:>10} {5:>10}".format(desc, "Q1", "median", "Q3", "min", "max"))
  return


def get_quartiles(data):
  q1 = np.percentile(data, 25)
  q2 = np.percentile(data, 50)
  q3 = np.percentile(data, 75)
  return q1, q2, q3


def print_quartiles(desc: str, data: np.array, floats: bool = False):
  q1, q2, q3 = get_quartiles(data)
  if floats:
    print(f"{desc:<20} {q1:>10.3f} {q2:>10.3f} {q3:>10.3f} {min(data):>10.3f} {max(data):>10.3f}")
  else:
    data = np.round(data).astype(int)
    print(f"{desc:<20} {q1:>10} {q2:>10} {q3:>10} {min(data):>10} {max(data):>10}")


def get_stats(dataset, desc=""):
  if len(dataset) == 0:
    return
  cnt = {}
  max_cost = 0
  graph_nodes = []
  graph_edges = []
  graph_dense = []
  ys = []

  for data in dataset:
    if type(dataset[0]) == tuple:  # CGraphs
      graph, y = data
      n_nodes = len(graph.nodes)
      n_edges = len(graph.edges)
    else:  # TGraphs
      y = data.y
      n_nodes = data.x.shape[0] if data.x is not None else 0
      try:
        n_edges = data.edge_index.shape[1]
      except:
        n_edges = sum(e.shape[1] for e in data.edge_index)
      density = graph_density(n_nodes, n_edges, directed=True)

    if y not in cnt:
      cnt[y] = 0
    cnt[y] += 1
    max_cost = max(max_cost, round(y))
    density = graph_density(n_nodes, n_edges, directed=True)
    graph_nodes.append(n_nodes)
    graph_edges.append(n_edges)
    graph_dense.append(density)
    ys.append(y)

  # Statistics
  print_quartile_desc(desc)
  print_quartiles("costs:", ys)
  print_quartiles("n_nodes:", graph_nodes)
  print_quartiles("n_edges:", graph_edges)
  print_quartiles("density:", graph_dense, floats=True)

  return
