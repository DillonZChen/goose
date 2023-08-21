import time
from typing import Optional, Dict
from .base_kernel import *


class WeisfeilerLehmanKernel(Kernel):
  def __init__(self, iterations: int) -> None:
    super().__init__()

    # hashes neighbour multisets of colours
    self._hash = {}

    # number of wl iterations
    self.iterations = iterations

  def _get_hash_value(self, colour) -> int:
    if self._train:
      if colour not in self._hash:
        self._hash[colour] = len(self._hash)
      return self._hash[colour]
    else:
      if colour in self._hash:
        return self._hash[colour]
      else:
        return -1
  
  def compute_histograms(self, graphs: List[CGraph]) -> Dict[CGraph, Histogram]:
    """ Read graphs and return histogram. 
    
        self._train value determines if new colours are stored or not
    """

    histograms = {}

    # compute colours and hashmap from training data
    for G in graphs:
      cur_colours = {}
      histogram = {}

      def store_colour(colour):
        nonlocal histogram
        colour_hash = self._get_hash_value(colour)
        if colour_hash not in histogram:
          histogram[colour_hash] = 0
        histogram[colour_hash] += 1

      # collect initial colours
      for u in G.nodes:

        # initial colour is feature of the node
        colour = G.nodes[u]["colour"]
        cur_colours[u] = self._get_hash_value(colour)
        store_colour(colour)

      # WL iterations
      for itr in range(self.iterations):
        new_colours = {}
        for u in G.nodes:

          # edge label WL variant
          neighbour_colours = []
          for v in G[u]:
            colour_node = cur_colours[v]
            colour_edge = G.edges[(u,v)]["edge_label"]
            neighbour_colours.append((colour_node, colour_edge))
          neighbour_colours = sorted(neighbour_colours)
          colour = tuple([cur_colours[u]] + neighbour_colours)
          new_colours[u] = self._get_hash_value(colour)
          store_colour(colour)

        cur_colours = new_colours
      
      # store histogram of graph colours
      histograms[G] = histogram

    return histograms

  def get_x(
    self, 
    graphs: CGraph, 
    histograms: Optional[Dict[CGraph, Histogram]]=None
  ) -> np.array:
    """ Explicit feature representation
        O(nd) time; n x d output 
    """

    n = len(graphs)
    d = len(self._hash)
    X = np.zeros((n, d))

    if histograms is None:
      histograms = self.compute_histograms(graphs)
    else:
      histograms = histograms

    for i, G in enumerate(graphs):
      histogram = histograms[G]
      for j in histogram:
        if 0 <= j and j < d:
          X[i][j] = histogram[j]

    return X
  
  def get_k(
    self, 
    graphs: CGraph,
    histograms: Dict[CGraph, Histogram]
  ) -> np.array:
    """ Implicit feature representation
        O(n^2d) time; n x n output 
    """

    n = len(graphs)
    K = np.zeros((n, n))
    for i in range(n):
      for j in range(i, n):
        k = 0

        histogram_i = histograms[graphs[i]]
        histogram_j = histograms[graphs[j]]

        common_colours = set(histogram_i.keys()).intersection(set(histogram_j.keys()))
        for c in common_colours:
          k += histogram_i[c] * histogram_j[c]

        K[i][j] = k
        K[j][i] = k

    return K
  
  @property
  def n_colours_(self) -> int:
    return len(self._hash)